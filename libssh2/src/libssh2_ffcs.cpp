#if 0
#ifdef HAVE_WINSOCK2_H
# include <winsock2.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
# include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
# include <netinet/in.h>
#endif
#ifdef HAVE_SYS_SELECT_H
# include <sys/select.h>
#endif
# ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_ARPA_INET_H
# include <arpa/inet.h>
#endif

#ifndef WIN32
#include <sys/time.h>
#include <sys/socket.h>
#endif

#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include "Config.h"
#endif
#ifndef WIN32
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>	
#include <errno.h>
#endif

#include "libssh2_ffcs.h"
#include <stdio.h>
#include <time.h>

//#define _LOG_SSH_FAIL_

#ifdef _LOG_SSH_FAIL_
	#include "Log.h"
	extern CLog* g_pLog;
#endif
using namespace std;

static int waitsocket(int socket_fd, LIBSSH2_SESSION *session)
{
    struct timeval timeout;
    int rc;
    fd_set fd;
    fd_set *writefd = NULL;
    fd_set *readfd = NULL;
    int dir;

    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    FD_ZERO(&fd);

    FD_SET(socket_fd, &fd);

    /* now make sure we wait in the correct direction */
    dir = libssh2_session_block_directions(session);

    if(dir & LIBSSH2_SESSION_BLOCK_INBOUND)
        readfd = &fd;

    if(dir & LIBSSH2_SESSION_BLOCK_OUTBOUND)
        writefd = &fd;

    rc = select(socket_fd + 1, readfd, writefd, NULL, &timeout);

    return rc;
}

int ssh_exec(string& r_strResult, const char* hostname, const char* username, 
			 const char* password, const char* commandline)
{
	LIBSSH2_SESSION* p_session;
	int sock;
	int result = sbs_ssh_exec_open(hostname, username, password, &p_session, &sock);
	
	if ( result == 0 )
	{
		result = sbs_ssh_exec(r_strResult, p_session, sock, commandline);
		
		sbs_ssh_exec_close(p_session, sock);

		return result;
	}
	else
	{
		return result;
	}
}

#if 0
//以下代码为测试过的可以使用的代码,但是为了使用三阶段的函数,所以正式的代码是上面一个函数
int ssh_exec(string& r_strResult, const char* hostname, const char* username, 
			 const char* password, const char* commandline)
{
	char *exitsignal;
	int exitcode;

#ifdef WIN32
    WSADATA wsadata;
    WSAStartup(MAKEWORD(2,0), &wsadata);
#endif
    int rc = libssh2_init (0);

    if (rc != 0) 
	{
        fprintf (stderr, "libssh2 initialization failed (%d)\n", rc);
#ifdef WIN32
		WSACleanup();
#endif	
        return 1;
    }

	/* Ultra basic "connect to port 22 on localhost"
     * Your code is responsible for creating the socket establishing the
     * connection
     */
	struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(22);
    sin.sin_addr.s_addr = inet_addr(hostname);
	int sock = socket(AF_INET, SOCK_STREAM, 0);

	if (connect(sock, (struct sockaddr*)(&sin), sizeof(struct sockaddr_in)) != 0) 
	{
        fprintf(stderr, "failed to connect, %s!\n",hostname);

#ifdef WIN32
    closesocket(sock);
#else
    close(sock);
#endif

	//fprintf(stderr, "all done\n");
		libssh2_exit();
#ifdef WIN32
		WSACleanup();
#endif	

        return -1;
    }

    /* Create a session instance */
    LIBSSH2_SESSION* session = libssh2_session_init();
    
	if (!session)
    {
#ifdef _LOG_SSH_FAIL_		
		g_pLog->LOG1("libssh2_session_init fail!\n");
#endif	    
		#ifdef WIN32
    		closesocket(sock);
		#else
   			close(sock);
		#endif
			libssh2_exit();
#ifdef WIN32
			WSACleanup();
#endif	
        return -1;
    }
    /* tell libssh2 we want it all done non-blocking */
    libssh2_session_set_blocking(session, 0);

	time_t tBegin = time(NULL);

	while ((rc = libssh2_session_startup(session, sock)) ==
		LIBSSH2_ERROR_EAGAIN)
	{
		if(time(NULL) > tBegin + 30)
			break;
	}
 
    //rc = libssh2_session_startup(session, sock);
    if (rc) 
	{
        fprintf(stderr, "Failure establishing SSH session: %d\n", rc);
		#ifdef WIN32
			closesocket(sock);
		#else
			close(sock);
		#endif
			libssh2_session_free(session);
			libssh2_exit();
#ifdef WIN32
			WSACleanup();
#endif	
        return -1;
    }

    LIBSSH2_KNOWNHOSTS* nh = libssh2_knownhost_init(session);

    if(!nh) 
	{
		#ifdef WIN32
			closesocket(sock);
		#else
			close(sock);
		#endif
			libssh2_session_disconnect(session, "libssh2_knownhost_init error");
			libssh2_session_free(session);
			libssh2_exit();
#ifdef WIN32
			WSACleanup();
#endif	
        return 2;
    }

    /* read all hosts from here */
    libssh2_knownhost_readfile(nh, "known_hosts",
                               LIBSSH2_KNOWNHOST_FILE_OPENSSH);

    /* store all known hosts to here */
    libssh2_knownhost_writefile(nh, "dumpfile",
                                LIBSSH2_KNOWNHOST_FILE_OPENSSH);
	size_t len;
	int type;

    const char* fingerprint = libssh2_session_hostkey(session, &len, &type);
	
	if (fingerprint) 
	{
		int check = 0;
        struct libssh2_knownhost *host;
#if LIBSSH2_VERSION_NUM >= 0x010206
        /* introduced in 1.2.6 */
        check = libssh2_knownhost_checkp(nh, hostname, 22,
                                             fingerprint, len,
                                             LIBSSH2_KNOWNHOST_TYPE_PLAIN|
                                             LIBSSH2_KNOWNHOST_KEYENC_RAW,
                                             &host);
#else
        /* 1.2.5 or older */
        check = libssh2_knownhost_check(nh, hostname,
                                            fingerprint, len,
                                            LIBSSH2_KNOWNHOST_TYPE_PLAIN|
                                            LIBSSH2_KNOWNHOST_KEYENC_RAW,
                                            &host);
#endif
        //fprintf(stderr, "Host check: %d, key: %s\n", check,
        //        (check <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH)?
        //        host->key:"<none>");

        /*****
         * At this point, we could verify that 'check' tells us the key is
         * fine or bail out.
         *****/
    }
    else {
        /* eeek, do cleanup here */
#ifdef _LOG_SSH_FAIL_		
		g_pLog->LOG1("libssh2_knownhost_check fail: %d\n", check);
#endif			
#ifdef WIN32
		closesocket(sock);
#else
		close(sock);
#endif
		libssh2_knownhost_free(nh);
		libssh2_session_disconnect(session, "libssh2_session_hostkey error");
		libssh2_session_free(session);

		//fprintf(stderr, "all done\n");
		libssh2_exit();
#ifdef WIN32
		WSACleanup();
#endif	
        return 3;
    }
    libssh2_knownhost_free(nh);

    if ( strlen(password) != 0 ) 
	{
        /* We could authenticate via password */
		//需要做超时限制,30秒超时
		time_t tBegin = time(NULL);
        
		while ((rc = libssh2_userauth_password(session, username, password)) ==
               LIBSSH2_ERROR_EAGAIN)
		{
			if(time(NULL) > tBegin + 30)
				break;
		}

		
        if (rc) {
            fprintf(stderr, "Authentication by password failed.\n");
#ifdef _LOG_SSH_FAIL_		
			g_pLog->LOG1("Authentication by password failed.\n");
#endif				
            goto shutdown;
        }
    }
    else {
        /* Or by public key */
		time_t tBegin = time(NULL);
        while ((rc = libssh2_userauth_publickey_fromfile(session, username,
                                                         "/home/user/"
                                                         ".ssh/id_rsa.pub",
                                                         "/home/user/"
                                                         ".ssh/id_rsa",
                                                         password)) ==
               LIBSSH2_ERROR_EAGAIN)
        {
			if(time(NULL) > tBegin + 30)
				break;
		}
		
        if (rc) {
            fprintf(stderr, "\tAuthentication by public key failed\n");
#ifdef _LOG_SSH_FAIL_		
			g_pLog->LOG1("\tAuthentication by public key failed\n");
#endif				
            goto shutdown;
        }
    }

    /* Exec non-blocking on the remove host */
	tBegin = time(NULL);
	LIBSSH2_CHANNEL *channel;

    while( (channel = libssh2_channel_open_session(session)) == NULL &&
           libssh2_session_last_error(session,NULL,NULL,0) ==
           LIBSSH2_ERROR_EAGAIN )
    {
        waitsocket(sock, session);
		if (time(NULL) > tBegin + 30)
			break;
    }
	
    if( channel == NULL )
    {
        fprintf(stderr,"libssh2_channel_open_session failed\n");
		libssh2_session_disconnect(session, "libssh2_channel_open_session error");
		libssh2_session_free(session);

#ifdef WIN32
		closesocket(sock);
#else
		close(sock);
#endif
		//fprintf(stderr, "all done\n");
		libssh2_exit();
#ifdef WIN32
		WSACleanup();
#endif	

        return -1;
    }

	tBegin = time(NULL);
    while( (rc = libssh2_channel_exec(channel, commandline)) ==
           LIBSSH2_ERROR_EAGAIN )
    {
        waitsocket(sock, session);
		if(time(NULL) > tBegin + 30)
			break;		
    }

    if( rc != 0 )
    {
        fprintf(stderr,"libssh2_channel_exec failed\n");
		libssh2_channel_close(channel);
		libssh2_session_disconnect(session, "libssh2_channel_open_session error");
		libssh2_session_free(session);

#ifdef WIN32
		closesocket(sock);
#else
		close(sock);
#endif
		//fprintf(stderr, "all done\n");
		libssh2_exit();
#ifdef WIN32
		WSACleanup();
#endif	

        return -1;
    }

	//这里可能长时间block，未添加超时处理
    for( ;; )
    {
        /* loop until we block */
        int rc;
        
		do
        {
            //char *buffer[1024*256];
            char *buffer = new char[1024*256];
            rc = libssh2_channel_read( channel, buffer, 1024*256 );
            if( rc > 0 )
            {
				r_strResult += string(buffer,rc);
            }
			
            else 
			{
                //fprintf(stderr, "libssh2_channel_read returned %d\n", rc);
            }
			delete []buffer;
        }
        while( rc > 0 );

        /* this is due to blocking that would occur otherwise so we loop on
           this condition */
        if( rc == LIBSSH2_ERROR_EAGAIN )
        {
            waitsocket(sock, session);
        }
        else
            break;
    }
    
	exitcode = 127;

    while ( (rc = libssh2_channel_close(channel)) == LIBSSH2_ERROR_EAGAIN )
	{
        waitsocket(sock, session);
	}

	exitsignal=(char *)"none";

    if ( rc == 0 )
    {
        exitcode = libssh2_channel_get_exit_status( channel );
        libssh2_channel_get_exit_signal(channel, &exitsignal,
                                        NULL, NULL, NULL, NULL, NULL);
    }

    if (exitsignal)
    {
        //printf("\nGot signal: %s\n", exitsignal);
#ifdef _LOG_SSH_FAIL_		
		g_pLog->LOG1("\nGot signal: %s\n", exitsignal);
#endif 	
    }
    else
    {
        //printf("\nEXIT: %d bytecount: %d\n", exitcode, bytecount);
#ifdef _LOG_SSH_FAIL_		
		g_pLog->LOG1("\nEXIT: %d bytecount: %d\n", exitcode, bytecount);
#endif 	
    }
	
    libssh2_channel_free(channel);
    channel = NULL;

shutdown:

    libssh2_session_disconnect(session,
                               "Normal Shutdown, Thank you for playing");
    libssh2_session_free(session);

#ifdef WIN32
    closesocket(sock);
#else
    close(sock);
#endif
    //fprintf(stderr, "all done\n");

    libssh2_exit();
#ifdef WIN32
	WSACleanup();
#endif	

    return 0;
}
#endif

int sbs_ssh_exec_open(const char* hostname, const char* username, const char* password,
					  LIBSSH2_SESSION** psession,int* psock)
{
#ifdef WIN32
    WSADATA wsadata;
    WSAStartup(MAKEWORD(2,0), &wsadata);
#endif
    int rc = libssh2_init (0);

    if (rc != 0) 
	{
        fprintf (stderr, "libssh2 initialization failed (%d)\n", rc);
#ifdef WIN32
		WSACleanup();
#endif	
        return 1;
    }

    /* Ultra basic "connect to port 22 on localhost"
     * Your code is responsible for creating the socket establishing the
     * connection
     */
	struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(22);
    sin.sin_addr.s_addr = inet_addr(hostname);
	int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (connect(sock, (struct sockaddr*)(&sin), sizeof(struct sockaddr_in)) != 0) 
	{
        fprintf(stderr, "failed to connect!,%s\n",hostname);
#ifdef WIN32
			closesocket(sock);
#else
			close(sock);
#endif
		libssh2_exit();
#ifdef WIN32
		WSACleanup();
#endif	

        return -1;
    }

    /* Create a session instance */
    LIBSSH2_SESSION* session = libssh2_session_init();

    if (!session)
    {
#ifdef _LOG_SSH_FAIL_		
		g_pLog->LOG1("libssh2_session_init failed!\n");
#endif 	    
#ifdef WIN32
			closesocket(sock);
#else
			close(sock);
#endif

			libssh2_exit();
#ifdef WIN32
			WSACleanup();
#endif	

        return -1;
    }
	
    /* tell libssh2 we want it all done non-blocking */
    libssh2_session_set_blocking(session, 0);

    /* ... start it up. This will trade welcome banners, exchange keys,
     * and setup crypto, compression, and MAC layers
     */
	time_t tBegin = time(NULL);

	while ((rc = libssh2_session_startup(session, sock)) ==
		LIBSSH2_ERROR_EAGAIN)
	{
		if(time(NULL) > tBegin + 30)
		{
			break;
		}
	}

    if (rc) {
        fprintf(stderr, "Failure establishing SSH session: %d\n", rc);
#ifdef _LOG_SSH_FAIL_		
		g_pLog->LOG1("Failure establishing SSH session: %d\n", rc);
#endif 	  		
#ifdef WIN32
			closesocket(sock);
#else
			close(sock);
#endif
			libssh2_session_free(session);
			libssh2_exit();
#ifdef WIN32
			WSACleanup();
#endif	

        return -1;
    }

    LIBSSH2_KNOWNHOSTS* nh = libssh2_knownhost_init(session);

    if(!nh) {
        /* eeek, do cleanup here */
#ifdef _LOG_SSH_FAIL_		
		g_pLog->LOG1("libssh2_knownhost_init failed:%d\n", nh);
#endif 		
#ifdef WIN32
			closesocket(sock);
#else
			close(sock);
#endif
			libssh2_session_disconnect(session, "libssh2_knownhost_init error");
			libssh2_session_free(session);
			libssh2_exit();
#ifdef WIN32
			WSACleanup();
#endif	

        return 2;
    }

    /* read all hosts from here */
    libssh2_knownhost_readfile(nh, "known_hosts",
                               LIBSSH2_KNOWNHOST_FILE_OPENSSH);

    /* store all known hosts to here */
    libssh2_knownhost_writefile(nh, "dumpfile",
                                LIBSSH2_KNOWNHOST_FILE_OPENSSH);

	size_t len;
	int type;

    const char* fingerprint = libssh2_session_hostkey(session, &len, &type);
	
    if ( fingerprint ) 
	{
		int check = 0;
        struct libssh2_knownhost *host;
#if LIBSSH2_VERSION_NUM >= 0x010206
        /* introduced in 1.2.6 */
        check = libssh2_knownhost_checkp(nh, hostname, 22,
                                             fingerprint, len,
                                             LIBSSH2_KNOWNHOST_TYPE_PLAIN|
                                             LIBSSH2_KNOWNHOST_KEYENC_RAW,
                                             &host);
#else
        /* 1.2.5 or older */
        check = libssh2_knownhost_check(nh, hostname,
                                            fingerprint, len,
                                            LIBSSH2_KNOWNHOST_TYPE_PLAIN|
                                            LIBSSH2_KNOWNHOST_KEYENC_RAW,
                                            &host);
#endif
        //fprintf(stderr, "Host check: %d, key: %s\n", check,
         //       (check <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH)?
         //       host->key:"<none>");

        /*****
         * At this point, we could verify that 'check' tells us the key is
         * fine or bail out.
         *****/
    }
    else {
        /* eeek, do cleanup here */
#ifdef _LOG_SSH_FAIL_		
		g_pLog->LOG1("libssh2_knownhost_check failed:%d\n", check);
#endif 			
#ifdef WIN32
			closesocket(sock);
#else
			close(sock);
#endif
		libssh2_knownhost_free(nh);
		libssh2_session_disconnect(session, "libssh2_session_hostkey error");
		libssh2_session_free(session);

		//fprintf(stderr, "all done\n");
		libssh2_exit();
#ifdef WIN32
		WSACleanup();
#endif

        return 3;
    }

    libssh2_knownhost_free(nh);

    if ( strlen(password) != 0 ) {
        /* We could authenticate via password */
		time_t tBegin = time(NULL);
        while ((rc = libssh2_userauth_password(session, username, password)) ==
               LIBSSH2_ERROR_EAGAIN)
        {
			if(time(NULL)> tBegin + 30)
				break;
		}
		
        if (rc) {
            fprintf(stderr, "Authentication by password failed.\n");
#ifdef _LOG_SSH_FAIL_		
			g_pLog->LOG1("Authentication by password failed.\n");
#endif 			
            goto shutdown;
        }
    }
    else {
        /* Or by public key */
		time_t tBegin = time(NULL);
        while ((rc = libssh2_userauth_publickey_fromfile(session, username,
                                                         "/home/user/"
                                                         ".ssh/id_rsa.pub",
                                                         "/home/user/"
                                                         ".ssh/id_rsa",
                                                         password)) ==
               LIBSSH2_ERROR_EAGAIN)
        {
			if(time(NULL) > tBegin + 30)
				break;
		}
		
        if (rc) 
		{
            fprintf(stderr, "\tAuthentication by public key failed\n");
#ifdef _LOG_SSH_FAIL_		
			g_pLog->LOG1("\tAuthentication by public key failed\n");
#endif 				
            goto shutdown;
        }
    }

#if 0
    libssh2_trace(session, ~0 );
#endif

	*psession = session;	
	*psock = sock;

	return 0;

shutdown:

    libssh2_session_disconnect(session,
                               "Normal Shutdown, Thank you for playing");
    libssh2_session_free(session);

#ifdef WIN32
    closesocket(sock);
#else
    close(sock);
#endif
    //fprintf(stderr, "all done\n");

    libssh2_exit();
#ifdef WIN32
	WSACleanup();
#endif	

    return -1;
}

int sbs_ssh_exec(string& r_strResult, LIBSSH2_SESSION *session,int sock, const char* commandline)
{
    time_t tBegin = time(NULL);
	LIBSSH2_CHANNEL* channel;

    while( (channel = libssh2_channel_open_session(session)) == NULL &&
           libssh2_session_last_error(session,NULL,NULL,0) ==LIBSSH2_ERROR_EAGAIN )
    {
        waitsocket(sock, session);

		if ( time(NULL) > tBegin + 30 )
		{
			break;
		}
    }
	
    if ( channel == NULL )
    {
        fprintf(stderr,"libssh2_channel_open_session failed\n");
        return -1;
    }

	tBegin = time(NULL);
	int rc;

    while( (rc = libssh2_channel_exec(channel, commandline)) ==
           LIBSSH2_ERROR_EAGAIN )
    {
        waitsocket(sock, session);

		if ( time(NULL) > tBegin + 30 )
		{
			break;	
		}
    }
	
    if ( rc != 0 )
    {
        fprintf(stderr,"libssh2_channel_exec failed\n");
        return -1;
    }
	
    for( ;; )
    {
        /* loop until we block */
        int rc;

        do
        {
            char buffer[0x4000];
            rc = libssh2_channel_read( channel, buffer, sizeof(buffer) );
            
			if( rc > 0 )
            {
				r_strResult += string(buffer,rc);
            }
            else {
                //fprintf(stderr, "libssh2_channel_read returned %d\n", rc);
#ifdef _LOG_SSH_FAIL_		
				g_pLog->LOG1("libssh2_channel_read returned %d\n", rc);
#endif   				
            }
        }while( rc > 0 );

        /* this is due to blocking that would occur otherwise so we loop on
           this condition */
        if ( rc == LIBSSH2_ERROR_EAGAIN )
        {
            waitsocket(sock, session);
        }
        else
            break;
    }

	int exitcode = 127;
	
    while( (rc = libssh2_channel_close(channel)) == LIBSSH2_ERROR_EAGAIN )
	{
        waitsocket(sock, session);
	}

	char *exitsignal=(char *)"none";

    if( rc == 0 )
    {
        exitcode = libssh2_channel_get_exit_status( channel );
        libssh2_channel_get_exit_signal(channel, &exitsignal,
                                        NULL, NULL, NULL, NULL, NULL);
    }

    if (exitsignal)
    {
        //printf("\nGot signal: %s\n", exitsignal);
#ifdef _LOG_SSH_FAIL_		
		g_pLog->LOG1("\nGot signal: %s\n", exitsignal);
#endif 		
    }
    else
    {
        //printf("\nEXIT: %d bytecount: %d\n", exitcode, bytecount);
#ifdef _LOG_SSH_FAIL_		
		g_pLog->LOG1("\nEXIT: %d bytecount: %d\n", exitcode, bytecount);
#endif 	
    }
	
    libssh2_channel_free(channel);
    channel = NULL;

	return 0;
}

void sbs_ssh_exec_close(LIBSSH2_SESSION* session, int sock)
{
    libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
    libssh2_session_free(session);

#ifdef WIN32
    closesocket(sock);
#else
    close(sock);
#endif
    libssh2_exit();

#ifdef WIN32
	WSACleanup();
#endif	
}

int ssh_scp(char* hostname,char* username,char* password,
	char* scppath,char* filename,char* destpath)
{
    unsigned long hostaddr;
    int sock, auth_pw = 1;
    struct sockaddr_in sin;
    const char *fingerprint;
    LIBSSH2_SESSION *session;
    LIBSSH2_CHANNEL *channel;
    //const char *username="username";
    //const char *password="password";
    //const char *scppath="/tmp/TEST";
    struct stat fileinfo;
    int rc;
    off_t got=0;
	FILE* f = NULL;

#ifdef WIN32
    WSADATA wsadata;

    WSAStartup(MAKEWORD(2,0), &wsadata);
#endif

	hostaddr = inet_addr(hostname);

    rc = libssh2_init (0);
    if (rc != 0) {
        fprintf (stderr, "libssh2 initialization failed (%d)\n", rc);
#ifdef _LOG_SSH_FAIL_		
		g_pLog->LOG1("libssh2 initialization failed (%d)\n", rc);
#endif		
        return 1;
    }

    /* Ultra basic "connect to port 22 on localhost"
     * Your code is responsible for creating the socket establishing the
     * connection
     */
    sock = socket(AF_INET, SOCK_STREAM, 0);

    sin.sin_family = AF_INET;
    sin.sin_port = htons(22);
    sin.sin_addr.s_addr = hostaddr;
    if (connect(sock, (struct sockaddr*)(&sin),
            sizeof(struct sockaddr_in)) != 0) {
        fprintf(stderr, "failed to connect!,%s\n",hostname);
#ifdef _LOG_SSH_FAIL_		
		g_pLog->LOG1("failed to connect!,%s\n",hostname);
#endif			
#ifdef WIN32
			closesocket(sock);
#else
			close(sock);
#endif

        return -1;
    }

    /* Create a session instance
     */
    session = libssh2_session_init();
    if(!session)
    {
 #ifdef _LOG_SSH_FAIL_		
		g_pLog->LOG1("libssh2_session_init failed!\n");
#endif	   
#ifdef WIN32
			closesocket(sock);
#else
			close(sock);
#endif

        return -1;
    }
	
    /* ... start it up. This will trade welcome banners, exchange keys,
     * and setup crypto, compression, and MAC layers
     */
    rc = libssh2_session_startup(session, sock);
    if(rc) {
        fprintf(stderr, "Failure establishing SSH session: %d\n", rc);
#ifdef _LOG_SSH_FAIL_		
		g_pLog->LOG1("Failure establishing SSH session: %d\n", rc);
#endif	 		
#ifdef WIN32
			closesocket(sock);
#else
			close(sock);
#endif

        return -1;
    }

    /* At this point we havn't yet authenticated.  The first thing to do
     * is check the hostkey's fingerprint against our known hosts Your app
     * may have it hard coded, may go to a file, may present it to the
     * user, that's your call
     */
    fingerprint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);
    //fprintf(stderr, "Fingerprint: ");
    //for(i = 0; i < 20; i++) {
     //   fprintf(stderr, "%02X ", (unsigned char)fingerprint[i]);
    //}
    //fprintf(stderr, "\n");

    if (auth_pw) {
        /* We could authenticate via password */
        if (libssh2_userauth_password(session, username, password)) {
            fprintf(stderr, "Authentication by password failed.\n");
#ifdef _LOG_SSH_FAIL_		
			g_pLog->LOG1("Authentication by password failed.\n");
#endif				
            goto shutdown;
        }
    } else {
        /* Or by public key */
        if (libssh2_userauth_publickey_fromfile(session, username,
                            "/home/username/.ssh/id_rsa.pub",
                            "/home/username/.ssh/id_rsa",
                            password)) {
            fprintf(stderr, "\tAuthentication by public key failed\n");
#ifdef _LOG_SSH_FAIL_		
			g_pLog->LOG1("\tAuthentication by public key failed\n");
#endif			
            goto shutdown;
        }
    }

    /* Request a file via SCP */
	char cfilepath[256];
	memset(cfilepath,0,sizeof(cfilepath));
	strcpy(cfilepath,scppath);
	strcat(cfilepath,filename);
	
    //channel = libssh2_scp_recv(session, scppath, &fileinfo);
    channel = libssh2_scp_recv(session, cfilepath, &fileinfo);

    if (!channel) {
        fprintf(stderr, "Unable to open a session: %d\n",
                libssh2_session_last_errno(session));
#ifdef _LOG_SSH_FAIL_		
		g_pLog->LOG1("Unable to open a session: %d\n",
                libssh2_session_last_errno(session));
#endif			
        goto shutdown;
    }

	char cDestFile[256];
	memset(cDestFile,0,sizeof(cDestFile));
	strcpy(cDestFile,destpath);
	strcat(cDestFile,filename);
//	CreateFold(destpath);
	f = fopen(cDestFile,"w+");

    while(got < fileinfo.st_size) {
        char mem[1024];
        int amount=sizeof(mem);

        if((fileinfo.st_size -got) < amount) {
            amount = fileinfo.st_size -got;
        }

        rc = libssh2_channel_read(channel, mem, amount);
        if(rc > 0) {
            //write(1, mem, rc);
            //write(f,mem,rc);
			fwrite(mem, sizeof(char), rc, f);
        }
        else if(rc < 0) {
            fprintf(stderr, "libssh2_channel_read() failed: %d\n", rc);
#ifdef _LOG_SSH_FAIL_		
			g_pLog->LOG1("libssh2_channel_read() failed: %d\n", rc);
#endif				
            break;
        }
        got += rc;
    }

	fclose(f);
	
    libssh2_channel_free(channel);
    channel = NULL;

 shutdown:

    libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
    libssh2_session_free(session);

#ifdef WIN32
    closesocket(sock);
#else
    close(sock);
#endif
    fprintf(stderr, "all done\n");

    libssh2_exit();

    return 0;
}

//sts --> step by step
int ssh_sftp_download_open(char* hostname,char* username,char* password,
	LIBSSH2_SESSION **psession,int *psock)
{
#ifdef WIN32
    WSADATA wsadata;
    WSAStartup(MAKEWORD(2,0), &wsadata);
#endif

    int rc = libssh2_init(0);

    if ( rc != 0 ) 
	{
        fprintf (stderr, "libssh2 initialization failed (%d)\n", rc);
#ifdef WIN32
		WSACleanup();
#endif

        return 1;
    }

    /* Ultra basic "connect to port 22 on localhost"
     * Your code is responsible for creating the socket establishing the
     * connection
     */
    int sock = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(22);
	sin.sin_addr.s_addr = inet_addr(hostname);

	if (connect(sock, (struct sockaddr*)(&sin), sizeof(struct sockaddr_in)) != 0) 
	{
        fprintf(stderr, "failed to connect!,%s\n",hostname);
#ifdef WIN32
			closesocket(sock);
#else
			close(sock);
#endif
			libssh2_exit();
#ifdef WIN32
			WSACleanup();
#endif
        return -1;
    }

    /* Create a session instance
     */
    LIBSSH2_SESSION* session = libssh2_session_init();

    if(!session)
    {
#ifdef WIN32
			closesocket(sock);
#else
			close(sock);
#endif
			libssh2_exit();
#ifdef WIN32
			WSACleanup();
#endif
        return -1;
    }
	
    /* ... start it up. This will trade welcome banners, exchange keys,
     * and setup crypto, compression, and MAC layers
     */
    rc = libssh2_session_startup(session, sock);

    if(rc) 
	{
        fprintf(stderr, "Failure establishing SSH session: %d\n", rc);
#ifdef WIN32
			closesocket(sock);
#else
			close(sock);
#endif
			libssh2_session_free(session);
			libssh2_exit();
#ifdef WIN32
			WSACleanup();
#endif

        return -1;
    }

    /* At this point we havn't yet authenticated.  The first thing to do
     * is check the hostkey's fingerprint against our known hosts Your app
     * may have it hard coded, may go to a file, may present it to the
     * user, that's your call
     */
    libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);
    //fprintf(stderr, "Fingerprint: ");
    //for(i = 0; i < 20; i++) {
    //    fprintf(stderr, "%02X ", (unsigned char)fingerprint[i]);
    //}
    //fprintf(stderr, "\n");

	int auth_pw = 1;

    if (auth_pw) {
        /* We could authenticate via password */
        if (libssh2_userauth_password(session, username, password)) {
            //fprintf(stderr, "Authentication by password failed.\n");
#ifdef _LOG_SSH_FAIL_		
			g_pLog->LOG1("Authentication by password failed.\n");
#endif	 			
            goto shutdown;
        }
    } else {
        /* Or by public key */
        if (libssh2_userauth_publickey_fromfile(session, username,
                            "/home/username/.ssh/id_rsa.pub",
                            "/home/username/.ssh/id_rsa",
                            password)) {
            //fprintf(stderr, "\tAuthentication by public key failed\n");
#ifdef _LOG_SSH_FAIL_		
			g_pLog->LOG1("\tAuthentication by public key failed\n");
#endif				
            goto shutdown;
        }
    }

	*psession = session;
	*psock = sock;
	return 0;
	
 shutdown:

    libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
    libssh2_session_free(session);

#ifdef WIN32
    closesocket(sock);
#else
    close(sock);
#endif
    //fprintf(stderr, "all done\n");

    libssh2_exit();
#ifdef WIN32
	WSACleanup();
#endif

    return -1;
}

int ssh_sftp_download(LIBSSH2_SESSION *session,char* scppath,char* filename,char* destpath)
{
	/* Request a file via SCP */
	char cfilepath[2048];
	memset(cfilepath,0,sizeof(cfilepath));
	strcpy(cfilepath,scppath);
	strcat(cfilepath,filename);

    struct stat fileinfo;
    LIBSSH2_CHANNEL* channel = libssh2_scp_recv(session, cfilepath, &fileinfo);

    if (!channel) 
	{
        fprintf(stderr, "Unable to open a session: %d\n", libssh2_session_last_errno(session));
#ifdef _LOG_SSH_FAIL_		
		g_pLog->LOG1("Unable to open a session: %d\n",
                libssh2_session_last_errno(session));
#endif			
        return -2;
    }

	//2011-11-28 add by yf 没内容的文件不拷贝
	if (fileinfo.st_size == 0)
	{
		libssh2_channel_free(channel);

		return 0;
	}

	char cDestFile[2048];
	memset(cDestFile,0,sizeof(cDestFile));
	strcpy(cDestFile,destpath);
	strcat(cDestFile,filename);
	FILE* f = fopen(cDestFile,"w+");

	if ( f == NULL )
	{
		printf("open file %s fail: errnum: %d %s",cDestFile,errno,strerror(errno));
#ifdef _LOG_SSH_FAIL_		
		g_pLog->LOG1("open file %s fail: errnum: %d %s",cDestFile,errno,strerror(errno));
#endif			
		libssh2_channel_free(channel);
		return -1;
	}

	bool isOk = true;
	off_t got=0;

    while( got < fileinfo.st_size) 
	{
        char mem[1024];
        int amount=sizeof(mem);

        if ( (fileinfo.st_size -got) < amount ) 
		{
            amount = fileinfo.st_size -got;
        }

        int rc = libssh2_channel_read(channel, mem, amount);
        
		if(rc > 0) 
		{
			fwrite(mem, sizeof(char), rc, f);
        }
        else if(rc < 0) 
		{
            fprintf(stderr, "libssh2_channel_read() failed: %d\n", rc);
#ifdef _LOG_SSH_FAIL_		
			g_pLog->LOG1("libssh2_channel_read() failed: %d\n", rc);
#endif		
			isOk = false;
            break;
        }

        got += rc;
    }

	fclose(f);
	
    libssh2_channel_free(channel);
    channel = NULL;

	if ( isOk )
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

void ssh_sftp_download_close(LIBSSH2_SESSION *session,int sock)
{
	libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
	libssh2_session_free(session);
	
#ifdef WIN32
	closesocket(sock);
#else
	close(sock);
#endif
	//fprintf(stderr, "all done\n");
	
	libssh2_exit();
#ifdef WIN32
	WSACleanup();
#endif
}

//sts --> step by step
int sbs_ssh_sftp_open(char* hostname,char* username,char* password,
	LIBSSH2_SESSION** psession,int* psock)
{
#ifdef WIN32
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2,0), &wsadata);
#endif
	int rc = libssh2_init (0);

	if (rc != 0) 
	{
		fprintf (stderr, "libssh2 initialization failed (%d)\n", rc);
#ifdef WIN32
		WSACleanup();
#endif

		return 1;
	}

	unsigned long hostaddr = inet_addr(hostname);
	
	/* Ultra basic "connect to port 22 on localhost"
	 * Your code is responsible for creating the socket establishing the
	 * connection
	 */
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(22);
	sin.sin_addr.s_addr = hostaddr;

	if (connect(sock, (struct sockaddr*)(&sin), sizeof(struct sockaddr_in)) != 0) 
	{
		fprintf(stderr, "failed to connect!,%s\n",hostname);
#ifdef WIN32
		closesocket(sock);
#else
		close(sock);
#endif

		libssh2_exit();
#ifdef WIN32
		WSACleanup();
#endif
		return -1;
	}
	
	/* Create a session instance
	 */
	LIBSSH2_SESSION* session = libssh2_session_init();

	if ( session==NULL )
	{
#ifdef WIN32
			closesocket(sock);
#else
			close(sock);
#endif

		libssh2_exit();
#ifdef WIN32
		WSACleanup();
#endif
		return -1;
	}
	
	// Since we have set non-blocking, tell libssh2 we are blocking 
    //libssh2_session_set_blocking(session, 1);

	// ... start it up. This will trade welcome banners, exchange keys,
	 // and setup crypto, compression, and MAC layers
	 // 
	//rc = libssh2_session_handshake(session, sock);

	//if(rc) {
	//	fprintf(stderr, "Failure establishing SSH session: %d\n", rc);
	//	return -1;
	//}

    /* ... start it up. This will trade welcome banners, exchange keys,
     * and setup crypto, compression, and MAC layers
     */
    rc = libssh2_session_startup(session, sock);

    if(rc) 
	{
		libssh2_session_free(session);

        fprintf(stderr, "Failure establishing SSH session: %d\n", rc);
#ifdef WIN32
			closesocket(sock);
#else
			close(sock);
#endif

		libssh2_exit();
#ifdef WIN32
		WSACleanup();
#endif
        return -1;
    }
	
	/* At this point we havn't yet authenticated.  The first thing to do
	 * is check the hostkey's fingerprint against our known hosts Your app
	 * may have it hard coded, may go to a file, may present it to the
	 * user, that's your call
	 */
	libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);
	//fprintf(stderr, "Fingerprint: ");
	//for(i = 0; i < 20; i++) {
	//	  fprintf(stderr, "%02X ", (unsigned char)fingerprint[i]);
	//}
	//fprintf(stderr, "\n");
	int auth_pw = 1;

	if (auth_pw) {
		/* We could authenticate via password */
		if (libssh2_userauth_password(session, username, password)) 
		{
			fprintf(stderr, "Authentication by password failed.\n");
			goto shutdown;
		}
	} 
	else 
	{
		/* Or by public key */
		if (libssh2_userauth_publickey_fromfile(session, username,
							"/home/username/.ssh/id_rsa.pub",
							"/home/username/.ssh/id_rsa",
							password)) {
			fprintf(stderr, "\tAuthentication by public key failed\n");
			goto shutdown;
		}
	}
	
	*psession = session;
	*psock = sock;

	return 0;
		
 shutdown:
	libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
	libssh2_session_free(session);
	
#ifdef WIN32
	closesocket(sock);
#else
	close(sock);
#endif
	libssh2_exit();
#ifdef WIN32
	WSACleanup();
#endif
	return -1;
}

int sbs_ssh_sftp(LIBSSH2_SESSION *session,char* scppath,char* filename,char* destpath)
{
	LIBSSH2_SFTP *sftp_session;
    LIBSSH2_SFTP_HANDLE *sftp_handle;

	FILE *local = NULL;
	char cfilepath[2048];
	char mem[1024*100];
	size_t nread;
	char *ptr;
	int rc = 0;
	char cDestFilePath[2048];
	
	memset(cfilepath,0,sizeof(cfilepath));
	strcpy(cfilepath,scppath);
	strcat(cfilepath,filename);

	local = fopen(cfilepath, "rb");

	if (!local) {
		printf("Can't open file %s\n", cfilepath);
		return -2;
	 }

	//fprintf(stderr, "libssh2_sftp_init()!\n");
	
	sftp_session = libssh2_sftp_init(session);
	
	if (!sftp_session) {
		fprintf(stderr, "Unable to init SFTP session\n");
#ifdef _LOG_SSH_FAIL_		
		g_pLog->LOG1("Unable to init SFTP session\n");
#endif	
		fclose(local);
		return -1;
	}
	 
	//fprintf(stderr, "libssh2_sftp_open()!\n");

	memset(cDestFilePath,0,sizeof(cDestFilePath));
	strcpy(cDestFilePath,destpath);
	strcat(cDestFilePath,filename);
	
	/* Request a file via SFTP */ 
	sftp_handle =
		libssh2_sftp_open(sftp_session, cDestFilePath,
		  LIBSSH2_FXF_WRITE|LIBSSH2_FXF_CREAT|LIBSSH2_FXF_TRUNC,
		  LIBSSH2_SFTP_S_IRUSR|LIBSSH2_SFTP_S_IWUSR|
		  LIBSSH2_SFTP_S_IRGRP|LIBSSH2_SFTP_S_IROTH);
	 
	if (!sftp_handle) 
	{
		fprintf(stderr, "Unable to open file with SFTP\n");
#ifdef _LOG_SSH_FAIL_		
		g_pLog->LOG1("Unable to open file with SFTP\n");
#endif			
		fclose(local);
		libssh2_sftp_shutdown(sftp_session);
		return -1;
	}
	//fprintf(stderr, "libssh2_sftp_open() is done, now send data!\n");
	
	do {
		nread = fread(mem, 1, sizeof(mem), local);
		if (nread <= 0) {
			/* end of file */ 
			break;
		}
		ptr = mem;
	 
		do {
			/* write data in a loop until we block */ 
			rc = libssh2_sftp_write(sftp_handle, ptr, nread);
	
			if(rc < 0)
				break;
			ptr += rc;
			nread -= rc;
		} while (nread);
	} while (rc > 0);
	 
	libssh2_sftp_close(sftp_handle);
	libssh2_sftp_shutdown(sftp_session);
	fclose(local);

	return 0;
}

void sbs_ssh_sftp_close(LIBSSH2_SESSION *session,int sock)
{
	libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
	libssh2_session_free(session);
	
#ifdef WIN32
	closesocket(sock);
#else
	close(sock);
#endif
	//fprintf(stderr, "all done\n");
	libssh2_exit();
#ifdef WIN32
	WSACleanup();
#endif
}

int listRemoteFileName(list<string>& r_fileName, const char* p_ip, const char* p_userName, 
					   const char* p_password,const char* p_dir)
{
	char szCmd[1024];

	sprintf(szCmd,"cd %s;ls --full-time -t|tac|awk '{print $9}'", p_dir);
	string strResult;

	if ( ssh_exec(strResult, p_ip, p_userName, p_password, szCmd) == 0 )
	{
		if ( !strResult.empty() )
		{
			string::size_type lastPos = 0;

			for ( ; ; )
			{
				string::size_type pos = strResult.find('\n', lastPos);

				if ( pos != string::npos )
				{
					string strFileName = strResult.substr(lastPos, pos - lastPos);

					if ( !strFileName.empty() )
					{
						r_fileName.push_back(strFileName);
					}

					lastPos = pos + 1;
				}
				else
				{
					string strFileName = strResult.substr(lastPos);

					if ( !strFileName.empty() )
					{
						r_fileName.push_back(strFileName);
					}

					break;
				}
			}
		}

		return 0;
	}
	else
	{
		return -1;
	}
}


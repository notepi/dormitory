#include <stdio.h>
#include <libgen.h>
#include <poll.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>

#include "IPv4socket.h"

#include <iostream>

using namespace std;

#define BUFFER_SIZE 1024

char read_buf[BUFFER_SIZE];
	
int main(int argc, char *argv[])
{

	if( argc <= 2 ){
		printf( "usage: %s ip_address port_number\n", basename( argv[0] ) );
		return 1;
	}
	const char* ip = argv[1];
	int port = atoi( argv[2] );
	
	int sockfd = CreatIPV4TCPCleintSocket(ip, port);//创建tcp socket
	if(sockfd < 0){
		return 0;
	}
	
	//poll初始化
	pollfd fds[2];
    fds[0].fd = 0;
    fds[0].events = POLLIN ;
    fds[0].revents = 0;
    
    fds[1].fd = sockfd;
    fds[1].events = POLLIN ;
    fds[1].revents = 0;
    
    while (1) {
    	int ret = poll( fds, 2, -1 );
		if( ret < 0 )
		{
			printf( "poll failure\n" );
		}
	    if( fds[0].revents & POLLIN )//客户端有数据输入
	    {
			cout << "ok!" << endl;
			fgets(read_buf, size_t(read_buf), stdin);
			cout << read_buf << endl;
			send( sockfd, read_buf, strlen( read_buf ), 0 );//发送给服务器
	    }
		else if( fds[1].revents & POLLRDHUP )
        {
            printf( "server close the connection\n" );
            break;
        }
	    if( fds[1].revents & POLLIN )//服务器有数据下发
	    {
			cout << "out ok!";
			sleep(1);
			memset( read_buf, '\0', BUFFER_SIZE );
            recv( fds[1].fd, read_buf, BUFFER_SIZE-1, 0 );
            cout << read_buf << endl;
	    }
    }
	close( sockfd );
    return 0;
}
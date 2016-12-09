#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>

#include "IPv4socket.h"

/*	函数功能：创建客户端端IPV4的socket并返回
	参数：1：监听的ip地址，当地址为NULL时，监听所有。
		  2：监听的端口
	返回：socket的fd			
*/
int CreatIPV4TCPCleintSocket(const char* ip, int port)
{
	struct sockaddr_in server_address;
    bzero( &server_address, sizeof( server_address ) );
    server_address.sin_family = AF_INET;
    inet_pton( AF_INET, ip, &server_address.sin_addr );
    server_address.sin_port = htons( port );

    int sockfd = socket( PF_INET, SOCK_STREAM, 0 );
    if(sockfd < 0){
    		printf ("failed to creat socket!\n");
    		return -1;
    }
    if ( connect( sockfd, ( struct sockaddr* )&server_address, sizeof( server_address ) ) < 0 ){
	        printf( "connection failed\n" );
	        close( sockfd );
	        return -1;
	}
	return sockfd;
}


/*	函数功能：创建服务器端IPV4的socket并返回
	参数：1：监听的ip地址，当地址为NULL时，监听所有。
		  2：监听的端口
		  3：监听队列成员数
	返回：listen的fd			
*/

int CreatIPVTCPServerSocket( const char* ip, int port, int backlog )
{
	int ret;
	int sockfd = socket (AF_INET, SOCK_STREAM, 0);		// 创建TCP的socket
	if( sockfd < 0){
		printf("failed to call socket!\n");
		// 如果创建失败，打印错误原因并返回
		printf("errno value: %d, it means: %s",errno, strerror(errno));
		return -1;
	}
	
	// 创建一个IPv4 socket地址
	struct sockaddr_in address;
	bzero( &address, sizeof(address) );					// 结构体清零
	address.sin_family = AF_INET;						// TCP/IPv4协议族
	address.sin_port = htons(port);						// 将端口转成网络字节存入sin_port
	if (NULL == ip){									// 地址为空则监听所有
		address.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else{												// 监听特定IP
		inet_pton(AF_INET, ip, &address.sin_addr);		// 将ip转成网络字节序存入sin_addr
	}
	
	ret = bind (sockfd, (struct sockaddr*)&address, 	// 绑定socket
					sizeof(address));
	if ( ret < 0){
		printf("failed to call bind!\n");
		// 如果绑定失败，打印错误原因并返回
		printf("errno value: %d, it means: %s",errno, strerror(errno));
		return -1;
	}
	
	ret = listen(sockfd, backlog);						// 最多监听backlog个连接
	if( ret < 0){
		printf("failed to call listen!\n");
		// 如果创建失败，打印错误原因并返回
		printf("errno value: %d, it means: %s",errno, strerror(errno));
		return -1;
	}
	
	return sockfd;										// 返回监听
}


/*	函数功能：创建IPV4的socket并绑定返回
	参数：1：监听的ip地址，当地址为NULL时，监听所有。
		  2：监听的端口
		  3：监听队列成员数
	返回：listen的fd			
*/
int Creat_IPV4_UDP_Socket( const char* ip, int port)
{
	
	int sock = socket (AF_INET, SOCK_DGRAM, 0);			// 创建UDP的socket
	if ( sock < 0){
		printf("failed to call socket!\n");
		// 如果创建失败，打印错误原因并返回
		printf("errno value: %d, it means: %s",errno, strerror(errno));
		return -1;
	}
	
	// 创建一个IPv4 socket地址
	struct sockaddr_in address;
	bzero(&address, sizeof(address));					// 结构体清零
	address.sin_family = AF_INET;						// TCP/IPv4协议族
	if (NULL == ip){									// 地址为空则监听所有
		address.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else{												// 监听特定IP
		inet_pton(AF_INET, ip, &address.sin_addr);		// 将ip转成网络字节序存入sin_addr
	}
	address.sin_port = htons(port);						// 将端口转成网络字节存入sin_port
	
	int ret = bind (sock, (struct sockaddr*)&address, 	// 绑定socket
				sizeof(address));
	if ( ret < 0){
		// 绑定失败则退出，打印错误原因并返回
		printf("errno value: %d, it means: %s",errno, strerror(errno));
		return -1;
	}
	
	return sock;
	
}

/*将文件描述符设置成非阻塞的*/
/*	函数功能：将文件描述符设置成非阻塞的，并返回文件描述符原始状态
	参数：1：文件描述符
	返回：原始文件描述符			
*/
int SetNonblocking(int fd)
{
	int ret;
	int old_option = fcntl( fd, F_GETFL);					// 获取旧fd的标志
	int new_option = old_option | O_NONBLOCK;				// fd设置为非阻塞模式
	ret = fcntl( fd, F_SETFL, new_option);					// fd设置为非阻塞模式 
	if ( ret < 0){
		printf("failed to call fcntl!\n");
		// 如果设置失败，打印错误原因并返回
		printf("errno value: %d, it means: %s",errno, strerror(errno));
		return -1;
	}
	return old_option;
}






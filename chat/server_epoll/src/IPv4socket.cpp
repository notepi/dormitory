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
int CreatIpv4TcpCleintSocket(const char* pfcIp, int nPort)
{
	struct sockaddr_in sServerAddress;
    bzero(&sServerAddress, sizeof(sServerAddress));
    sServerAddress.sin_family = AF_INET;
    inet_pton(AF_INET, pfcIp, &sServerAddress.sin_addr);
    sServerAddress.sin_port = htons(nPort);

    int nSockFd = socket(PF_INET, SOCK_STREAM, 0);
    if(nSockFd < 0){
    		printf ("failed to creat socket!\n");
    		return -1;
    }
    if (connect(nSockFd, (struct sockaddr*)&sServerAddress, sizeof(sServerAddress) ) < 0){
	        printf("connection failed\n");
	        close(nSockFd);
	        return -1;
	}
	return nSockFd;
}


/*	函数功能：创建服务器端IPV4的socket并返回
	参数：1：监听的ip地址，当地址为NULL时，监听所有。
		  2：监听的端口
		  3：监听队列成员数
	返回：listen的fd			
*/

int CreatIpv4TcpServerSocket(const char* pfcIp, int nPort, int nBacklog)
{
	int nRet;
	int nSockFd = socket(AF_INET, SOCK_STREAM, 0);		// 创建TCP的socket
	if(nSockFd < 0){
		printf("failed to call socket!\n");
		// 如果创建失败，打印错误原因并返回
		printf("errno value: %d, it means: %s",errno, strerror(errno));
		return -1;
	}
	
	// 创建一个IPv4 socket地址
	struct sockaddr_in sAddress;
	bzero( &sAddress, sizeof(sAddress) );				// 结构体清零
	sAddress.sin_family = AF_INET;						// TCP/IPv4协议族
	sAddress.sin_port = htons(nPort);					// 将端口转成网络字节存入sin_port
	if (NULL == pfcIp){									// 地址为空则监听所有
		sAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else{												// 监听特定IP
		inet_pton(AF_INET, pfcIp, &sAddress.sin_addr);	// 将ip转成网络字节序存入sin_addr
	}
	
	nRet = bind(nSockFd, (struct sockaddr*)&sAddress, 	// 绑定socket
					sizeof(sAddress));
	if(nRet < 0){
		printf("failed to call bind!\n");
		// 如果绑定失败，打印错误原因并返回
		printf("errno value: %d, it means: %s\n",errno, strerror(errno));
		return -1;
	}
	
	nRet = listen(nSockFd, nBacklog);					// 最多监听backlog个连接
	if(nRet < 0){
		printf("failed to call listen!\n");
		// 如果创建失败，打印错误原因并返回
		printf("errno value: %d, it means: %s",errno, strerror(errno));
		return -1;
	}
	
	return nSockFd;										// 返回监听
}


/*	函数功能：创建IPV4的socket并绑定返回
	参数：1：监听的ip地址，当地址为NULL时，监听所有。
		  2：监听的端口
		  3：监听队列成员数
	返回：listen的fd			
*/
int CreatIpv4UdpSocket( const char* pfcIp, int nPort)
{
	
	int nSockFd = socket(AF_INET, SOCK_DGRAM, 0);			// 创建UDP的socket
	if(nSockFd < 0){
		printf("failed to call socket!\n");
		// 如果创建失败，打印错误原因并返回
		printf("errno value: %d, it means: %s",errno, strerror(errno));
		return -1;
	}
	
	// 创建一个IPv4 socket地址
	struct sockaddr_in sAddress;
	bzero(&sAddress, sizeof(sAddress));						// 结构体清零
	sAddress.sin_family = AF_INET;							// TCP/IPv4协议族
	if (NULL == pfcIp){										// 地址为空则监听所有
		sAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else{													// 监听特定IP
		inet_pton(AF_INET, pfcIp, &sAddress.sin_addr);		// 将ip转成网络字节序存入sin_addr
	}
	sAddress.sin_port = htons(nPort);						// 将端口转成网络字节存入sin_port
	
	int nRet = bind (nSockFd, (struct sockaddr*)&sAddress, 	// 绑定socket
				sizeof(sAddress));
	if (nRet < 0){
		// 绑定失败则退出，打印错误原因并返回
		printf("errno value: %d, it means: %s",errno, strerror(errno));
		return -1;
	}
	
	return nSockFd;
	
}

/*将文件描述符设置成非阻塞的*/
/*	函数功能：将文件描述符设置成非阻塞的，并返回文件描述符原始状态
	参数：1：文件描述符
	返回：原始文件描述符			
*/
int SetNonblocking(int nFd)
{
	int nRet;
	int nOldOption = fcntl(nFd, F_GETFL);					// 获取旧fd的标志
	int nNewOption = nOldOption | O_NONBLOCK;				// fd设置为非阻塞模式
	nRet = fcntl(nFd, F_SETFL, nNewOption);					// fd设置为非阻塞模式 
	if (nRet < 0){
		printf("failed to call fcntl!\n");
		// 如果设置失败，打印错误原因并返回
		printf("errno value: %d, it means: %s",errno, strerror(errno));
		return -1;
	}
	return nOldOption;
}






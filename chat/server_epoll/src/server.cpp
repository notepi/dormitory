#include <stdio.h>
#include <libgen.h>
#include <poll.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <signal.h>
#include <assert.h>
#include "epoll.h"
#include "IPv4socket.h"
#include "signal.h"
#include <iostream>

using namespace std;

#define USER_LIMIT  5		//最大监听数
#define MAX_EVENTS 20


struct PROCESS_SIGNAL_DATE g_sMainProcessSigDate;//主进程信号有关变量

int main(int argc, char *argv[])
{
	struct epoll_event gsEvents[MAX_EVENTS];

	if( argc <= 2 ){
		printf( "usage: %s ip_address port_number\n", basename( argv[0] ) );
		return 1;
	}
	const char* pfcIp = argv[1];
	int nPort = atoi( argv[2] );
	int nListenFd = CreatIpv4TcpServerSocket(pfcIp, nPort, 5);//创建tcp socket
	if(nListenFd < 0){
		return 0;
	}
	
	int nEpollFd = epoll_create(5);// 创建epoll，5是常规值
	if(nEpollFd < 0){
		printf("failed to call epoll_create!\n");
		return -1;
	}
	ProcessSigInit(&g_sMainProcessSigDate);//主进程信号处理初始化
	
	AddFdToEpoll(nEpollFd, nListenFd, 1);//将socket的listen添加到事件关注
	AddFdToEpoll(nEpollFd, g_sMainProcessSigDate.m_nRdFd, 1);//添加“信号捕捉”事件关注
	
	int nBreakFlags = 0;
	while( !nBreakFlags){//循环等待
		int nRet = epoll_wait(nEpollFd, gsEvents, MAX_EVENTS, -1);
		if((nRet < 0) && ( errno != EINTR )) {//忽略等待中被信号中断打断的报错
			perror("epoll failure\n");
			break;
		}
		cout << " epoll wait" << endl; 
		lt(gsEvents, nRet, nEpollFd, nListenFd);
	}
	close(nListenFd);
	return 0;
}
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

#define USER_LIMIT  5		//��������
#define MAX_EVENTS 20


struct PROCESS_SIGNAL_DATE g_sMainProcessSigDate;//�������ź��йر���

int main(int argc, char *argv[])
{
	struct epoll_event gsEvents[MAX_EVENTS];

	if( argc <= 2 ){
		printf( "usage: %s ip_address port_number\n", basename( argv[0] ) );
		return 1;
	}
	const char* pfcIp = argv[1];
	int nPort = atoi( argv[2] );
	int nListenFd = CreatIpv4TcpServerSocket(pfcIp, nPort, 5);//����tcp socket
	if(nListenFd < 0){
		return 0;
	}
	
	int nEpollFd = epoll_create(5);// ����epoll��5�ǳ���ֵ
	if(nEpollFd < 0){
		printf("failed to call epoll_create!\n");
		return -1;
	}
	ProcessSigInit(&g_sMainProcessSigDate);//�������źŴ����ʼ��
	
	AddFdToEpoll(nEpollFd, nListenFd, 1);//��socket��listen��ӵ��¼���ע
	AddFdToEpoll(nEpollFd, g_sMainProcessSigDate.m_nRdFd, 1);//��ӡ��źŲ�׽���¼���ע
	
	int nBreakFlags = 0;
	while( !nBreakFlags){//ѭ���ȴ�
		int nRet = epoll_wait(nEpollFd, gsEvents, MAX_EVENTS, -1);
		if((nRet < 0) && ( errno != EINTR )) {//���Եȴ��б��ź��жϴ�ϵı���
			perror("epoll failure\n");
			break;
		}
		cout << " epoll wait" << endl; 
		lt(gsEvents, nRet, nEpollFd, nListenFd);
	}
	close(nListenFd);
	return 0;
}
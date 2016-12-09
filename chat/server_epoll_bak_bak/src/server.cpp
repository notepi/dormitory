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
int pipefd[2];

int main(int argc, char *argv[])
{
	struct epoll_event events[MAX_EVENTS];
	
	if( argc <= 2 ){
		printf( "usage: %s ip_address port_number\n", basename( argv[0] ) );
		return 1;
	}
	const char* ip = argv[1];
	int port = atoi( argv[2] );
	int listenfd = CreatIPVTCPServerSocket(ip, port, 5);//����tcp socket
	if(listenfd < 0){
		return 0;
	}
	
	int ret = socketpair( PF_UNIX, SOCK_STREAM, 0, pipefd );
	assert( ret != -1 );
	SetNonblocking( pipefd[1]);
	
	int epollfd = epoll_create(5);// ����epoll��5�ǳ���ֵ
	if(epollfd < 0){
		printf("failed to call epoll_create!\n");
		return -1;
	}
	
	AddFd2Epoll(epollfd, listenfd, 1);//��socket��listen��ӵ��¼���ע
	AddFd2Epoll(epollfd, pipefd[0], 1);//��ӡ��źŲ�׽���¼���ע
	
	SigSet(SIGUSR1);//��׽SIGUSR1�ź�
	
	int breakflags = 0;
	while( !breakflags){//ѭ���ȴ�
		int ret = epoll_wait(epollfd, events, MAX_EVENTS, -1);
		if (ret < 0&& ( errno != EINTR )) {//���Եȴ��б��ź��жϴ�ϵı���
			perror("epoll failure\n");
			break;
		}
		cout << " epoll wait" << endl; 
		lt( events, ret, epollfd, listenfd);
	}
	close( listenfd );
	return 0;
}
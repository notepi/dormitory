#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <pthread.h>

#include <iostream>

using namespace std;
static int pipefd[2];
#define MAX_EVENTS 20
int SignalProcessing(int sockfd);
/*���ļ����������óɷ�������*/
/*	�������ܣ����ļ����������óɷ������ģ��������ļ�������ԭʼ״̬
	������1���ļ�������
	���أ�ԭʼ�ļ�������			
*/
int SetNonblocking(int fd)
{
	int ret;
	int old_option = fcntl( fd, F_GETFL);					// ��ȡ��fd�ı�־
	int new_option = old_option | O_NONBLOCK;				// fd����Ϊ������ģʽ
	ret = fcntl( fd, F_SETFL, new_option);					// fd����Ϊ������ģʽ 
	if ( ret < 0){
		printf("failed to call fcntl!\n");
		// �������ʧ�ܣ���ӡ����ԭ�򲢷���
		printf("errno value: %d, it means: %s",errno, strerror(errno));
		return -1;
	}
	return old_option;
}

void addfd(int epollfd, int listenfd, int enable)
{
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd= listenfd;
	if(enable){
		ev.events |= EPOLLET;
	}
	epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev);
	SetNonblocking(listenfd);
}
// ���÷�����ģʽ
int setnonblocking( int fd )
{
    int old_option = fcntl( fd, F_GETFL );
    int new_option = old_option | O_NONBLOCK;
    fcntl( fd, F_SETFL, new_option );
    return old_option;
}


// �źŴ���
void sig_handler (int signum)
{
	// ����ԭ����errno����֤����������
	int save_errno = errno;
	int sig = signum;
	send(pipefd[1], (char *)&sig, sizeof(char), 0);
	cout << "receive signas" << endl;
	errno = save_errno;
	
}

// �ź�ע��
void addsig(int signum)
{
	struct sigaction act;
	memset( &act, '\0', sizeof( act ) );
	act.sa_handler = sig_handler;
	act.sa_flags |= SA_RESTART;
	sigfillset( &act.sa_mask );
	assert(sigaction(signum, &act, NULL) != -1 );;
}

// ��fd��ӵ�EPOLL��ע
void AddFd2Epoll(int epollfd, int listenfd, int enable)
{
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd= listenfd;
	if(enable){
		ev.events |= EPOLLET;
	}
	epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev);
	SetNonblocking(listenfd);
}

int main()
{
	struct epoll_event events[MAX_EVENTS];
	int ret = socketpair( PF_UNIX, SOCK_STREAM, 0, pipefd );
	assert( ret != -1 );
	setnonblocking( pipefd[1] );
	
	int epollfd = epoll_create(5);// ����epoll
	if(epollfd < 0){
		printf("failed to call epoll_create!\n");
		return -1;
	}
	AddFd2Epoll(epollfd, pipefd[0], 1);//����¼���ע
    addfd( epollfd, pipefd[0], 1);
	int breakflags = 0;
	
	addsig(SIGUSR1);
	
	while (!breakflags) {
		cout << "while" << endl;
		int ret = epoll_wait(epollfd, events, MAX_EVENTS, -1);
		if (ret < 0 && ( errno != EINTR )) {//���Եȴ��б��ź��жϴ�ϵı���
			perror("epoll failure\n");
			break;
		}
		for (int i=0; i<ret; i++){//ѭ����ȡ�¼�
			int sockfd = events[i].data.fd;
			if(sockfd == pipefd[0]  && ( events[i].events & EPOLLIN )){//�źŴ���
				if(!SignalProcessing(sockfd)){
					continue;
				};    
			}
		}
		 
	}	
}


int SignalProcessing(int sockfd)
{
	char signals[1024];
	memset(signals, 0x00, sizeof(signals));

	int ret = recv(sockfd, signals, sizeof(signals), 0);
	if( ret == -1 ){
		return 0;
	}
	else if( ret == 0 ){
		return 0;
	}
	else {
		for (int j=0; j < ret; j++){
			switch (signals[j]){
				case SIGUSR1:
					cout << "get signal SIGUSRl" << endl;
				break;
				default:

				break;
			}
		}

	}
	return 1;
}
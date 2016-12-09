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

#include "epoll.h"
#include "IPv4socket.h"
#include "signal.h"

#include <iostream>

using namespace std;

extern int pipefd[2];
#define BUFFER_SIZE 1024

/*	�������ܣ���fd��ӵ�epoll����
	������1��������epoll  2����������fd
		  3���Ƿ����ó�etģʽ��1-�� 0-��
	���أ���			
*/
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

/*	�������ܣ���fd��epoll������ɾ��
	������1��������epoll  2����������fd
	���أ�socket��fd			
*/
void DeletFdFromEpoll (int epollfd, int listenfd)
{
	struct epoll_event ev;// ���ã�ֻ����������
	epoll_ctl(epollfd, EPOLL_CTL_DEL, listenfd, &ev);
}



void lt( epoll_event* events, int number, int epollfd, int listenfd )
{
	char buf[ BUFFER_SIZE ];
	struct epoll_event ev;
	int ret;
	for(int i=0; i < number ; i++){//��ѯ�������¼�
		int sockfd = events[i].data.fd;
		if(sockfd == pipefd[0]  && ( events[i].events & EPOLLIN )){//�źŴ���
			if(!SignalProcessing(sockfd)){
				continue;
			}  
		}
		else if(listenfd == sockfd ){//���µ�����
			struct sockaddr_in client_address;
		    socklen_t cleintaddrlen = sizeof(struct sockaddr);
    		int connfd = accept(listenfd, ( struct sockaddr* )&client_address, &cleintaddrlen);
    		if(connfd < 0){//��ȡʧ��
    			printf("failed to accept!\n");
    			continue;
    		}
    		
			AddFd2Epoll(epollfd, connfd, 0);//����¼���ע
			cout << "comes a new user" << endl;
		}
		else if (events[i].events & POLLRDHUP)//�����ӶϿ�
		{
			cout << "a client left\n" << endl;
			DeletFdFromEpoll(epollfd, sockfd);
			close(sockfd);
		}
		else if(events[i].events & EPOLLIN){//������
            memset( buf, '\0', sizeof(buf));
            ret = recv( sockfd, buf, BUFFER_SIZE-1, 0 );            
            if( ret < 0 ){//����
                if( (errno != EAGAIN) && (errno != EWOULDBLOCK) ){//����������Ϊ����δ����
                	DeletFdFromEpoll (epollfd, sockfd);
                    close( sockfd );
                    continue;
                }
            }
			printf( "get %d bytes of client data :%s from: %d\n", ret, buf, sockfd );
        }
	}
}


void et( epoll_event* events, int number, int epollfd, int listenfd)
{
	char buf[ BUFFER_SIZE ];
	struct epoll_event ev;
	int ret;
	for(int i=0; i < number ; i++){//��ѯ�������¼�
		int sockfd = events[i].data.fd;
		if(listenfd == events[i].data.fd ){//���µ�����
			struct sockaddr_in client_address;
		    socklen_t cleintaddrlen = sizeof(struct sockaddr);
    		int connfd = accept(listenfd, ( struct sockaddr* )&client_address, &cleintaddrlen);
    		if(connfd < 0){//��ȡʧ��
    			printf("failed to accept!\n");
    			continue;
    		}
    		
			AddFd2Epoll(epollfd, connfd, 1);//����¼���ע
			cout << "comes a new user\n" << endl;
		}
		else if (events[i].events & POLLRDHUP)//�����ӶϿ�
		{
			cout << "a client left\n" << endl;
			DeletFdFromEpoll(epollfd, sockfd);
			close(sockfd);
		}
		else if(events[i].events & EPOLLIN){//������
    		while(1)
    		{
	            memset( buf, '\0', sizeof(buf));
	            ret = recv( sockfd, buf, BUFFER_SIZE-1, 0 );            
	            if( ret < 0 ){//����
	                if( (errno == EAGAIN) || (errno == EWOULDBLOCK) ){//����δ����
	                    continue;
	                }
	                //���ӳ�������
					DeletFdFromEpoll (epollfd, sockfd);//ɾ����ע
                    close( sockfd );
                    break;	                
	            }
				printf( "get %d bytes of client data :%s! from: %d\n", ret, buf, sockfd );
			}
        }
	}
}
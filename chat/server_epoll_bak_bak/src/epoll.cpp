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

/*	函数功能：将fd添加到epoll监听
	参数：1：监听的epoll  2：被监听的fd
		  3：是否配置成et模式，1-是 0-否
	返回：无			
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

/*	函数功能：将fd从epoll监听中删除
	参数：1：监听的epoll  2：被监听的fd
	返回：socket的fd			
*/
void DeletFdFromEpoll (int epollfd, int listenfd)
{
	struct epoll_event ev;// 无用，只做参数满足
	epoll_ctl(epollfd, EPOLL_CTL_DEL, listenfd, &ev);
}



void lt( epoll_event* events, int number, int epollfd, int listenfd )
{
	char buf[ BUFFER_SIZE ];
	struct epoll_event ev;
	int ret;
	for(int i=0; i < number ; i++){//轮询发生的事件
		int sockfd = events[i].data.fd;
		if(sockfd == pipefd[0]  && ( events[i].events & EPOLLIN )){//信号处理
			if(!SignalProcessing(sockfd)){
				continue;
			}  
		}
		else if(listenfd == sockfd ){//来新的链接
			struct sockaddr_in client_address;
		    socklen_t cleintaddrlen = sizeof(struct sockaddr);
    		int connfd = accept(listenfd, ( struct sockaddr* )&client_address, &cleintaddrlen);
    		if(connfd < 0){//获取失败
    			printf("failed to accept!\n");
    			continue;
    		}
    		
			AddFd2Epoll(epollfd, connfd, 0);//添加事件关注
			cout << "comes a new user" << endl;
		}
		else if (events[i].events & POLLRDHUP)//有连接断开
		{
			cout << "a client left\n" << endl;
			DeletFdFromEpoll(epollfd, sockfd);
			close(sockfd);
		}
		else if(events[i].events & EPOLLIN){//来数据
            memset( buf, '\0', sizeof(buf));
            ret = recv( sockfd, buf, BUFFER_SIZE-1, 0 );            
            if( ret < 0 ){//出错
                if( (errno != EAGAIN) && (errno != EWOULDBLOCK) ){//报错并不是因为数据未读完
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
	for(int i=0; i < number ; i++){//轮询发生的事件
		int sockfd = events[i].data.fd;
		if(listenfd == events[i].data.fd ){//来新的链接
			struct sockaddr_in client_address;
		    socklen_t cleintaddrlen = sizeof(struct sockaddr);
    		int connfd = accept(listenfd, ( struct sockaddr* )&client_address, &cleintaddrlen);
    		if(connfd < 0){//获取失败
    			printf("failed to accept!\n");
    			continue;
    		}
    		
			AddFd2Epoll(epollfd, connfd, 1);//添加事件关注
			cout << "comes a new user\n" << endl;
		}
		else if (events[i].events & POLLRDHUP)//有连接断开
		{
			cout << "a client left\n" << endl;
			DeletFdFromEpoll(epollfd, sockfd);
			close(sockfd);
		}
		else if(events[i].events & EPOLLIN){//来数据
    		while(1)
    		{
	            memset( buf, '\0', sizeof(buf));
	            ret = recv( sockfd, buf, BUFFER_SIZE-1, 0 );            
	            if( ret < 0 ){//出错
	                if( (errno == EAGAIN) || (errno == EWOULDBLOCK) ){//数据未读完
	                    continue;
	                }
	                //链接出现问题
					DeletFdFromEpoll (epollfd, sockfd);//删除关注
                    close( sockfd );
                    break;	                
	            }
				printf( "get %d bytes of client data :%s! from: %d\n", ret, buf, sockfd );
			}
        }
	}
}
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

extern struct PROCESS_SIGNAL_DATE g_sMainProcessSigDate;
#define BUFFER_SIZE 1024

/*	函数功能：将fd添加到epoll监听
	参数：1：监听的epoll  2：被监听的fd
		  3：是否配置成et模式，1-是 0-否
	返回：无			
*/
void AddFdToEpoll(int nEpollFd, int nListenFd, int nEnable)
{
	struct epoll_event sEv;
	sEv.events = EPOLLIN;
	sEv.data.fd= nListenFd;
	if(nEnable){
		sEv.events |= EPOLLET;
	}
	epoll_ctl(nEpollFd, EPOLL_CTL_ADD, nListenFd, &sEv);
	SetNonblocking(nListenFd);
}

/*	函数功能：将fd从epoll监听中删除
	参数：1：监听的epoll  2：被监听的fd
	返回：socket的fd			
*/
void DeleteFdFromEpoll (int nEpollFd, int nListenFd)
{
	struct epoll_event sEv;// 无用，只做参数满足
	epoll_ctl(nEpollFd, EPOLL_CTL_DEL, nListenFd, &sEv);
}



void lt( epoll_event* pfsEvents, int nNumber, int nEpollFd, int nListenFd )
{
	char gcBuff[ BUFFER_SIZE ];
	struct epoll_event sEv;
	int nRet;
	for(int i=0; i < nNumber ; i++){//轮询发生的事件
		int nSockFd = pfsEvents[i].data.fd;
		if(nSockFd == g_sMainProcessSigDate.m_nRdFd && (pfsEvents[i].events & EPOLLIN )){//信号处理
			if(!MainProcessSignalProcessing(nSockFd)){
				continue;
			}  
		}
		else if(nListenFd == nSockFd ){//来新的链接
			struct sockaddr_in sClientAddress;
		    socklen_t sklCleintAddrLen = sizeof(struct sockaddr);
    		int nConnFd = accept(nListenFd, (struct sockaddr*)&sClientAddress, &sklCleintAddrLen);
    		if(nConnFd < 0){//获取失败
    			printf("failed to accept!\n");
    			continue;
    		}
    		
			AddFdToEpoll(nEpollFd, nConnFd, 0);//添加事件关注
			cout << "comes a new user" << endl;
		}
		else if (pfsEvents[i].events & EPOLLRDHUP)//有连接断开
		{
			cout << "a client left\n" << endl;
			DeleteFdFromEpoll(nEpollFd, nSockFd);
			close(nSockFd);
		}
		else if(pfsEvents[i].events & EPOLLIN){//来数据
            memset(gcBuff, '\0', sizeof(gcBuff));
            nRet = recv(nSockFd, gcBuff, BUFFER_SIZE-1, 0 );            
            if(nRet <= 0 ){//出错
                if((errno != EAGAIN) && (errno != EWOULDBLOCK) ){//报错并不是因为数据未读完
                	DeleteFdFromEpoll(nEpollFd, nSockFd);
                	printf("a client lesft!\n");
                    close(nSockFd);
                    continue;
                }
            }
			printf( "get %d bytes of client data :%s from: %d\n", nRet, gcBuff, nSockFd );
        }
	}
}


void et(epoll_event* pfsEvents, int nNumber, int nEpollFd, int nListenFd)
{
	char gcBuff[BUFFER_SIZE];
	struct epoll_event sEv;
	int nRet;
	for(int i=0; i < nNumber ; i++){//轮询发生的事件
		int nSockFd = pfsEvents[i].data.fd;
		if(nListenFd == pfsEvents[i].data.fd ){//来新的链接
			struct sockaddr_in sClientAddress;
		    socklen_t sklCleintAddrLen = sizeof(struct sockaddr);
    		int nConnFd = accept(nListenFd, ( struct sockaddr* )&sClientAddress, &sklCleintAddrLen);
    		if(nConnFd < 0){//获取失败
    			printf("failed to accept!\n");
    			continue;
    		}
    		
			AddFdToEpoll(nEpollFd, nConnFd, 1);//添加事件关注
			cout << "comes a new user\n" << endl;
		}
		else if(pfsEvents[i].events & POLLRDHUP)//有连接断开
		{
			cout << "a client left\n" << endl;
			DeleteFdFromEpoll(nEpollFd, nSockFd);
			close(nSockFd);
		}
		else if(pfsEvents[i].events & EPOLLIN){//来数据
    		while(1)
    		{
	            memset(gcBuff, '\0', sizeof(gcBuff));
	            nRet = recv(nSockFd, gcBuff, BUFFER_SIZE-1, 0 );            
	            if(nRet < 0 ){//出错
	                if( (errno == EAGAIN) || (errno == EWOULDBLOCK) ){//数据未读完
	                    continue;
	                }
	                //链接出现问题
					DeleteFdFromEpoll(nEpollFd, nSockFd);//删除关注
                    close(nSockFd );
                    break;	                
	            }
				printf("get %d bytes of client data :%s! from: %d\n", nRet, gcBuff, nSockFd);
			}
        }
	}
}
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
int epoll_create ( int size )

#include "IPv4socket.h"

#include <iostream>

using namespace std;
#define FD_LIMIT 65535
#define USER_LIMIT  5		//��������
#define BUFFER_SIZE 1024

#define MAX_EVENTS 20
char read_buf[BUFFER_SIZE];
struct client_data
{
    struct sockaddr_in address;
    char* write_buf;
    char buf[ BUFFER_SIZE ];
};

void lt( epoll_event* events, int number, int epollfd, int listenfd );

int main(int argc, char *argv[])
{

	if( argc <= 2 ){
		printf( "usage: %s ip_address port_number\n", basename( argv[0] ) );
		return 1;
	}
//	client_data* users = new client_data[FD_LIMIT];//Ϊÿ���û���������
	
	const char* ip = argv[1];
	int port = atoi( argv[2] );
	int listenfd = CreatIPVTCPServerSocket(ip, port, 5);//����tcp socket
	if(listenfd < 0){
		return 0;
	}
}
//	int epollfd = epoll_create(5);
//	struct epoll_event ev, events[MAX_EVENTS];
//	ev.events = EPOLLIN;
//	ev.data.fd= listenfd;
//	epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev);
//	int user_counter = 0;
//	// ����epoll
//	while( 1 )
//	{
//		int nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
//	    if (nfds == -1) {
//	       perror("epoll_pwait");
//	       break;
//		}
//		lt( events, FD_LIMIT, epollfd, listenfd );
//	}
//	close( listenfd );
//	delete [] users;
//    return 0;
//}
//
//void lt( epoll_event* events, int number, int epollfd, int listenfd, client_data* users )
//{
//	struct epoll_event ev;
//	int ret;
//	for(int i=0; i < number ; i++){
//		if(listenfd == events[i].data.fd ){//���µ�����
//			struct sockaddr_in client_address;
//		    socklen_t cleintaddrlen = sizeof(struct sockaddr);
//    		int connfd = accept(listenfd, ( struct sockaddr* )&client_address, &cleintaddrlen);
//    		if(connfd < 0){//��ȡʧ��
//    			printf("failed to accept!\n");
//    			continue;
//    		}
////		    if( user_counter >= USER_LIMIT ){//�û�����
////                const char* info = "too many users\n";
////                printf( "%s", info );
////                send( connfd, info, strlen( info ), 0 );
////                close( connfd );
////                continue;
////            }
//
//			ev.events = EPOLLIN;
//			ev.data.fd= listenfd;
//			epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &ev);
//		}
//	    else if(events[i].events & EPOLLERR){//����
//    		printf( "get an error from %d\n", events[i].data.fd );
//            char errors[ 100 ];
//            memset( errors, '\0', 100 );
//            socklen_t length = sizeof( errors );
//            if( getsockopt( events[i].data.fd, SOL_SOCKET, SO_ERROR, &errors, &length ) < 0 )//���socket�Ĵ���
//            {
//                printf( "get socket option failed\n" );
//            }
//            continue;
//    	}
//		else if( events[i].events & EPOLLRDHUP ){// �ͻ����뿪
////			users[fds[i].fd] = users[fds[user_counter].fd];//ɾ�������ַ�������鱣������һ����ַ��ֵ��ǰ��
//			close( events[i].data.fd );
////			fds[i] = fds[user_counter];//������bug
////	        i--;
////	        user_counter--;
//			epoll_ctl(epollfd, EPOLL_CTL_DEL, listenfd, &ev);//�ӹ�ע����ɾ��
//			printf( "a client left\n" );
//		}
//		else if(events[i].events & EPOLLIN){//������
//    		int connfd = events[i].data.fd;
//            memset( users[connfd].buf, '\0', BUFFER_SIZE );
//            ret = recv( connfd, users[connfd].buf, BUFFER_SIZE-1, 0 );
//            
//            if( ret < 0 ){//����
//                if( errno != EAGAIN ){//����������Ϊ����δ����
//                    close( connfd );
////                    users[fds[i].fd] = users[fds[user_counter].fd];
////                    fds[i] = fds[user_counter];
////                    i--;
////                    user_counter--;
//                }
//            }
//			else if( ret == 0 ){
//                printf( "code should not come to here\n" );
//            }
//            else {//�ɹ�����
////				for( int j = 1; j <= user_counter; ++j ){//ѭ��
////                    if( fds[j].fd == connfd ){
////                        continue;
////                    }
////                    
////                    fds[j].events |= ~POLLIN;
////                    fds[j].events |= POLLOUT;
////                    users[fds[j].fd].write_buf = users[connfd].buf;
////                }
//				printf( "get %d bytes of client data %s from %d\n", ret, users[connfd].buf, connfd );
//            }
//        }
//	}
//}
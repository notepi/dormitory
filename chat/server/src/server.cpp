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
#include "IPv4socket.h"

#include <iostream>

using namespace std;
#define FD_LIMIT 65535
#define USER_LIMIT  5		//最大监听数
#define BUFFER_SIZE 1024
char read_buf[BUFFER_SIZE];
struct client_data
{
    struct sockaddr_in address;
    char* write_buf;
    char buf[ BUFFER_SIZE ];
};

int main(int argc, char *argv[])
{

	if( argc <= 2 )
	{
		printf( "usage: %s ip_address port_number\n", basename( argv[0] ) );
		return 1;
	}
	const char* ip = argv[1];
	int port = atoi( argv[2] );
	int sockfd = CreatIPVTCPServerSocket(ip, port, 5);//创建tcp socket
	if(sockfd < 0){
		return 0;
	}
	int user_counter = 0;
	pollfd fds[USER_LIMIT+1];
	client_data* users = new client_data[FD_LIMIT];
	fds[0].fd = sockfd; 
	fds[0].events = POLLIN | POLLRDHUP;//关注来数据，链接关闭
	while( 1 )
	{
        int ret = poll( fds, user_counter+1, -1 );//循环监听
        if ( ret < 0 ){
            printf( "poll failure\n" );
            break;
        }
        
        for(int i=0; i < user_counter+1; i++){
        	if((fds[i].fd == sockfd) && (fds[i].revents & POLLIN)){//来新的链接
        		struct sockaddr_in client_address;
        		socklen_t cleintaddrlen = sizeof(struct sockaddr);
        		int connfd = accept(sockfd, ( struct sockaddr* )&client_address, &cleintaddrlen);
        		if(connfd < 0){//获取失败
        			printf("failed to accept!\n");
        		}
				if( user_counter >= USER_LIMIT ){//客户过多
                    const char* info = "too many users\n";
                    printf( "%s", info );
                    send( connfd, info, strlen( info ), 0 );
                    close( connfd );
                    continue;
                }
                user_counter++;//记录客户端数
                users[connfd].address = client_address;//记录客户端地址
                SetNonblocking( connfd );
				fds[user_counter].fd = connfd;//加入关注
                fds[user_counter].events = POLLIN | POLLRDHUP | POLLERR;
                fds[user_counter].revents = 0;
				printf( "comes a new user, now have %d users\n", user_counter );
        	}
        	else if(fds[i].revents & POLLERR){//出错
        		printf( "get an error from %d\n", fds[i].fd );
                char errors[ 100 ];
                memset( errors, '\0', 100 );
                socklen_t length = sizeof( errors );
                if( getsockopt( fds[i].fd, SOL_SOCKET, SO_ERROR, &errors, &length ) < 0 )//清除socket的错误
                {
                    printf( "get socket option failed\n" );
                }
                continue;
        	}
        	else if( fds[i].revents & POLLRDHUP ){// 客户端离开
        		users[fds[i].fd] = users[fds[user_counter].fd];//删除保存地址，将数组保存的最后一个地址赋值给前面
        		close( fds[i].fd );
				fds[i] = fds[user_counter];//这里有bug
                i--;
                user_counter--;
                printf( "a client left\n" );
        	}
        	else if(fds[i].revents & POLLIN){//来数据
        		int connfd = fds[i].fd;
                memset( users[connfd].buf, '\0', BUFFER_SIZE );
                ret = recv( connfd, users[connfd].buf, BUFFER_SIZE-1, 0 );
                printf( "get %d bytes of client data %s from %d\n", ret, users[connfd].buf, connfd );
                if( ret < 0 ){//出差
                    if( errno != EAGAIN ){//报错并不是因为数据未读完
                        close( connfd );
                        users[fds[i].fd] = users[fds[user_counter].fd];
                        fds[i] = fds[user_counter];
                        i--;
                        user_counter--;
                    }
                }
				else if( ret == 0 ){
                    printf( "code should not come to here\n" );
                }
                else {
					for( int j = 1; j <= user_counter; ++j ){//循环
                        if( fds[j].fd == connfd ){
                            continue;
                        }
                        
                        fds[j].events |= ~POLLIN;
                        fds[j].events |= POLLOUT;
                        users[fds[j].fd].write_buf = users[connfd].buf;
                    }
                	
                }

        	}
			else if( fds[i].revents & POLLOUT ){//检测是否可写
                int connfd = fds[i].fd;
                if( ! users[connfd].write_buf )
                {
                    continue;
                }
                ret = send( connfd, users[connfd].write_buf, strlen( users[connfd].write_buf ), 0 );
                users[connfd].write_buf = NULL;
                fds[i].events |= ~POLLOUT;
                fds[i].events |= POLLIN;
            }
        }
    }
     
	delete [] users;
    close( sockfd );
    return 0;
	
}
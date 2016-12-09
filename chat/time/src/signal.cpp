#include <signal.h>
#include <assert.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include "signal.h"

#include <iostream>
using namespace std;

extern int pipefd[2];
/*	函数功能：SIGUSR1信号的处理
	参数：无
	返回：无		
*/
void SIGUSR1Handler()
{
	cout << "get signals SIGUSRl" << endl;
}

/*	函数功能：创建处理进程接受到的信号
	参数：1：接受信号的套接字
	返回：处理结果			
*/
int SignalProcessing(int sockfd)
{
	char signals[1024];
	memset(signals, 0x00, sizeof(signals));

	int ret = recv(sockfd, signals, sizeof(signals), 0);
	if( ret == -1 ){//读取失败
		return 0;
	}
	else if( ret == 0 ){//未读到数据
		return 0;
	}
	else {
		for (int j=0; j < ret; j++){
			switch (signals[j]){
				case SIGUSR1:
					SIGUSR1Handler();
					break;
				default:

				break;
			}
		}
	}
}


/*	函数功能：接受信号后发送给套接字
	参数：1：接受的信号  1：发送给的套接字
	返回：处理结果			
*/
void SigHandler (int signum)
{
	// 保存原来的errno，保证函数可重用
	int save_errno = errno;
	int sig = signum;
	send(pipefd[1], (char *)&sig, sizeof(char), 0);
	cout << "receive signals" << endl;
	errno = save_errno;
}

/*	函数功能：接受信号后发送给套接字
	参数：1：接受的信号  1：发送给的套接字
	返回：无		
*/
void SigSet(int signum)
{
	struct sigaction act;
	memset( &act, '\0', sizeof( act));
	act.sa_handler = SigHandler;
	act.sa_flags |= SA_RESTART;
	sigfillset( &act.sa_mask );
	assert(sigaction(signum, &act, NULL) != -1 );
}
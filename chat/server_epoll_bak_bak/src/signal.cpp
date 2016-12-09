#include <signal.h>
#include <assert.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include "signal.h"

#include <iostream>
using namespace std;

extern int pipefd[2];
/*	�������ܣ�SIGUSR1�źŵĴ���
	��������
	���أ���		
*/
void SIGUSR1Handler()
{
	cout << "get signals SIGUSRl" << endl;
}

/*	�������ܣ�����������̽��ܵ����ź�
	������1�������źŵ��׽���
	���أ�������			
*/
int SignalProcessing(int sockfd)
{
	char signals[1024];
	memset(signals, 0x00, sizeof(signals));

	int ret = recv(sockfd, signals, sizeof(signals), 0);
	if( ret == -1 ){//��ȡʧ��
		return 0;
	}
	else if( ret == 0 ){//δ��������
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


/*	�������ܣ������źź��͸��׽���
	������1�����ܵ��ź�  1�����͸����׽���
	���أ�������			
*/
void SigHandler (int signum)
{
	// ����ԭ����errno����֤����������
	int save_errno = errno;
	int sig = signum;
	send(pipefd[1], (char *)&sig, sizeof(char), 0);
	cout << "receive signals" << endl;
	errno = save_errno;
}

/*	�������ܣ������źź��͸��׽���
	������1�����ܵ��ź�  1�����͸����׽���
	���أ���		
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
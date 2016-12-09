
extern struct PROCESS_SIGNAL_DATE g_sMainProcessSigDate;//主进程信号有关变量
#include "signal.h"
/*****************************************************
 * 函数	 : SigSet
 *
 * 功能  : 设置接受信号后的动作
 *
 * 参数  : 1：接受的信号  1：发送给的套接字
 *			 
 *
 * 返回值: 无
 *****************************************************/
void SigSet(int nSigNum, void (*OnSigHandler)(int sig))
{
	struct sigaction sAct;
	memset( &sAct, '\0', sizeof(sAct));
	sAct.sa_handler = OnSigHandler;
	sAct.sa_flags |= SA_RESTART;
	sigfillset( &sAct.sa_mask );
	assert(sigaction(nSigNum, &sAct, NULL) != -1 );
}

/*****************************************************
 * 函数	 : SIGUSR1Handler
 *
 * 功能  : 处理SIGUSR1信号
 *
 * 参数  : 无
 *			 
 *
 * 返回值: 无
 *****************************************************/
void SIGUSR1Handler()
{
	cout << "get signals SIGUSRl" << endl;
}

/*****************************************************
 * 函数	 : SIGPIPEHandler
 *
 * 功能  : 处理SIGHUP信号
 *
 * 参数  : 无
 *			 
 *
 * 返回值: 无
 *****************************************************/
void SIGPIPEHandler()
{
	cout << "get signals SIGPIPE" << endl;
}

/*****************************************************
 * 函数	 : SIGHUPHandler
 *
 * 功能  : 处理SIGHUP信号
 *
 * 参数  : 无
 *			 
 *
 * 返回值: 无
 *****************************************************/
void SIGHUPHandler()
{
	cout << "get signals SIGHUP" << endl;
}

/*****************************************************
 * 函数	 : 建立进程统一处理信号流程
 *
 * 功能  : 处理SIGUSR1信号
 *
 * 参数  : 无
 *			 
 *
 * 返回值: 是否成功处理  0-否 1-是
 *****************************************************/
int MainProcessSignalProcessing(int nSockFd)
{
	char gcSignals[1024];
	memset(gcSignals, 0x00, sizeof(gcSignals));

	int nRet = recv(nSockFd, gcSignals, sizeof(gcSignals), 0);
	if(nRet == -1 ){//读取失败
		return 0;
	}
	else if( nRet == 0 ){//未读到数据
		return 0;
	}
	else{
		for (int j=0; j < nRet; j++){
			switch (gcSignals[j]){
				case SIGUSR1:
					SIGUSR1Handler();
					break;
				case SIGHUP:
					SIGHUPHandler();
					break;
				case SIGPIPE:
					SIGPIPEHandler();
					break;
				default:

				break;
			}
		}
	}
}

/*****************************************************
 * 函数	 : SigHandler
 *
 * 功能  : 接受信号后发送给套接字
 *
 * 参数  : 1：接受的信号  1：发送给的套接字
 *			 
 *
 * 返回值: 无
 *****************************************************/
void MainProcessSigHandler (int nSigNum)
{
	// 保存原来的errno，保证函数可重用
	int nSaveErrno = errno;
	int nSig = nSigNum;
	send(g_sMainProcessSigDate.m_nWrFd, (char *)&nSig, sizeof(char), 0);
	cout << "receive signals" << endl;
	errno = nSaveErrno;
}
/*****************************************************
 * 函数	 : MainProcessSigInit
 *
 * 功能  : 进程信号处理初始化
 *
 * 参数  : 1.接发套接字
 *			 
 *
 * 返回值: 无
 *****************************************************/
void ProcessSigInit(struct PROCESS_SIGNAL_DATE *pfsSigData)
{
	int nRet = socketpair(PF_UNIX, SOCK_STREAM, 0, pfsSigData->m_gnPipeFd);
	assert(nRet != -1 );
	
	SetNonblocking(pfsSigData->m_gnPipeFd[1]);
	SetNonblocking(pfsSigData->m_gnPipeFd[0]);
	pfsSigData->m_nRdFd = pfsSigData->m_gnPipeFd[0];
	pfsSigData->m_nWrFd = pfsSigData->m_gnPipeFd[1];
	
	SigSet(SIGUSR1, MainProcessSigHandler);	//捕捉SIGUSR1信号
	SigSet(SIGHUP, MainProcessSigHandler);	//捕捉SIGHUP信号，终端退出
	SigSet(SIGPIPE, MainProcessSigHandler);	//捕捉SIGPIPE信号，错误的管道读写
	
}
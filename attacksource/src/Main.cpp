#include<stdio.h>
#include<signal.h>
#include<string>
#include<string.h>
#include<pthread.h>
//#include "util/Log4cplusUtil.h"
#include "Init.h"
#include "AdsReport.h"
#include "Tools.h"
#include "Global.h"
using namespace ffcsns;
using namespace log4cplus;
CDataBase *CDataBase::database = NULL;

log4cplus::Logger* g_p_debugLog;
log4cplus::Logger* ffcsns::g_p_defaultLog;
void cleanExit(int signal);
extern void* MqThread_(void *);
int main()
{
	if (signal(SIGTERM, cleanExit) == SIG_ERR)
    {
        fprintf(stderr, "Can not catch SIGTERM.\n");
    }

    if (signal(SIGINT, cleanExit) == SIG_ERR)
    {
        fprintf(stderr, "Can not catch SIGINT.\n");
    }

    if (signal(SIGQUIT, cleanExit) == SIG_ERR)
    {
        fprintf(stderr, "Can not catch SIGQUIT.\n");
    }
	ffcsns::log4cplusStart("../config/flow.properties");
	Logger defaultLog = Logger::getInstance(LOG4CPLUS_TEXT("FLOW_INFO_LOGGER"));
	g_p_defaultLog = &defaultLog;
	Logger debugLog = Logger::getInstance(LOG4CPLUS_TEXT("DEBUG_LOGGER"));
	g_p_debugLog = &debugLog;
	pthread_t vMqCollect;	
	CInit init;
	int iRet = 0;
	if( (iRet = pthread_create(&vMqCollect, NULL, MqThread_, NULL)) )
	{
		exit(1);
	}
	if(vMqCollect != 0)
	{
        	pthread_join(vMqCollect, NULL);        
    }
	return 0;
}

void cleanExit(int signal)
{
		CDataBase::DatabaseDestroy();
		exit(0);	
}

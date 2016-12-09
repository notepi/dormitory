#ifndef _DPCLEAN_H_
#define _DPCLEAN_H_
#include<string>
#include "cJSON.h"
using namespace std;
class DpClean
{
public:
	DpClean(std::string ip,std::string port);
	~DpClean();
	
	int doClean(std::string strStartIp,std::string strEndIp);
	int stopClean(std::string strStartIp,std::string strEndIp);
	void UpdateStatus(std::string strIp,int iStatus);

private:	
	std::string strDeviceIp;
	std::string strPort;
};
#endif

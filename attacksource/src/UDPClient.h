/*

* Copyright (c) 2007,福建富士通信息软件有限公司网络安全事业部

* All rights reserved.

*

* 文件名称：UDPServer.h

* 文件标识：见配置管理计划书

* 摘    要：UDP客户端的头文件

*

* 当前版本：1.0

* 作    者：Garry < liubr@ffcs.cn >

* 完成日期：2007年9月11日

*

* 取代版本：

* 原作者  ：

* 完成日期：

*/

#ifndef __UDPCLIENT__H__
#define __UDPCLIENT__H__

#include <stdio.h>
#include <string.h>
#include <unistd.h>  

#ifdef LINUX
    #include <getopt.h>
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class CUdpClient
{
public:
	CUdpClient();
	~CUdpClient();

	int m_iOpen(int iPort, char *ServerIP);
	int m_iSendTo(char *pcMsgBuf,int iBufLen);
	void m_iClose();

protected:
	int m_iSockfd;
	struct sockaddr_in stSockAddr;
};

#endif

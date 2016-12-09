/*

* Copyright (c) 2007,������ʿͨ��Ϣ������޹�˾���簲ȫ��ҵ��

* All rights reserved.

*

* �ļ����ƣ�UDPServer.h

* �ļ���ʶ�������ù���ƻ���

* ժ    Ҫ��UDP�ͻ��˵�ͷ�ļ�

*

* ��ǰ�汾��1.0

* ��    �ߣ�Garry < liubr@ffcs.cn >

* ������ڣ�2007��9��11��

*

* ȡ���汾��

* ԭ����  ��

* ������ڣ�

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

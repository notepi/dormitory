/*

* Copyright (c) 2007,������ʿͨ��Ϣ������޹�˾���簲ȫ��ҵ��

* All rights reserved.

*

* �ļ����ƣ�UDPServer.h

* �ļ���ʶ�������ù���ƻ���

* ժ    Ҫ��UDP�ͻ��˵�ʵ��

*

* ��ǰ�汾��1.0

* ��    �ߣ�Garry < liubr@ffcs.cn >

* ������ڣ�2007��9��11��

*

* ȡ���汾��

* ԭ����  ��

* ������ڣ�

*/

#include "UDPClient.h"

/*
 *
 * ����: CUdpClient()
 *
 * ����: ���캯��
 *
 * ����: ��
 *
 * ����ֵ: ��
 *
 */
CUdpClient::CUdpClient()
{

}

/*
 *
 * ����: ~CUdpClient()
 *
 * ����: ��������
 *
 * ����: ��
 *
 * ����ֵ: ��
 *
 */
CUdpClient::~CUdpClient()
{

}

/*
 *
 * ����: m_iOpen(int iPort)
 *
 * ����:
 *      �����׽���
 *
 * ����:
 *      iPort: �����˿ں�
 *
 * ����ֵ:
 *      ����0 ��ʾ�ɹ�; ����1 ��ʾʧ��
 *
 */
int CUdpClient::m_iOpen(int iPort, char *ServerIP)
{
	  /* Create a socket */
	  m_iSockfd = socket(AF_INET, SOCK_DGRAM, 0);
	  if (m_iSockfd < 0) 
	  {
	  	fprintf(stderr, "%s:%d Intialize socket failed!\n", __FILE__, __LINE__);
	  	return 1;
	  }
    
	  // Bind the socket to specialized IP address and the specialized port.
	  memset(&stSockAddr, 0, sizeof(stSockAddr));
	  stSockAddr.sin_family = AF_INET;
	  stSockAddr.sin_addr.s_addr = inet_addr(ServerIP);
	  stSockAddr.sin_port = htons(iPort);
    
	  return 0;
}

/*
 *
 * ����: m_iSendTo(char *pcMsgBuf, int iBufLen)
 *
 * ����: ��������
 *
 * ����: pSocketAddr: �ͻ����׽��ֵ�ַ; pcMsgBuf: ���ջ�����; iBufLen: ���ջ���������
 *
 * ����ֵ: ���͵��ַ���������SOCKET_ERROR(����)
 *
 */
int CUdpClient::m_iSendTo(char *pcMsgBuf, int iBufLen)
{
	  int iSendNum = 0, iCliLen = 0;
    
	  iCliLen = sizeof(struct sockaddr_in);
	  iSendNum = sendto(m_iSockfd, pcMsgBuf, iBufLen, 0, (struct sockaddr*)&stSockAddr, iCliLen);
    
	  return iSendNum;
}

/*
 *
 * ����: m_iClose()
 *
 * ����: �ر��׽���
 *
 * ����: ��
 *
 * ����ֵ: ��
 *
 */
void CUdpClient::m_iClose()
{
	   close(m_iSockfd);
}

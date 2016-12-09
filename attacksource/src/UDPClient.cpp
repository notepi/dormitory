/*

* Copyright (c) 2007,福建富士通信息软件有限公司网络安全事业部

* All rights reserved.

*

* 文件名称：UDPServer.h

* 文件标识：见配置管理计划书

* 摘    要：UDP客户端的实现

*

* 当前版本：1.0

* 作    者：Garry < liubr@ffcs.cn >

* 完成日期：2007年9月11日

*

* 取代版本：

* 原作者  ：

* 完成日期：

*/

#include "UDPClient.h"

/*
 *
 * 函数: CUdpClient()
 *
 * 功能: 构造函数
 *
 * 参数: 无
 *
 * 返回值: 无
 *
 */
CUdpClient::CUdpClient()
{

}

/*
 *
 * 函数: ~CUdpClient()
 *
 * 功能: 析构函数
 *
 * 参数: 无
 *
 * 返回值: 无
 *
 */
CUdpClient::~CUdpClient()
{

}

/*
 *
 * 函数: m_iOpen(int iPort)
 *
 * 功能:
 *      建立套接字
 *
 * 参数:
 *      iPort: 监听端口号
 *
 * 返回值:
 *      返回0 表示成功; 返回1 表示失败
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
 * 函数: m_iSendTo(char *pcMsgBuf, int iBufLen)
 *
 * 功能: 发送数据
 *
 * 参数: pSocketAddr: 客户机套接字地址; pcMsgBuf: 接收缓冲区; iBufLen: 接收缓冲区长度
 *
 * 返回值: 发送的字符个数或者SOCKET_ERROR(出错)
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
 * 函数: m_iClose()
 *
 * 功能: 关闭套接字
 *
 * 参数: 无
 *
 * 返回值: 无
 *
 */
void CUdpClient::m_iClose()
{
	   close(m_iSockfd);
}

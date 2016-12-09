#ifndef __IPV4SOCKET_H__
#define __IPV4SOCKET_H__


int CreatIpv4TcpCleintSocket(const char* pfcIp, int nPort);
int CreatIpv4TcpServerSocket(const char* pfcIp, int nPort, int nBacklog);
int CreatIpv4UdpSocket( const char* pfcIp, int nPort);
int SetNonblocking(int nFd);

#endif
#ifndef __IPV4SOCKET_H__
#define __IPV4SOCKET_H__


int CreatIPV4TCPCleintSocket(const char* ip, int port);
int CreatIPVTCPServerSocket( const char* ip, int port, int backlog );
int Creat_IPV4_UDP_Socket( const char* ip, int port);
int SetNonblocking(int fd);

#endif
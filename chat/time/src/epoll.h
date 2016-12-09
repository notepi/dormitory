#ifndef __EPOLL_H__
#define __EPOLL_H__

void AddFd2Epoll(int epollfd, int listenfd, int enable);
void DeletFdFromEpoll (int epollfd, int listenfd);
void lt( epoll_event* events, int number, int epollfd, int listenfd);
void et( epoll_event* events, int number, int epollfd, int listenfd);

#endif
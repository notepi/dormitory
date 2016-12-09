#ifndef __EPOLL_H__
#define __EPOLL_H__

void AddFdToEpoll(int epollfd, int listenfd, int enable);
void DeleteFdFromEpoll (int nEpollFd, int nListenFd);
void lt( epoll_event* events, int number, int epollfd, int listenfd);
void et( epoll_event* events, int number, int epollfd, int listenfd);

#endif
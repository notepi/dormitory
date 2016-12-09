#ifndef __SIGNAL_H__
#define __SIGNAL_H__

void SIGUSR1Handler();
int  SignalProcessing(int sockfd);
void SigHandler (int signum);
void SigSet(int signum);


#endif


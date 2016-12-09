#ifndef _COMMAND_H_
#define _COMMAND_H_

#include<iostream>
#include <string>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
using namespace std;

class Make_Command {
	public:
		Make_Command(string pid, string filename);
		int Comannd_ps_aux ();							// 产生ps命令
		int Comannd_lsof();								// 产生lsof命令
		int Comannd_netstat ();							// 产生netstat命令
		
		string command;									// 用于存放产生的命令
		string FileName;
		string PID;
	
	private:


};

class Process_Info:public Make_Command  {
	public:
		Process_Info(string pid, string filename);
		int Info_Cpu_Memory();
		int Info_File_Num();
		int Info_Sock_Num();
		int Info_all();
	private:
		char buff[1024];
		const char * seps = " \n";						// 参数的分隔符
		char *GetNParameter(char *ptr, const char *seps, unsigned int N);
	
};

#endif

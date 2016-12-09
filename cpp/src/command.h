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
		int Comannd_ps_aux ();							// ����ps����
		int Comannd_lsof();								// ����lsof����
		int Comannd_netstat ();							// ����netstat����
		
		string command;									// ���ڴ�Ų���������
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
		const char * seps = " \n";						// �����ķָ���
		char *GetNParameter(char *ptr, const char *seps, unsigned int N);
	
};

#endif

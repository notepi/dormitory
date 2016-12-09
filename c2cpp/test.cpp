#include<iostream>
#include <string>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>




// 写入数据库


// 异常处理
// 保存文件处理
// 构析函数处理


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

Make_Command::Make_Command (string pid, string filename)
{
	PID = pid;					
	FileName = filename;
	
}

int Make_Command::Comannd_ps_aux ()
{	
	string ps_aux = "ps aux | grep ";
	
	command = ps_aux + " " + PID + " > " + FileName;
}

int Make_Command::Comannd_lsof ()
	{
		string lsof = "lsof -p";	
		command = lsof + " " + PID + " | grep REG > " + FileName;	
	}

int Make_Command::Comannd_netstat ()
{	
	string netstat = "netstat -anp";
	
	command = netstat + " | grep " + PID +" > " + FileName;
}


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

Process_Info::Process_Info(string pid, string filename):Make_Command(pid, filename)
	{
		
	}


int Process_Info::Info_Cpu_Memory()
{
	int flag = 0;
	char * pret;
								// 参数的分隔符
	FILE *stream;
	// 组合命令查询cpu和内存
	Comannd_ps_aux();
	
	system(command.c_str());
	stream = fopen(FileName.c_str(), "r");	
	if(NULL == stream){
		printf("fopen error!\n");
		return -1;
	}
	
	do{
		(pret = fgets(buff, sizeof(buff), stream));				// 从文件读取一行数据
		if(NULL == pret){
			return -1;
		}
		pret = GetNParameter(buff, seps, 2);					// 根据pid号判断是否是要读取的行
		if(0 == strcmp(pret, PID.c_str())){
			flag = 1;
			break;
		}
	}while(1);
	
	if(!flag){
		return -1;
	}
	
	pret = GetNParameter(NULL, seps, 1);						// 读取cpu的参数
	if(NULL == pret){
		printf("failed to find!\n");
		return -1;
	}  
	
	cout << "CPU: " << pret << endl;							// 打印cpu参数
	
	pret = GetNParameter(NULL, seps,1);							// 读取内存参数
	if(NULL == pret){
		printf("failed to find!\n");
		return -1;
	}  
	cout << "memory:" << pret << endl;							// 打印内存参数
	
	fclose(stream);
}
       
int Process_Info::Info_File_Num()
{
	FILE *stream;
	int NumCount = 0;
	char * pret;
	
	
	// 组合命令查询cpu和内存
	Comannd_lsof();
	system(command.c_str());
	
	stream = fopen(FileName.c_str(), "r");	

	while(pret = fgets(buff, sizeof(buff), stream)){
		pret = GetNParameter(buff, seps,4);						// 读取打开文件描述符数参数
		if(NULL == pret){
			cout <<"failed to find!" << endl;
			return -1;
		}
		if((*pret <= '9') && (*pret > '0')){					// 筛选符合要求的参数
			NumCount++;
		}	
	}
	cout << "fdnum is:"<< NumCount << endl;
	
	fclose(stream);
}     
       
int Process_Info::Info_Sock_Num()
{
	FILE *stream;
	int NumCount = 0;
	char *pret;
	char *port;
	
	
	// 组合命令查询cpu和内存
	Comannd_lsof();
	system(command.c_str());
	
	stream = fopen(FileName.c_str(), "r");	
// 组合命令查询连接数

	while(pret = fgets(buff, sizeof(buff), stream)){
		pret = GetNParameter(buff, seps,1);						// 读取连接类型参数
		if(NULL == pret){
			cout << "parament error!" << endl;
			return -1;
		}
		if(0 == strcmp(pret, "tcp")){							// tcp的链接
			pret = GetNParameter(NULL, seps,3);					// 读取链接地址参数
			if(NULL == pret){
				cout << "parament error!" << endl;
				return -1;
			}
			
			if(0 == strncmp(pret, "0.0.0.0", sizeof("0.0.0.0"))){// 是否是本地
				continue;
			}
			if(':' == *pret){									// :::port只统计链接数
				NumCount++;
				continue;
			}
			pret = strchr(pret, ':');							// 查询端口号
			if(NULL == pret){
				cout << "failed to find tcp port!" << endl;
				return -1;
			}
			port = ++pret;
			
			pret = GetNParameter(NULL, seps, 2);				// 读取链接状态
			if(NULL == pret){
				cout << "parament error!" << endl;
				return -1;
			}
			
			if((0 != strcmp(pret, "LISTEN")) && (0 != strcmp(pret, "ESTABLISHED"))){
				continue;										// 非以上两种状态的链接不统计
			}
			
			cout << "port is:" << port << endl;
			NumCount++;											// 连接数加1
			continue;
		}
		
		if(0 == strcmp(pret, "tcp6")){
			NumCount++;											// 对于tcp6只统计连接数
			continue;	
		}
		
		if(0 == strcmp(pret, "udp")){
			pret = GetNParameter(NULL, seps,3);					// 读取链接地址
			if(NULL == pret){
				cout << "parament error!" << endl;
				return -1;
			}
			if(0 == strncmp(pret, "0.0.0.0", sizeof("0.0.0.0"))){// 是否是本地
				continue;
			}
			if(':' == *pret){									// :::port只统计链接数
				NumCount++;
				continue;
			}
			
			pret = strchr(pret, ':');
			if(NULL == pret){
				cout << "failed to find unp port!" << endl;
				return -1;
			}
			
			cout << "port is:%s" << ++pret << endl;
			NumCount++;											// 连接数加1
		}
	}
	cout << "sockfd num is:" << NumCount <<endl;
	fclose(stream);
}
       
int Process_Info:: Info_all()
	{
		Info_Cpu_Memory();
		Info_File_Num();
		Info_Sock_Num();
	}
       
       
       
char * Process_Info :: GetNParameter(char *ptr, const char *seps, unsigned int N)
{
	char *pret;
	pret = strtok(ptr, seps);
	
	while((pret != NULL) && N){
		if((--N) <= 0){
			break;
		}
		pret = strtok( NULL, seps );//首次调用时，第一个参数string指向要分解的字符串，之后再次调用要把s设成NULL
	}
	return pret;
}       






                                                                                                                                                                                                                                                                                                                                                                        
int main()
{
	Process_Info ocj("6788", "./a.txt");
	ocj.Info_all();
	cout << ocj.command << endl;
	return 0;
}

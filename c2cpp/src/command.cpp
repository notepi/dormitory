#include "command.h" 
/*****************************************************
 * 函数	 : Make_Command
 *
 * 功能  : 初始化类的参数
 *
 * 参数  : 传入的pid参数和保存产生的中间文件名以及地址
 *			 
 *
 * 返回值: 无
 *****************************************************/
Make_Command::Make_Command (string pid, string filename)
{
	PID = pid;					
	FileName = filename;
	
}
/*****************************************************
 * 函数	 : Comannd_ps_aux
 *
 * 功能  : 构造ps命令参数
 *
 * 参数  : 无
 *			 
 *
 * 返回值: 无
 *****************************************************/
int Make_Command::Comannd_ps_aux ()
{	
	string ps_aux = "ps aux | grep ";
	
	command = ps_aux + " " + PID + " > " + FileName;
}
/*****************************************************
 * 函数	 : Comannd_lsof
 *
 * 功能  : 构造lsof命令参数
 *
 * 参数  : 无
 *			 
 *
 * 返回值: 无
 *****************************************************/
int Make_Command::Comannd_lsof ()
	{
		string lsof = "lsof -p";	
		command = lsof + " " + PID + " | grep REG > " + FileName;	
	}
/*****************************************************
 * 函数	 : Comannd_netstat
 *
 * 功能  : 构造netstat命令参数
 *
 * 参数  : 无
 *			 
 *
 * 返回值: 无
 *****************************************************/
int Make_Command::Comannd_netstat ()
{	
	string netstat = "netstat -anp";
	
	command = netstat + " | grep " + PID +" > " + FileName;
}
/*****************************************************
 * 函数	 : Process_Info
 *
 * 功能  : 继承类的初始化函数
 *
 * 参数  : 无
 *			 
 *
 * 返回值: 无
 *****************************************************/

Process_Info::Process_Info(string pid, string filename):Make_Command(pid, filename)
	{
		
	}

/*****************************************************
 * 函数	 : Info_Cpu_Memory
 *
 * 功能  : 读取进程的cpu占用率和内存使用情况
 *
 * 参数  : 无
 *			 
 *
 * 返回值: 无
 *****************************************************/
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
 /*****************************************************
 * 函数	 : Info_File_Num
 *
 * 功能  : 读取进程的cpu占用率和内存使用情况
 *
 * 参数  : 无
 *			 
 *
 * 返回值: 无
 *****************************************************/      
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
 /*****************************************************
 * 函数	 : Info_Sock_Num
 *
 * 功能  : 读取进程的sock使用状态
 *
 * 参数  : 无
 *			 
 *
 * 返回值: 无
 *****************************************************/        
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
 /*****************************************************
 * 函数	 : Info_all
 *
 * 功能  : 读取进程的sock使用状态，内存、cpu使用率，文件描述符打开情况
 *
 * 参数  : 无
 *			 
 *
 * 返回值: 无
 *****************************************************/           
int Process_Info:: Info_all()
	{
		Info_Cpu_Memory();
		Info_File_Num();
		Info_Sock_Num();
	}
 /*****************************************************
 * 函数	 : GetNParameter
 *
 * 功能  : 根据分隔符对字符串参数解析
 *
 * 参数  : ptr：字符串，第一次解析传入字符串地址，以后则传入NULL，
 * 				seps：分隔符号，
 *					N：要解析参数的位置，当为0时返回当前参数，
 *						大于0表示当前参数以后的第N个参数
 *			 
 *
 * 返回值: 获取的参数地址，可直接copy
 *****************************************************/ 	
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
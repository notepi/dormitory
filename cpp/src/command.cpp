#include "command.h" 
/*****************************************************
 * ����	 : Make_Command
 *
 * ����  : ��ʼ����Ĳ���
 *
 * ����  : �����pid�����ͱ���������м��ļ����Լ���ַ
 *			 
 *
 * ����ֵ: ��
 *****************************************************/
Make_Command::Make_Command (string pid, string filename)
{
	PID = pid;					
	FileName = filename;
	
}
/*****************************************************
 * ����	 : Comannd_ps_aux
 *
 * ����  : ����ps�������
 *
 * ����  : ��
 *			 
 *
 * ����ֵ: ��
 *****************************************************/
int Make_Command::Comannd_ps_aux ()
{	
	string ps_aux = "ps aux | grep ";
	
	command = ps_aux + " " + PID + " > " + FileName;
}
/*****************************************************
 * ����	 : Comannd_lsof
 *
 * ����  : ����lsof�������
 *
 * ����  : ��
 *			 
 *
 * ����ֵ: ��
 *****************************************************/
int Make_Command::Comannd_lsof ()
	{
		string lsof = "lsof -p";	
		command = lsof + " " + PID + " | grep REG > " + FileName;	
	}
/*****************************************************
 * ����	 : Comannd_netstat
 *
 * ����  : ����netstat�������
 *
 * ����  : ��
 *			 
 *
 * ����ֵ: ��
 *****************************************************/
int Make_Command::Comannd_netstat ()
{	
	string netstat = "netstat -anp";
	
	command = netstat + " | grep " + PID +" > " + FileName;
}
/*****************************************************
 * ����	 : Process_Info
 *
 * ����  : �̳���ĳ�ʼ������
 *
 * ����  : ��
 *			 
 *
 * ����ֵ: ��
 *****************************************************/

Process_Info::Process_Info(string pid, string filename):Make_Command(pid, filename)
	{
		
	}

/*****************************************************
 * ����	 : Info_Cpu_Memory
 *
 * ����  : ��ȡ���̵�cpuռ���ʺ��ڴ�ʹ�����
 *
 * ����  : ��
 *			 
 *
 * ����ֵ: ��
 *****************************************************/
int Process_Info::Info_Cpu_Memory()
{
	int flag = 0;
	char * pret;
								// �����ķָ���
	FILE *stream;
	// ��������ѯcpu���ڴ�
	Comannd_ps_aux();
	
	system(command.c_str());
	stream = fopen(FileName.c_str(), "r");	
	if(NULL == stream){
		printf("fopen error!\n");
		return -1;
	}
	
	do{
		(pret = fgets(buff, sizeof(buff), stream));				// ���ļ���ȡһ������
		if(NULL == pret){
			return -1;
		}
		pret = GetNParameter(buff, seps, 2);					// ����pid���ж��Ƿ���Ҫ��ȡ����
		if(0 == strcmp(pret, PID.c_str())){
			flag = 1;
			break;
		}
	}while(1);
	
	if(!flag){
		return -1;
	}
	
	pret = GetNParameter(NULL, seps, 1);						// ��ȡcpu�Ĳ���
	if(NULL == pret){
		printf("failed to find!\n");
		return -1;
	}  
	
	cout << "CPU: " << pret << endl;							// ��ӡcpu����
	
	pret = GetNParameter(NULL, seps,1);							// ��ȡ�ڴ����
	if(NULL == pret){
		printf("failed to find!\n");
		return -1;
	}  
	cout << "memory:" << pret << endl;							// ��ӡ�ڴ����
	
	fclose(stream);
}
 /*****************************************************
 * ����	 : Info_File_Num
 *
 * ����  : ��ȡ���̵�cpuռ���ʺ��ڴ�ʹ�����
 *
 * ����  : ��
 *			 
 *
 * ����ֵ: ��
 *****************************************************/      
int Process_Info::Info_File_Num()
{
	FILE *stream;
	int NumCount = 0;
	char * pret;
	
	
	// ��������ѯcpu���ڴ�
	Comannd_lsof();
	system(command.c_str());
	
	stream = fopen(FileName.c_str(), "r");	

	while(pret = fgets(buff, sizeof(buff), stream)){
		pret = GetNParameter(buff, seps,4);						// ��ȡ���ļ�������������
		if(NULL == pret){
			cout <<"failed to find!" << endl;
			return -1;
		}
		if((*pret <= '9') && (*pret > '0')){					// ɸѡ����Ҫ��Ĳ���
			NumCount++;
		}	
	}
	cout << "fdnum is:"<< NumCount << endl;
	
	fclose(stream);
}     
 /*****************************************************
 * ����	 : Info_Sock_Num
 *
 * ����  : ��ȡ���̵�sockʹ��״̬
 *
 * ����  : ��
 *			 
 *
 * ����ֵ: ��
 *****************************************************/        
int Process_Info::Info_Sock_Num()
{
	FILE *stream;
	int NumCount = 0;
	char *pret;
	char *port;
	
	
	// ��������ѯcpu���ڴ�
	Comannd_lsof();
	system(command.c_str());
	
	stream = fopen(FileName.c_str(), "r");	
// ��������ѯ������

	while(pret = fgets(buff, sizeof(buff), stream)){
		pret = GetNParameter(buff, seps,1);						// ��ȡ�������Ͳ���
		if(NULL == pret){
			cout << "parament error!" << endl;
			return -1;
		}
		if(0 == strcmp(pret, "tcp")){							// tcp������
			pret = GetNParameter(NULL, seps,3);					// ��ȡ���ӵ�ַ����
			if(NULL == pret){
				cout << "parament error!" << endl;
				return -1;
			}
			
			if(0 == strncmp(pret, "0.0.0.0", sizeof("0.0.0.0"))){// �Ƿ��Ǳ���
				continue;
			}
			if(':' == *pret){									// :::portֻͳ��������
				NumCount++;
				continue;
			}
			pret = strchr(pret, ':');							// ��ѯ�˿ں�
			if(NULL == pret){
				cout << "failed to find tcp port!" << endl;
				return -1;
			}
			port = ++pret;
			
			pret = GetNParameter(NULL, seps, 2);				// ��ȡ����״̬
			if(NULL == pret){
				cout << "parament error!" << endl;
				return -1;
			}
			
			if((0 != strcmp(pret, "LISTEN")) && (0 != strcmp(pret, "ESTABLISHED"))){
				continue;										// ����������״̬�����Ӳ�ͳ��
			}
			
			cout << "port is:" << port << endl;
			NumCount++;											// ��������1
			continue;
		}
		
		if(0 == strcmp(pret, "tcp6")){
			NumCount++;											// ����tcp6ֻͳ��������
			continue;	
		}
		
		if(0 == strcmp(pret, "udp")){
			pret = GetNParameter(NULL, seps,3);					// ��ȡ���ӵ�ַ
			if(NULL == pret){
				cout << "parament error!" << endl;
				return -1;
			}
			if(0 == strncmp(pret, "0.0.0.0", sizeof("0.0.0.0"))){// �Ƿ��Ǳ���
				continue;
			}
			if(':' == *pret){									// :::portֻͳ��������
				NumCount++;
				continue;
			}
			
			pret = strchr(pret, ':');
			if(NULL == pret){
				cout << "failed to find unp port!" << endl;
				return -1;
			}
			
			cout << "port is:%s" << ++pret << endl;
			NumCount++;											// ��������1
		}
	}
	cout << "sockfd num is:" << NumCount <<endl;
	fclose(stream);
}
 /*****************************************************
 * ����	 : Info_all
 *
 * ����  : ��ȡ���̵�sockʹ��״̬���ڴ桢cpuʹ���ʣ��ļ������������
 *
 * ����  : ��
 *			 
 *
 * ����ֵ: ��
 *****************************************************/           
int Process_Info:: Info_all()
	{
		Info_Cpu_Memory();
		Info_File_Num();
		Info_Sock_Num();
	}
 /*****************************************************
 * ����	 : GetNParameter
 *
 * ����  : ���ݷָ������ַ�����������
 *
 * ����  : ptr���ַ�������һ�ν��������ַ�����ַ���Ժ�����NULL��
 * 				seps���ָ����ţ�
 *					N��Ҫ����������λ�ã���Ϊ0ʱ���ص�ǰ������
 *						����0��ʾ��ǰ�����Ժ�ĵ�N������
 *			 
 *
 * ����ֵ: ��ȡ�Ĳ�����ַ����ֱ��copy
 *****************************************************/ 	
char * Process_Info :: GetNParameter(char *ptr, const char *seps, unsigned int N)
{
	char *pret;
	pret = strtok(ptr, seps);
	
	while((pret != NULL) && N){
		if((--N) <= 0){
			break;
		}
		pret = strtok( NULL, seps );//�״ε���ʱ����һ������stringָ��Ҫ�ֽ���ַ�����֮���ٴε���Ҫ��s���NULL
	}
	return pret;
}  
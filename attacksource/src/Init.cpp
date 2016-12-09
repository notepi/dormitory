/*

* Copyright (c) 2007,������ʿͨ��Ϣ�������޹�˾���簲ȫ��ҵ��

* All rights reserved.

*

* �ļ����ƣ�Init.cpp

* �ļ���ʶ�������ù����ƻ���

* ժ    Ҫ��Ϊ����ģ���ṩͳһ�Ľӿڣ����ڼ�������չ

*

* ��ǰ�汾��1.0

* ��    �ߣ�huanglr < huanglr@ffcs.cn >

* �������ڣ�2014��11��28��

*

* ȡ���汾��

* ԭ����  ��

* �������ڣ�

*/
#include <iostream>
#include "Init.h"
#include "MqMonitor.h"
#include "DataDefine.h"
#include "Log.h"

extern CRabbitMqClient g_Client;
extern CRabbitMqClient g_Client_1;
extern CRabbitMqClient g_Client_2;
extern CRabbitMqClient g_Client_Rule;
#define _HAINAN_DB_
char ReportPath[256];
char ReportStorePath[256];
int CSystemInit::GetConfigString(const char *pSession, const char *pKey, char *pValue, char *pBuf)
{
		char *pConfig, *pDetail, *pTemp;
	  char Item[100];
	  int i;
    
	  if (sizeof(Item) < (strlen(pSession) + 3) || sizeof(Item) < (strlen(pKey) + 1))
	  {
	  	  return -1;
	  }
    
	  //search the string session "[%s]"
	  snprintf(Item,sizeof(Item), "[%s]", pSession);
	  pConfig = strstr(pBuf, Item);
	  if (pConfig == NULL)
	  {
	  	  return -1;
	  }
	  pConfig += strlen(Item);
    
	  //search the string key "%s"
	  snprintf(Item,sizeof(Item), "%s", pKey);	
	  pDetail = strstr(pConfig, Item);
	  if (pDetail == NULL)
	  {
	  	  return -1;
	  }
    
	  pDetail = strchr(pDetail, '=');
	  if(pDetail == NULL)
	  {
	  	  return -1;
	  }
	  ++pDetail;   //go to the end of "="
    
	  //go to the end of "=  "
	  for (i = 0; i < strlen(pDetail); ++i)
	  {
	  	  if (pDetail[i] == ' ')
	  	  {
	  	  	  continue;
	  	  }
	  	  else
	  	  {
	  	  	  break;
	  	  }
	  }
	  pDetail += i;
    
	  //make pTemp point to the end of string "value"
	  pTemp = strchr(pDetail, '\r'); 
	  if (pTemp == NULL)
	  {
	  	  pTemp = strchr(pDetail, '\n');
	  	  //the last line of buffer
	  	  if (pTemp == NULL)
	  	  {
	  	  	  pTemp = pBuf + strlen(pBuf);
	  	  }
	  }
    
	  for (i = 0; i < pTemp - pDetail; ++i)
	  {
	  	  if (pDetail[i] == ' ')
	  	  {
	  	  	  pTemp = pDetail + i;
	  	  	  break;
	  	  }
	  }
    
	  strncpy(pValue, pDetail, pTemp - pDetail);
	  pValue[pTemp - pDetail] = '\0';
	  return 0;
}

int CSystemInit::ReadConfigFile(const char *pFileName, char *pFileBuf)
{
		FILE *fp = NULL;
	  struct stat StatInfo;
	  char ErrLog[500];
	  memset(ErrLog , 0 ,500);
	  
	  if ((fp = fopen(pFileName, "r")) == NULL)
	  {
	  	  snprintf(ErrLog, sizeof(ErrLog), "open config file %s fail at %s:%d\n",pFileName, __FILE__, __LINE__);
	  	  ErrLog[499] = '\0';
	  	  //WriteLogFile("NSSSVR-001",ErrLog,3);
	  	  return -1;
	  }
    
	  if (stat(pFileName, &StatInfo) == -1)
	  {
	  	  snprintf(ErrLog, sizeof(ErrLog), "get config file %s attribute fail at %s:%d\n",pFileName, __FILE__, __LINE__);
	  	  ErrLog[499] = '\0';
	  	  //WriteLogFile("NSSSVR-001",ErrLog,3);
	      fclose(fp);
	      fp = NULL;	  	
	  	  return -1;
	  }
    
	  if (fread(pFileBuf, StatInfo.st_size, 1, fp) != 1)
	  {
	  	  snprintf(ErrLog, sizeof(ErrLog), "read config file %s fail at %s:%d\n",pFileName, __FILE__, __LINE__);
	  	  ErrLog[499] = '\0';
	  	  //WriteLogFile("NSSSVR-001",ErrLog,3);
	      fclose(fp);
	      fp = NULL;	  	
	  	  return -1;
	  }
    
	  fclose(fp);
	  fp = NULL;
	  return 0;
}

void CSystemInit::InitDatabase()
{
		int iRet;
	  char file_buffer[4096*2];
	  char User[100] = {0};
    char Pwd[100] = {0};
    char Conn[16] = {0};
    char RabittMQServer[20] ={0};
	  char ErrLog[500];
	  memset(ErrLog , 0 ,500);
	  //���������ļ�,��ȡ������Ϣ
	  iRet = ReadConfigFile("../config/flow.conf", file_buffer);
	  if(iRet == -1)
	  {
	  	  snprintf(ErrLog, sizeof(ErrLog), "read config file %s fail at %s:%d\n","", __FILE__, __LINE__);
	  	  ErrLog[499] = '\0';
	  	  CSystemLog inf("Oralce error",ErrLog);	  
	  	  return;
	  }
    //�������ݿ����û���
	  iRet = GetConfigString("Database", "User", User, file_buffer);
	  if(iRet == -1)
	  {
	  	  fprintf(stderr, "Read [Database] -> User failed!\n");
	  	  snprintf(ErrLog, sizeof(ErrLog), "config file format is error %s:%d\n", __FILE__, __LINE__);
	  	  ErrLog[499] = '\0';
	  	  CSystemLog inf("Oralce error",ErrLog);	  	
	  	  return;
	  }
	  this->strUser = User;
    //�������ݿ�������
	  iRet = GetConfigString("Database", "Pwd", Pwd, file_buffer);
	  if(iRet == -1)
	  {
		    fprintf(stderr, "Read [Database] -> Pwd failed!\n");
	  	  snprintf(ErrLog, sizeof(ErrLog), "config file format is error %s:%d\n", __FILE__, __LINE__);
	  	  ErrLog[499] = '\0';
	  	  CSystemLog inf("Oralce error",ErrLog);	  
	  	  return;
	  }
	  this->strPwd = Pwd;
    //�������ݿ������Ӵ�
    iRet = GetConfigString("Database", "Conn_str", Conn, file_buffer);
    if(iRet == -1)
	  {
		    fprintf(stderr, "Read [Database] -> Conn_str failed!\n");
	  	  snprintf(ErrLog, sizeof(ErrLog), "config file format is error %s:%d\n", __FILE__, __LINE__);
	  	  ErrLog[499] = '\0';
	  	  CSystemLog inf("Oralce error",ErrLog);	  
	  	  return;
	  }
	  this->strConn = Conn;
	  
	  iRet = GetConfigString("rabittmq", "Server", RabittMQServer, file_buffer);
    if(iRet == -1)
	  {
		    fprintf(stderr, "Read [Database] -> Conn_str failed!\n");
	  	  snprintf(ErrLog, sizeof(ErrLog), "config file format is error %s:%d\n", __FILE__, __LINE__);
	  	  ErrLog[499] = '\0';
	  	  CSystemLog inf("Oralce error",ErrLog);	  
	  	  return;
	  }
	  this->strRabittServer = RabittMQServer;
	  printf("server:%s\n",RabittMQServer);
	  char cMQuser[64] = {0};
	  iRet = GetConfigString("rabittmq", "user", cMQuser, file_buffer);
	  if(iRet == -1) 
	  {   
		  fprintf(stderr, "Read [Database] -> Conn_str failed!\n");
		  snprintf(ErrLog, sizeof(ErrLog), "config file format is error %s:%d\n", __FILE__, __LINE__);
		  ErrLog[499] = '\0';
		  CSystemLog inf("Oralce error",ErrLog);    
		  return;
	  }   
	  this->strMqUser = cMQuser;
	  char cMQpwd[64]={0};
	  iRet = GetConfigString("rabittmq", "pwd", cMQpwd, file_buffer);
	  if(iRet == -1) 
	  {   
		  fprintf(stderr, "Read [Database] -> Conn_str failed!\n");
		  snprintf(ErrLog, sizeof(ErrLog), "config file format is error %s:%d\n", __FILE__, __LINE__);
		  ErrLog[499] = '\0';
		  CSystemLog inf("Oralce error",ErrLog);    
		  return;
	  }   
	  this->strMqPwd = cMQpwd;
	  char MQport[16] = {0};
	  iRet = GetConfigString("rabittmq", "port", MQport, file_buffer);
	  if(iRet == -1) 
	  {   
		  fprintf(stderr, "Read [Database] -> Conn_str failed!\n");
		  snprintf(ErrLog, sizeof(ErrLog), "config file format is error %s:%d\n", __FILE__, __LINE__);
		  ErrLog[499] = '\0';
		  CSystemLog inf("Oralce error",ErrLog);    
		  return;
	  }   
	  this->iMqPort = atoi(MQport);
	  char cExchange[64] = {0};
	  iRet = GetConfigString("rabittmq", "exchange", cExchange, file_buffer);
	  if(iRet == -1)
	  {   
		  fprintf(stderr, "Read [Database] -> Conn_str failed!\n");
		  snprintf(ErrLog, sizeof(ErrLog), "config file format is error %s:%d\n", __FILE__, __LINE__);
		  ErrLog[499] = '\0';
		  CSystemLog inf("Oralce error",ErrLog);    
		  return;
	  }   
	  strMqExchange = cExchange;
	  char cQueue[64] = {0};
	  iRet = GetConfigString("rabittmq", "queue", cQueue, file_buffer);
	  if(iRet == -1)
	  {  
		  fprintf(stderr, "Read [Database] -> Conn_str failed!\n");
		  snprintf(ErrLog, sizeof(ErrLog), "config file format is error %s:%d\n", __FILE__, __LINE__);
		  ErrLog[499] = '\0';
		  CSystemLog inf("Oralce error",ErrLog);    
		  return;
	  }   
	  strMqQueue = cQueue;
	  debug_log("Read [ex:%s,queue:%s]\n",strMqExchange.c_str(),strMqQueue.c_str());
	  iRet = GetConfigString("ReportPath", "Path", ReportPath, file_buffer);
    	  if(iRet == -1)
          {
                    fprintf(stderr, "Read [ReportPath] -> Path failed!\n");
                  snprintf(ErrLog, sizeof(ErrLog), "config file format is error %s:%d\n", __FILE__, __LINE__);
                  ErrLog[499] = '\0';
                  return;
          }
	  iRet = GetConfigString("ReportStorePath", "Path", ReportStorePath, file_buffer);
          if(iRet == -1)
          {
                    fprintf(stderr, "Read [ReportStorePath] -> Path failed!\n");
                  snprintf(ErrLog, sizeof(ErrLog), "config file format is error %s:%d\n", __FILE__, __LINE__);
                  ErrLog[499] = '\0';
                  return;
          }
	  //CSystemLog inf("Init","CONF Load End!");	
	  info_log("int conf load end!");
}

void CInit::ConnectInit(string user,string pwd,string conn,string RabittServer,string mquser,string mqpwd,int port)
{
		char ErrLog[500];
    memset(ErrLog , 0 ,500);	
   	CDataBase *one = CDataBase::GetInstance();
		try
		{
      	one->g_Env = Environment::createEnvironment(Environment::THREADED_MUTEXED);
      	one->g_Conn = (one->g_Env)->createConnection(user, pwd, conn);
    }
		catch(SQLException ex)
    {
				cout << "��ʼ�������쳣" << endl;
				cout << "Error number: " << ex.getErrorCode() << endl;
				cout << (ex.getMessage()).c_str() << endl;
	  		snprintf(ErrLog, sizeof(ErrLog), "Init Database connect fail at %s:%d, Error number is %d,Error is %s\n", __FILE__, __LINE__,ex.getErrorCode(),(ex.getMessage()).c_str());
	  		ErrLog[499] = '\0';
	  		CSystemLog inf("Oralce error",ErrLog);	   		    
				return;
		}
	printf("server:%s\n",RabittServer.c_str());
	g_Client.RabbitMqInit(RabittServer.c_str(),mquser,mqpwd,port);
	//g_Client_1.RabbitMqInit(strRabittServer.c_str());
	 //modify by yuzf 20160613 soc_flowֻ����һ��MQ
	//g_Client_2.RabbitMqInit(strRabittServer.c_str(),mquser,mqpwd,port);
	//g_Client_Rule.RabbitMqInit(strRabittServer.c_str());
		return;
}

int CInit::InitFlowID()
{
	char ErrLog[500] = {0};
#ifdef _HAINAN_DB_
	std::string sqlStmt = "select b.ip from asset_info a,asset_ip b where a.assetid = b.assetid and a.devicetype like '303%'";
#else
	std::string sqlStmt = "select b.ipaddress from asset_info a,asset_ip b where a.assetid = b.assetid and a.devicetype like '303%'";
#endif
	Statement* stmt = NULL;
  ResultSet* rset = NULL;
  CDataBase *one = CDataBase::GetInstance();  
  try
  {
       stmt = (one->g_Conn)->createStatement(sqlStmt);
       rset = stmt->executeQuery();
			 std::string temp;
       while(rset->next())
       {
					temp = "";
          if(!(rset->isNull(1)))
          {  
		  	temp = rset->getString(1);
			//CSystemLog info("get asset ip",temp.c_str());	
			info_log("get asset ip:[%s]",temp.c_str());
			g_FlowAssetipList.push_back(temp);
		  }
       }
       stmt->closeResultSet(rset);
       (one->g_Conn)->terminateStatement(stmt);
  }
	catch(SQLException ex)
	{
		 fprintf(stderr, "Exception thrown init flowid at %s:%d\n", __FILE__, __LINE__);
     cout << "Error number: " << ex.getErrorCode() << endl;
     cout << (ex.getMessage()).c_str() << endl;
     (one->g_Conn)->terminateStatement(stmt);
     snprintf(ErrLog, sizeof(ErrLog), "Exception thrown  init flowid fail at %s:%d, Error number is %d,Error is %s\n", __FILE__, __LINE__,ex.getErrorCode(),(ex.getMessage()).c_str());
		 ErrLog[499] = '\0';
      CSystemLog inf("Oralce error",ErrLog);	     
	}
  
  
  
}

int CInit::SendAdsInfo()
{
		std::string sqlStmt = "SELECT adsip,adskey,adsport from ads_info";
    Statement *stmt = NULL;
    ResultSet *rset = NULL;
    CDataBase *one = CDataBase::GetInstance();  
    char context[160] = {0};
    char ErrLog[500] = {0};
    /*���Ϳ�ʼ��*/	  
    ADSINFO node;
    memset(&node,0,sizeof(ADSINFO));
    //sprintf(node.MsgType,"%d",4);
    //node.Port = -1;
    //memcpy(context,&node,sizeof(ADSINFO));
    //g_Client.RabbitMqSend(context,sizeof(ADSINFO));
    /*******************************************************/
    
    try
    {
            stmt = (one->g_Conn)->createStatement(sqlStmt);

            rset = stmt->executeQuery ();
            while(rset->next())
           {
           		memset(&node,0,sizeof(ADSINFO));
           		//memset(context,0,sizeof(context));
              strncpy(node.AdsIP,(char *)rset->getString(1).c_str(),sizeof(node.AdsIP));
              sprintf(node.MsgType,"%d",4);
              strncpy(node.Key,(char *)rset->getString(2).c_str(),sizeof(node.Key));
              node.Port = rset->getInt(3);
              //memcpy(context,&node,sizeof(ADSINFO));
              //g_Client.RabbitMqSend(context,sizeof(ADSINFO));
              g_AdsIpPortListTmp.push_back(node);
           }
           stmt->closeResultSet(rset);
            (one->g_Conn)->terminateStatement(stmt);
    }//end try
    catch(SQLException ex)
    {
        fprintf(stderr, "Exception thrown Read ads info at %s:%d\n", __FILE__, __LINE__);
        cout << "Error number: " << ex.getErrorCode() << endl;
        cout << (ex.getMessage()).c_str() << endl;
        (one->g_Conn)->terminateStatement(stmt);
        snprintf(ErrLog, sizeof(ErrLog), "Exception thrown  Read ads info fail at %s:%d, Error number is %d,Error is %s\n", __FILE__, __LINE__,ex.getErrorCode(),(ex.getMessage()).c_str());
				ErrLog[499] = '\0';
        CSystemLog inf("Oralce error",ErrLog);	       
    }
    /*���ͽ�����*/
    /*memset(&node,0,sizeof(ADSINFO));
    sprintf(node.MsgType,"%d",4);
    node.Port = -4;
    memcpy(context,&node,sizeof(ADSINFO));
    g_Client.RabbitMqSend(context,sizeof(ADSINFO));*/
    CSystemLog inf("Init","Ads Info Send To Collect End!");
}
 

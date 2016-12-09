/*

* Copyright (c) 2007,������ʿͨ��Ϣ�������޹�˾���簲ȫ��ҵ��

* All rights reserved.

*

* �ļ����ƣ�Init.h

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
#ifndef _INIT_H_
#define _INIT_H_
#include<stdio.h>
#include<string>
#include<string.h>
#include<occi.h>
#include<unistd.h>
#include<sys/stat.h>
#include<errno.h>
#include "Tools.h"
#include "List.h"
 
using namespace std;
using namespace oracle::occi;
 
	
class CDataBase
{
	private: static CDataBase *database; 
	private: CDataBase()
	{
		printf("new database connection !\n");	
	}
	public:
	static void DatabaseDestroy()
	{
		if((database->g_Conn != NULL) && (database->g_Env != NULL) )
		{
		    database->g_Env->terminateConnection(database->g_Conn);
	 	    Environment::terminateEnvironment(database->g_Env);
		    printf("delete database connection!\n");
		}
	}
	public: static CDataBase *GetInstance()
	{
		if(database == NULL)
		{
			database = new CDataBase();
		}
		return database;
	}
	public:
	oracle::occi::Environment *g_Env;
	oracle::occi::Connection *g_Conn;
	int iMaxID;
};
class CSystemInit
{
public:
	CSystemInit(){};
	~CSystemInit(){};
	void InitDatabase();
	int ReadConfigFile(const char *pFileName, char *pFileBuf);
	int GetConfigString(const char *pSession, const char *pKey, char *pValue, char *pBuf);
public:
  string strUser;
  string strPwd;
  string strConn;
  string strRabittServer;
  string strMqUser;
  string strMqPwd;
  int iMqPort;
  int iMaxID;
};

class CInit : public CSystemInit
{
public:	
	CInit()
	{
		InitDatabase();
		ConnectInit(strUser,strPwd,strConn,strRabittServer,strMqUser,strMqPwd,iMqPort);
		InitFlowID();
		//SendAdsInfo();
	}
	~CInit(){};
	void ConnectInit(string user,string pwd,string conn,string RabittServer,string mquser,string mqpwd,int port);
  int InitFlowID();
	int SendAdsInfo();
};
class CSystemLog
{
public:
	CSystemLog(const char *action,const char *Log)
	{
		fp = NULL;
		std::string strPath = "../log/flow_";
		std::string strDay = GetSystemDay();
		strPath = strPath + strDay + ".log";
		fp = fopen(strPath.c_str(),"a+");
		if(fp == NULL)
				return;
		fprintf(fp,"%s:[%s:][%s]\n",GetSystemTime().c_str(),action,Log);	
		fclose(fp);
		fp = NULL;
	}
	~CSystemLog()
	{
		if(fp != NULL)
			fclose(fp);		
	}
private:
	FILE *fp;
};
#endif

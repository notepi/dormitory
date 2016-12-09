/*

* Copyright (c) 2007,福建富士通信息软件有限公司网络安全事业部

* All rights reserved.

*

* 文件名称：MqMonitor.h

* 文件标识：见配置管理计划书

* 摘    要：为各个模块提供统一的接口，便于集成与扩展

*

* 当前版本：1.0

* 作    者：huanglr < huanglr@ffcs.cn >

* 完成日期：2014年11月28日

*

* 取代版本：

* 原作者  ：

* 完成日期：

*/
#ifndef _MQMONITOR_H_
#define _MQMONITOR_H_

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<map>
#include<math.h>
#include<arpa/inet.h>
#include "SimpleAmqpClient/BasicMessage.h"
#include "SimpleAmqpClient/Channel.h"
#include "Init.h"
#include "Tools.h"
#include "AdsReport.h"
#include "amqp.h"
#include "DataDefine.h"
#include "AdsMonitor.h"
#include "Log.h"
extern _List<std::string> g_FlowAssetipList;

class CRabbitMqClient
{
public:
	    AmqpClient::Channel::ptr_t channel;
		void RabbitMqInit(std::string strServerIp,std::string strUser,std::string strPwd,int iPort){
		try
		{
			printf("%s,%s,%s,%d\n",strServerIp.c_str(),strUser.c_str(),strPwd.c_str(),iPort);
			//channel = AmqpClient::Channel::Create("110.84.128.185", 5672,"guest","FFCS@soc9", "/", 131072);
			channel = AmqpClient::Channel::Create(strServerIp, iPort,strUser.c_str(),strPwd.c_str(), "/", 131072);
			if(channel == NULL)
				return;
		    printf("bb\n");
			channel->DeclareExchange(SENDTOCOLLECTEX,
                AmqpClient::Channel::EXCHANGE_TYPE_FANOUT);
      channel->DeclareExchange(RULEEX,
                AmqpClient::Channel::EXCHANGE_TYPE_FANOUT);
		}
		catch(std::exception& r_e)
		{
				//CSystemLog info("connect rabbitmq-server error",r_e.what());
				printf("AA:connect rabbitmq-server error:%s",r_e.what());
				exit(0);
		}
	}

	virtual bool RabbitMqRecv(std::string &mess){
		try
		{
			AmqpClient::Envelope::ptr_t env;
			if(this->channel->BasicConsumeMessage(this->consumer,env,1000))
			{
        AmqpClient::BasicMessage::ptr_t request = env->Message();
        mess = request->Body();
				return true;
			}		
			return false;
		}
		catch(std::exception& r_e)
		{
				//CSystemLog info("connect rabbitmq-server error",r_e.what());
				err_log("connect rabbitmq-server error:%s",r_e.what());
		}
	}

	virtual void RabbitMqSend(const char *mess,const int len,int ntype=0){
		std::string Body(mess,len);
		try
		{
			if(ntype==1)
				channel->BasicPublish(REPORTEX, " ",AmqpClient::BasicMessage::Create(Body));
			else if(ntype==2)
				channel->BasicPublish(RULEEX, " ",AmqpClient::BasicMessage::Create(Body));			
		}
		catch(std::exception& r_e)
		{
				//CSystemLog info("connect rabbitmq-server error",r_e.what());
				err_log("connect rabbitmq-server error:%s",r_e.what());
		}
	}
	void UpdateFlowStatus(int status,int id);
	void InitCustom_Id_Ip();
	void GetAllofIP(std::string ip,const int customid);
	void GetNetsegmentIp(std::string ip,const int customid);
//private:
	std::string m_queueName;
	std::string m_sendQueue;
	std::string m_recvQueue;
	std::string consumer;
};



class CCollectLogMq : CRabbitMqClient
{
public:
	CCollectLogMq(AmqpClient::Channel::ptr_t channel):m_channel(channel)
	{
			try
			{
				m_channel->DeclareExchange(strMqExchange,
                AmqpClient::Channel::EXCHANGE_TYPE_FANOUT);
				//m_queueName = strMqQueue;
				m_queueName = m_channel->DeclareQueue(strMqQueue, false,false, true, true);
		
				m_channel->BindQueue(m_queueName,strMqExchange);
			
				m_consumer = m_channel->BasicConsume(m_queueName.c_str());
			}
			catch(std::exception& r_e)
			{
				//CSystemLog info("Log rabbitmq Declare error",r_e.what());
				err_log("connect rabbitmq-server error:%s",r_e.what());
				exit(0);
			}
	}
	~CCollectLogMq(){};
	void LogMonitor()
	{
		//CSystemLog info
		info_log("NTG Log Receive is Start!");
		std::string res;
		while(1)
		{
		        //add by yuzf 20160613 每天00:05分时将两天前的数据插入到event_flow_temp1表中
				//InsertIntoTemp();
				EVENTLOG log;
				bool isLog = RabbitMqRecv(log);	
				if(isLog)
				{
					string res= log.Log;
					//CSystemLog recvlog
					err_log("res:[%s]\n",res.c_str());
					ProcessLog(res);
				}
		}	
	}
	
	bool RabbitMqRecv(EVENTLOG &log)
	{
			try
			{
				AmqpClient::Envelope::ptr_t env;
				if(this->m_channel->BasicConsumeMessage(this->m_consumer,env,1000)){
        	AmqpClient::BasicMessage::ptr_t request = env->Message();
        const amqp_bytes_t& r_amqpByte = request->getAmqpBody();

				if ( r_amqpByte.bytes == NULL )
				{
					printf("Ntg Log Mq Recv error");
				}
			  memset(&log,0,sizeof(EVENTLOG));
			  memcpy(&log,r_amqpByte.bytes,r_amqpByte.len);
			  char tmp[3000] = {0};
        if(ntohl(log.MsgType) == 7641)
        {
        		struct in_addr addr;
        		memcpy(&addr,&(log.clientip),8);
        		std::string ip = inet_ntoa(addr);
        		_List<std::string>::iterator iter;
						for(iter = g_FlowAssetipList.begin();iter != g_FlowAssetipList.end(); iter ++)
						{
							if(ip == (*iter))
							{
								memset(tmp,0,sizeof(tmp));
								strncpy(tmp,log.Log,sizeof(tmp));
								//CSystemLog info("push flow ip:%s",ip.c_str());
								info_log("push flow ip:%s",ip.c_str());
								//g_EventFlowTemp.push_back(tmp);
								//break;
								return true;
							}
						}
				}         
				//return true;
				}
				return false;	
			}
			catch(std::exception& r_e)
			{
					//CSystemLog info
					err_log("Ntg Log Mq Recv error:%s",r_e.what());
			}
	}
 	void FLowToDB(STEVENTFLOW *Flowdata);	
	void ProcessLog(std::string mes);
	void InsertIntoTemp();
public:
	AmqpClient::Channel::ptr_t m_channel;	
	std::string m_queueName;
	std::string m_consumer;
	std::string m_ExChange;
}; 

class CADSRuleMq : CRabbitMqClient
{
	public:
	CADSRuleMq(AmqpClient::Channel::ptr_t channel):m_channel(channel)
	{
			try
			{	
				m_channel->DeclareExchange("recvweb",
                AmqpClient::Channel::EXCHANGE_TYPE_FANOUT,false,true,false);
				m_channel->DeclareExchange("ruleExchange",
                AmqpClient::Channel::EXCHANGE_TYPE_FANOUT);
				m_queueName = m_channel->DeclareQueue(FRAILRECVQ, false,false, true, true);
		
				m_channel->BindQueue(m_queueName,"ruleExchange");
			
				m_consumer = m_channel->BasicConsume(m_queueName.c_str());
			}
			catch(std::exception& r_e)
			{
					CSystemLog info("Rule Mq Declare error",r_e.what());
					exit(0);
			}
	}
	~CADSRuleMq(){};
	void AdsStatusToDb(const char *ip,const int maxbps,const int actualbps,const float cpu,const float mem);
	void ProcessResponse(std::string mes);
	void UpdateFlowStatus(std::string dstip);
	void RuleMonitor()
	{
		CSystemLog info("info","Rule Monitor is Start!");
		while(1)
		{
				std::string res = "";
				bool bHave = RabbitMqRecv(res);
				if(bHave)
					ProcessResponse(res);	
		}	
	}
	
	bool RabbitMqRecv(std::string &mess)
	{
			try
			{
				AmqpClient::Envelope::ptr_t env;
				if(this->m_channel->BasicConsumeMessage(this->m_consumer,env,1000))
				{
        	AmqpClient::BasicMessage::ptr_t request = env->Message();
        	mess = request->Body();
        	CSystemLog info("Rule Mq Receive",mess.c_str());
					return true;
				}
				return false;	
			}
			catch(std::exception& r_e)
			{
				CSystemLog info("Rule Mq Recv error",r_e.what());	
			}
	}
	void RabbitMqSend(const char *mess,const int len){
		std::string Body(mess,len);
		try
		{
			channel->BasicPublish("recvweb", " ",AmqpClient::BasicMessage::Create(Body));
		}
		catch(std::exception& r_e)
		{
				std::string Log = "Send:";Log = Log + mess;
				CSystemLog info(Log.c_str(),r_e.what());	
		}
	}
	//void ProcessResponse(std::string mes);
public:
	AmqpClient::Channel::ptr_t m_channel;	
	std::string m_queueName;
		std::string m_consumer;
}; 
#if 0
class CAdsProcess: CRabbitMqClient
{
	public:
	CAdsProcess(AmqpClient::Channel::ptr_t channel):m_channel(channel)
	{
			try
			{
        m_channel->DeclareExchange(SENDTOCOLLECTEX,AmqpClient::Channel::EXCHANGE_TYPE_FANOUT);   
        m_channel->DeclareExchange(RULEEX,AmqpClient::Channel::EXCHANGE_TYPE_FANOUT);    
        	  
				m_queueName = m_channel->DeclareQueue("web_yatx_recv_queue", false,false, true, true);		
				m_channel->BindQueue(m_queueName,SENDTOCOLLECTEX);		
				m_consumer = m_channel->BasicConsume(m_queueName.c_str());
			}
			catch(std::exception& r_e)
			{
				CSystemLog info("ads Process Mq Declare error",r_e.what());
				exit(0);	
			}
	}
	~CAdsProcess(){};
		
	void AdsProcessMonitor()
	{
		CSystemLog info("info","ads Process Monitor is Start!");
		while(1)
		{
				std::string res = "";
				bool isRequest = RabbitMqRecv(res);
				if(isRequest)
					Processads(res);	
		}	
	}
	
	bool RabbitMqRecv(std::string &mess)
	{
			try
			{
				AmqpClient::Envelope::ptr_t env;
				if(this->m_channel->BasicConsumeMessage(this->m_consumer,env,1000))
				{
        	AmqpClient::BasicMessage::ptr_t request = env->Message();
        	mess = request->Body();
        	
        	CSystemLog info("ads Mq Recv",mess.c_str());
					return true;
				}
				return false;	
			}
			catch(std::exception& r_e)
			{
				CSystemLog info("ads Mq recv error",r_e.what());	
			}
	}	
	
	void RabbitMqSend(const char* mess,const int len){
		std::string Body(mess,len);
		try
		{
				//m_channel->BasicPublish(RULEEX, "",AmqpClient::BasicMessage::Create(Body));
		}
		catch(std::exception& r_e)
		{
				CSystemLog info("adsProcess Mq send error",r_e.what());	
				CSystemLog tinfo("Content",Body.c_str());
		}
	}
	
	void Processads(const string& str)
	{
		 ADSREQUEST *tmp = (ADSREQUEST *)str.c_str();
		 if(atoi(tmp->MsgType) == 0)
			{
				GetAdsLoad(tmp->AdsIP,tmp->Request,this);
			}
			if(atoi(tmp->MsgType) == 1)
				SendSysLog(tmp->AdsIP,tmp->Request,strlen(tmp->Request));
			if(atoi(tmp->MsgType) == 4)
			{
				ADSINFO *ads = (ADSINFO *)str.c_str();
				if(ads->Port == -1)
					g_AdsIpPortListTmp.clear();
				else if(ads->Port == -4)
				{
					//看看是不是要保存文件
					//StoreAdsInfo();
					//IsStore = true;
					//ADSMutex.Lock();
					g_AdsIpPortList.clear();
					std::copy(g_AdsIpPortListTmp.begin(),g_AdsIpPortListTmp.end(),std::back_inserter(g_AdsIpPortList));
					//ADSMutex.Unlock();
				}
				else
				{
					g_AdsIpPortListTmp.push_back(*ads);
				}
			}		
	}	
	
	private:
	AmqpClient::Channel::ptr_t m_channel;	
	std::string m_queueName;
	std::string m_consumer;	
}
#endif
class CWebBackMq : CRabbitMqClient
{
	public:
	CWebBackMq(AmqpClient::Channel::ptr_t channel):m_channel(channel)
	{
			try
			{
				m_channel->DeclareExchange("recvweb",AmqpClient::Channel::EXCHANGE_TYPE_FANOUT,false,true,false);
        //m_channel->DeclareExchange(SENDTOCOLLECTEX,AmqpClient::Channel::EXCHANGE_TYPE_FANOUT);      
				//m_queueName = m_channel->DeclareQueue("web_yatx_getadsperform_queue_name", false,false, true, true);
				m_queueName = m_channel->DeclareQueue("web_getadsperform_queue_name", false,false, true, true);
				m_channel->DeclareExchange("flow_web_keepalive_ex",AmqpClient::Channel::EXCHANGE_TYPE_FANOUT);      	
				//m_queueName = m_channel->DeclareQueue("web_revadsperform_queue_name", false,false, true, true);	
				//m_SqueueName = m_channel->DeclareQueue("recvweb", false,false, true, true);
				//m_channel->BindQueue(m_queueName,"defalut_web_bind_ex");
			
				m_consumer = m_channel->BasicConsume(m_queueName.c_str());
			}
			catch(std::exception& r_e)
			{
				CSystemLog info("Web Mq Declare error",r_e.what());
				exit(0);	
			}
	}
	~CWebBackMq(){};
	void ProcessWebRequest(std::string mess);
	void toXML(std::string strAdsip,std::string strResponse,std::string &xml);
	void WebMonitor()
	{
		CSystemLog info("info","Web Monitor is Start!");
		while(1)
		{
				std::string res = "";
				bool isRequest = RabbitMqRecv(res);
				if(isRequest)
					ProcessWebRequest(res);	
				else
					m_channel->BasicPublish("flow_web_keepalive_ex", "",AmqpClient::BasicMessage::Create("keepalive"));
				sleep(2);
		}	
	}
	
	bool RabbitMqRecv(std::string &mess)
	{
			try
			{
				AmqpClient::Envelope::ptr_t env;
				if(this->m_channel->BasicConsumeMessage(this->m_consumer,env,1000))
				{
        	AmqpClient::BasicMessage::ptr_t request = env->Message();
        	mess = request->Body();
        	
        	CSystemLog info("Web Mq Recv",mess.c_str());
					return true;
				}
				return false;	
			}
			catch(std::exception& r_e)
			{
				CSystemLog info("Web Mq recv error",r_e.what());	
			}
	}
	void RabbitMqSend(const char* mess,const int len){
		std::string Body(mess,len);
		try
		{
				//m_channel->BasicPublish(SENDTOCOLLECTEX, "",AmqpClient::BasicMessage::Create(Body));
				m_channel->BasicPublish("recvweb", "",AmqpClient::BasicMessage::Create(Body));
		}
		catch(std::exception& r_e)
		{
				CSystemLog info("Web Mq send error",r_e.what());	
				CSystemLog tinfo("Content",Body.c_str());
		}
	}
public:
	AmqpClient::Channel::ptr_t m_channel;	
	std::string m_queueName;
	std::string m_SqueueName;
	std::string m_consumer;
}; 

class CAdsReport : CRabbitMqClient
{
public:
	CAdsReport(AmqpClient::Channel::ptr_t channel):m_channel(channel)
	{
			try
			{
				m_channel->DeclareExchange("reportExchange",
                AmqpClient::Channel::EXCHANGE_TYPE_FANOUT);
				m_queueName = m_channel->DeclareQueue("adsreport",false,false, true, true);
		
				m_channel->BindQueue(m_queueName,"reportExchange");
			
				this->m_consumer = m_channel->BasicConsume(m_queueName.c_str());
			}
			catch(std::exception& r_e)
			{
				CSystemLog info("report Mq Declare error",r_e.what());	
			}
	}
	~CAdsReport(){};
	void ProcessReport(std::string &mess);
	void AttackGobalToDB(ATTACKGOBAL *gobal);
	void AttackEventToDB(ATTACKEVENT *event);
	void AdsReportMonitor()
	{
		CSystemLog info("info","ADS report Monitor is Start!");
		while(1)
		{
				std::string res = "";
				bool bReport = RabbitMqRecv(res);
				if(bReport)
					ProcessReport(res);	
		}	
	}
	
	bool RabbitMqRecv(std::string &mess)
	{
			try
			{
				AmqpClient::Envelope::ptr_t env;
				if(this->m_channel->BasicConsumeMessage(this->m_consumer,env,1000))
				{
        	AmqpClient::BasicMessage::ptr_t request = env->Message();
        	mess = request->Body();
        	printf("len:%d\n",mess.length());
        	CSystemLog info("Ads Report Mq Recv",mess.c_str());
					return true;
				}
				return false;	
			}
			catch(std::exception& r_e)
			{
				CSystemLog info("Ads Report Mq recv error",r_e.what());	
			}
	}

private:
	AmqpClient::Channel::ptr_t m_channel;	
	std::string m_queueName;
	std::string m_consumer;	
};


#endif

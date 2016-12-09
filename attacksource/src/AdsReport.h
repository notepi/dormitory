#ifndef _ADSREPORT_H_
#define _ADSREPORT_H_
/*#include<stdio.h>
#include "MqMonitor.h"

using namespace AdsMq;
typedef struct st_attack_gobal
{
	char MsgType[2];
	char FileId[20];      	 	//�����ļ�ID
	char Adsip[20];				//�ϴ���ADSip
	char StartTime[20];			//������ʼʱ��
	char EndTime[20];
	char Duration[20];			//����ʱ��
	char dstip[20];
	char Totalpkts[20];			//����������
	char Totalbits[20];			//����������
	char Sendpkts[20];
	char Sendbits[20];
	char Repliedpkts[20];
	char Repliedbits[20];
	char Dropbits[20];			//��ϴ������
	char Droppkts[20];			//��ϴ������
}ATTACKGOBAL;

typedef struct st_attack_event
{
	char MsgType[2];
	char FileId[20];      	 	//�����ļ�ID
	char AttackID[20];	 		//�����¼�ID
	char StartTime[20];			//������ʼʱ��
	char Duration[20];			//����ʱ��
	char AttackType[20];		//��������
	char Totalpkts[20];			//����������
	char Totalbits[20];			//����������
	char Percent[15];			//��������ռ�������İٷֱ�
	char Dstport[6];			//����Ŀ�Ķ˿�
}ATTACKEVENT;

class CAdsReport : AdsMq::CRabbitMqClient
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
		while(1)
		{
				std::string res = "";
				RabbitMqRecv(res);
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
*/
void *ReportThread(void *);
#endif

#ifndef _DATA_DEFINE_H_
#define _DATA_DEFINE_H_

#include <string>
#include <list>
using namespace std;

#define LOGSENDEX "logCenterExchangeName"
#define FRAILRECVQ "YatxAdsQueue"
#define SENDTOCOLLECTEX "collectex_1"
#define FRAILRECVEX "logExchange_1"
#define RULEEX "ruleExchange_1"
#define REPORTEX "reportExchange_1"
#define _DEBUG_
 
typedef struct ADSRequest
{
	char AdsIP[16];
	char MsgType[2]; //0-��ȡ��ϴ������1---�·�ǣ����2---�·��ڶ�·��
	char Request[3000];	
}ADSREQUEST;

typedef struct ADSResponse
{
	char AdsIP[16];
	char MsgType[2]; //0-��ȡ��ϴ������1---�������� 2--ads��ϴ��־
	char Response[3000];	
}ADSRESPONSE;

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

typedef struct st_EventFlow
{
	unsigned long EventId;
	char	Time[20];									/*�¼�����ʱ��*/
	char	DevIP[48];									/*�豸IP*/
	char	AssetID[11];								/*�ʲ�ID*/
	char	BusRootID[11];								/*ҵ��ϵͳ���ڵ㣬�û�����*/
	char	RuleID[11];									/*������*/
	char	Level[11];									/*�ȼ�*/
	char	AnomalyID[20];								/*�쳣����ID*/					//modify by mumu 2014-04-24 AnomalyID[11];
	char	CreationTime[20];							/*�쳣��ʼʱ��*/
	char	UpdateTime[20];								/*�쳣����ʱ��*/
	char	Type[40];									/*�쳣����һ������*/
	char	SubType[40];								/*�쳣������������*/
	char	Severity[20];								/*���صȼ�*/
	char	Status[20];									/*״̬*/
	char	Direction[10];								/*��������*/
	char 	Resource[400];								/*��Դ*/
	char	Importance[20];								/*��Ҫ��*/
	char	Current[20];								/*��ǰ����*/
	char	Threshold[20];								/*ϵͳ������ֵ*/
	char	Unit[10];									/*������λ*/
	char	SrcIP_1[48];               					/*ԴIP 1*/
	char	DstIP_1[48];								/*Ŀ��IP 1*/
	char	SrcPort_1[15];								/*Դ�˿ں� 1*/
	char	DstPort_1[15];								/*Ŀ�Ķ˿ں�1*/
	char	SrcIP_2[48];								/*ԴIP 2*/
	char 	DstIP_2[48];								/*Ŀ��IP 2*/
	char 	SrcPort_2[15];								/*Դ�˿ں� 2*/
	char 	DstPort_2[15];								/*Ŀ�Ķ˿ں�2*/
	char 	SrcIP_3[48];								/*ԴIP 3*/
	char 	DstIP_3[48];								/*Ŀ��IP 3*/
	char 	Protocol[20];								/*Э���*/
	char 	URL[400];									/*��Ӧ��webҳ�����ӵ�ַ*/
	char 	DspLen[11];									/*ԭʼ��־����*/
	char 	Description[3000];								/*ԭʼ��־����*/
	char    ResourceID[12];							    /* �������*/
	char Duration[15];								/*����ʱ�� add by wanglihui*/
	char Dayscope[20];								//���ڷ�Χ add by wanglihui
	char Timescope[20];							//ʱ�䷶Χ add by wanglihui
	int SendStatus;							//����״̬ 1->�ѷ��ͣ�0->δ���� add by wanlihui
	double TotalFlow;							//������add by wanglihui
	unsigned long ID;
	int FilterID;										//ƥ�䵽��ģ��ID
	int isOngoing;										//0Ϊ���յ���anomalyid��recover��Ϣ,1-Ϊ��δ�յ�
	int DelaySeconds;									//�ӻ����͵�����
	int tStart;											//�յ���ʱ������
  
 //abr ��־		
		char impact[30];													//����������ֵ bylinlj
			double flow_max1;   //pps��ֵ
				double flow_max2;  //bps ��ֵ
}STEVENTFLOW;

typedef struct st_FlowLog
{
	char MsgType[2]; //0---ntg��־,1---ads������ϴ��־
	char Log[3000];	
}FLOWLOG;

typedef struct st_AdsInfo
{
	char AdsIP[16];
	char MsgType[2];	
	char Key[128];
	int Port;	
}ADSINFO;

typedef struct st_EventLog
{
    int TotalLen;
    int MsgType;    //7641 ---syslog
    unsigned long clientip;  //������
    int LogLength;
	int agentoffset; 		//add by huanglr 
	int agentidlength; 		//add by huanglr
    char Log[3000];
}EVENTLOG;

typedef struct st_IpRange
{
	int AssetID;
	std::string strStartIp;
	std::string strEndIp;
	std::string strStartNum;
	std::string strEndNum;
	std::string strDeviceIp;
}IPRANGE;

extern list<ADSINFO> g_AdsIpPortListTmp;
extern list<ADSINFO> g_AdsIpPortList;
extern std::string strMqExchange;
extern std::string strMqQueue;
#endif

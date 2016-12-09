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
	char MsgType[2]; //0-获取清洗能力，1---下发牵引，2---下发黑洞路由
	char Request[3000];	
}ADSREQUEST;

typedef struct ADSResponse
{
	char AdsIP[16];
	char MsgType[2]; //0-获取清洗能力，1---防护策略 2--ads清洗日志
	char Response[3000];	
}ADSRESPONSE;

typedef struct st_attack_gobal
{
	char MsgType[2];
	char FileId[20];      	 	//报表文件ID
	char Adsip[20];				//上传的ADSip
	char StartTime[20];			//攻击开始时间
	char EndTime[20];
	char Duration[20];			//攻击时长
	char dstip[20];
	char Totalpkts[20];			//攻击总流量
	char Totalbits[20];			//攻击总流量
	char Sendpkts[20];
	char Sendbits[20];
	char Repliedpkts[20];
	char Repliedbits[20];
	char Dropbits[20];			//清洗掉流量
	char Droppkts[20];			//清洗掉流量
}ATTACKGOBAL;

typedef struct st_attack_event
{
	char MsgType[2];
	char FileId[20];      	 	//报表文件ID
	char AttackID[20];	 		//攻击事件ID
	char StartTime[20];			//攻击开始时间
	char Duration[20];			//攻击时长
	char AttackType[20];		//攻击类型
	char Totalpkts[20];			//攻击总流量
	char Totalbits[20];			//攻击总流量
	char Percent[15];			//攻击流量占总流量的百分比
	char Dstport[6];			//攻击目的端口
}ATTACKEVENT;

typedef struct st_EventFlow
{
	unsigned long EventId;
	char	Time[20];									/*事件发生时间*/
	char	DevIP[48];									/*设备IP*/
	char	AssetID[11];								/*资产ID*/
	char	BusRootID[11];								/*业务系统根节点，用户关联*/
	char	RuleID[11];									/*规则编号*/
	char	Level[11];									/*等级*/
	char	AnomalyID[20];								/*异常流量ID*/					//modify by mumu 2014-04-24 AnomalyID[11];
	char	CreationTime[20];							/*异常开始时间*/
	char	UpdateTime[20];								/*异常更新时间*/
	char	Type[40];									/*异常流量一级分类*/
	char	SubType[40];								/*异常流量二级分类*/
	char	Severity[20];								/*严重等级*/
	char	Status[20];									/*状态*/
	char	Direction[10];								/*流量方向*/
	char 	Resource[400];								/*来源*/
	char	Importance[20];								/*重要性*/
	char	Current[20];								/*当前流量*/
	char	Threshold[20];								/*系统检测的阈值*/
	char	Unit[10];									/*流量单位*/
	char	SrcIP_1[48];               					/*源IP 1*/
	char	DstIP_1[48];								/*目的IP 1*/
	char	SrcPort_1[15];								/*源端口号 1*/
	char	DstPort_1[15];								/*目的端口号1*/
	char	SrcIP_2[48];								/*源IP 2*/
	char 	DstIP_2[48];								/*目的IP 2*/
	char 	SrcPort_2[15];								/*源端口号 2*/
	char 	DstPort_2[15];								/*目的端口号2*/
	char 	SrcIP_3[48];								/*源IP 3*/
	char 	DstIP_3[48];								/*目的IP 3*/
	char 	Protocol[20];								/*协议号*/
	char 	URL[400];									/*相应的web页面链接地址*/
	char 	DspLen[11];									/*原始日志长度*/
	char 	Description[3000];								/*原始日志描述*/
	char    ResourceID[12];							    /* 子网编号*/
	char Duration[15];								/*持续时间 add by wanglihui*/
	char Dayscope[20];								//日期范围 add by wanglihui
	char Timescope[20];							//时间范围 add by wanglihui
	int SendStatus;							//发送状态 1->已发送，0->未发送 add by wanlihui
	double TotalFlow;							//总流量add by wanglihui
	unsigned long ID;
	int FilterID;										//匹配到的模板ID
	int isOngoing;										//0为已收到该anomalyid的recover信息,1-为还未收到
	int DelaySeconds;									//延缓发送的秒数
	int tStart;											//收到的时间秒数
  
 //abr 日志		
		char impact[30];													//攻击流量峰值 bylinlj
			double flow_max1;   //pps峰值
				double flow_max2;  //bps 峰值
}STEVENTFLOW;

typedef struct st_FlowLog
{
	char MsgType[2]; //0---ntg日志,1---ads返回清洗日志
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
    unsigned long clientip;  //网络序
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

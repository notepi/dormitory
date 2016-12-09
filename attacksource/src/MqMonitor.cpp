#include<occi.h>
#include<stdlib.h>
#include<algorithm>
#include "util/Log4cplusUtil.h"
#include "MqMonitor.h"
#include "Map.h"
#include "ArborFlow.h"
#include "Global.h"
//#include "HWClean.h"
//#include "DpClean.h"
#include "CurlHttp.h"
#include "Log.h"
using namespace std;
using namespace oracle::occi;
using namespace log4cplus;
using namespace ffcsns;
void ProcessLog(std::string mes);
CArborFlow g_ArborFlowRule;
CRabbitMqClient g_Client;
CRabbitMqClient g_Client_1;
CRabbitMqClient g_Client_2;
CRabbitMqClient g_Client_3;
CRabbitMqClient g_Client_Rule;
_List<std::string> g_FlowAssetipList;
_Map<string,int> g_CustomIpIdMap;
std::string strMqExchange;
std::string strMqQueue;
multimap<string,IPRANGE> g_IpRangeMMap;
char LastDate[12+1];
void rtrim(char *str);
void ltrim(char *str);
void trim(char *str);


struct IP_INFO
{
	string NETWORKNAME;
	string PARENTNAME;	
};

struct IP_INFO SrcIP_1;
struct IP_INFO SrcIP_2;
struct IP_INFO SrcIP_3;
struct IP_INFO DstIP_1;

/********************************************
* 函  数: int Info_IP(const char *pSoc_ip, struct IP_INFO &Body_Info)
* 功  能: 从数据库查询ip对应的城域网，省域网
* 参  数: pSoc_ip:源ip,Body_Info: 查询后信息存放地址
* 返回值: 0--失败 ; 1--成功
**********************************************/
int Info_IP(const char *pSoc_ip, struct IP_INFO &Body_Info)
{
	oracle::occi::Environment *env;
	oracle::occi::Connection *conn;
	oracle::occi::Statement *stmt = NULL;
	oracle::occi::ResultSet *rs = NULL;
	
	string username = "gjsy";			// 用户名
	string passwd = "GJsy@ffcs1234";	// 密码
	string connstring = "orcl";			// 数据库连接字符串
	
	string sql;
	int ret, flags=0;
	struct in_addr Des_inp;				// 存放转换后ip
	struct in_addr Soc_inp;
	
	string STARTADDRESS;				// 存放原始字符串ip
	string ENDADDRESS;

	Body_Info.NETWORKNAME = "";			// 参数默认设置为空
	Body_Info.PARENTNAME = "";			// 参数默认设置为空
	
	inet_aton(pSoc_ip, &Soc_inp);										// 源IP转换成net数据
	cout << Soc_inp.s_addr << endl; 	
	
	try
	{
		env = oracle::occi::Environment::createEnvironment();			// 创建环境变量
		conn = env->createConnection(username, passwd, connstring);		// 创建数据库连接对象
		stmt = conn->createStatement();									// 创statement对象
		cout<<"connect success\n"<<endl;
		sql = "SELECT *FROM IP_INFO_TEST";
	
		stmt->setSQL(sql);												// 准备SQL语句
	
		//	executeQuery executeQurey
		rs = stmt->executeQuery();										// 执行SQL语句，返回结果集
		cout<<"exec "<<sql<<" success\n"<<endl;
		
		 while (rs->next()) {											// 未读到数据
		 	cout << "enter the loop! "<<endl;							// 取数据
		 	
		 	if(rs->isNull(4)){										
				continue;
		 	}
	 		STARTADDRESS = rs->getString(4);			
			ret = inet_aton(STARTADDRESS.c_str(), &Des_inp);
			if(!ret){
				continue;
			}
			if(Soc_inp.s_addr < Des_inp.s_addr){						// 判断ip是否小于最小ip
				continue;
			}
			
			if(rs->isNull(5)){	
				continue;									
		 	}

	 		ENDADDRESS = rs->getString(5);
			ret = inet_aton(STARTADDRESS.c_str(), &Des_inp);
			if(!ret){
				continue;
			}
			if(Soc_inp.s_addr > Des_inp.s_addr){						// 判断ip是否大于最大ip
				continue;
			}
			
		 	if(rs->isNull(2)){
				Body_Info.NETWORKNAME = "";
		 	}
		 	else {
		 		rs->setCharSet(2, "UTF8");
				Body_Info.NETWORKNAME = rs->getString(2);
		 	}

			
			if(rs->isNull(3)){
				Body_Info.PARENTNAME = "";
		 	}
		 	else {
			 	rs->setCharSet(3, "UTF8");
				Body_Info.PARENTNAME = rs->getString(3);
		 	}

			
			flags = 1;

			cout<<STARTADDRESS<<" "<<ENDADDRESS <<" "<<Body_Info.NETWORKNAME<<" "<<Body_Info.PARENTNAME << endl;
			break;
		 }
		cout << "SELECT SUCCESS" << endl;
		
	}catch (oracle::occi::SQLException  ex)								// 异常处理
	{
		cout << " Error Number " << ex.getErrorCode() << endl;
		cout << ex.getMessage() << endl;
	}	


	conn->terminateStatement(stmt);							// 终止Statement对象
	env->terminateConnection(conn);							// 断开数据库连接
	oracle::occi::Environment::terminateEnvironment(env);	// 终止环境变量
	
	cout << "helloworld!" << endl;
	return flags;
}






/*****************************************************
 * 锟斤拷锟斤拷: MqThread_(void *)
 *
 * 锟斤拷锟斤拷: Mq锟斤拷锟斤拷, 1.锟斤拷锟斤拷锟斤拷锟矫伙拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷   2.NTG锟斤拷锟斤拷ADS锟斤拷洗锟斤拷志   3.锟斤拷洗锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷 4.锟斤拷web通讯
 *
 * 锟斤拷锟斤拷: (void *)
 *
 * 锟斤拷锟斤拷值: 锟斤拷
 *****************************************************/
void* MqThread_(void *)
{
	CCollectLogMq *log = new CCollectLogMq(g_Client.channel);
	log->LogMonitor();
}
int ParseGennie(std::string mes,STEVENTFLOW &PrivateEventFlow)
{
	char *unit = NULL;
	    char *p = NULL;
		    char temp[100];
			    int iRet = 0; 
				    char ErrLog[500];
					    char value[400];
						    char Data[3000] = {0}; 
							    strncpy(Data,mes.c_str(),mes.length());

	memset(value,0 ,sizeof(value));
	if( GetStrValue("Anomaly ID",value,Data) == 0 )
		strncpy(PrivateEventFlow.AnomalyID,value,sizeof(PrivateEventFlow.AnomalyID)-1);

	memset(value,0 ,sizeof(value));
	if( GetStrValue("Creation Time",value,Data) == 0 )
	{
		memset(temp,0,sizeof(temp));
		iRet = ChangeDateTimeFormat(value,temp);
		if(iRet != 0)
		{
			printf("TimeFormat ERROR!\n");
			return 1;
		}
		strncpy(PrivateEventFlow.CreationTime,temp,sizeof(PrivateEventFlow.CreationTime)-1);
	}
							
	memset(value,0 ,sizeof(value));
	if( GetStrValue("Update Time",value,Data) == 0 )
	{
		memset(temp,0,sizeof(temp));
		iRet = ChangeDateTimeFormat(value,temp);
		if(iRet != 0)
		{
			printf("TimeFormat ERROR!\n");
			return 1;
		}
		strncpy(PrivateEventFlow.UpdateTime,temp,sizeof(PrivateEventFlow.UpdateTime)-1);
	}
	
	memset(value,0 ,sizeof(value));
	if( GetStrValue("Type",value,Data) == 0 )
		strncpy(PrivateEventFlow.Type,value,sizeof(PrivateEventFlow.Type)-1);
	
	memset(value,0 ,sizeof(value));
	if( GetStrValue("Sub-type",value,Data) == 0 )
		strncpy(PrivateEventFlow.SubType,value,sizeof(PrivateEventFlow.SubType)-1);
	
	memset(value,0 ,sizeof(value));
	if( GetStrValue("Severity",value,Data) == 0 )
		strncpy(PrivateEventFlow.Severity,value,sizeof(PrivateEventFlow.Severity)-1);
	if((strcmp(PrivateEventFlow.Severity,"Red") == 0) ||(strcmp(PrivateEventFlow.Severity,"red") == 0))
	{
		memset(PrivateEventFlow.Severity,0,sizeof(PrivateEventFlow.Severity));
		strcpy(PrivateEventFlow.Severity,"5");
	}
	else
	{
		memset(PrivateEventFlow.Severity,0,sizeof(PrivateEventFlow.Severity));
		strcpy(PrivateEventFlow.Severity,"3");
	}
				
	memset(value,0 ,sizeof(value));
	if( GetStrValue("Status",value,Data) == 0 )
		strncpy(PrivateEventFlow.Status,value,sizeof(PrivateEventFlow.Status)-1);
		
	memset(value,0 ,sizeof(value));
	if( GetStrValue("Resource",value,Data) == 0 )
		strncpy(PrivateEventFlow.Resource,value,sizeof(PrivateEventFlow.Resource)-1);

	memset(value,0 ,sizeof(value));
	if( GetStrValue("Direction",value,Data) == 0 )
		strncpy(PrivateEventFlow.Direction,value,sizeof(PrivateEventFlow.Direction)-1);

	memset(value,0 ,sizeof(value));
	if( GetStrValue("Resource ID",value,Data) == 0 )
		strncpy(PrivateEventFlow.ResourceID,value,sizeof(PrivateEventFlow.ResourceID)-1);

	memset(value,0 ,sizeof(value));
	if( GetStrValue("Importance",value,Data) == 0 )
		strncpy(PrivateEventFlow.Importance,value,sizeof(PrivateEventFlow.Importance)-1);

	memset(value,0 ,sizeof(value));
	if( GetStrValue("Triggered Value",value,Data) == 0 ){
	    /*modify by yuzf triggered value值形式为 数字 over*/
	//	if ((p=strrchr(value,' '))!=NULL)
	//	    *p='\0';
     //   trim(value);
		strncpy(PrivateEventFlow.Current,value,sizeof(PrivateEventFlow.Current)-1);
    }

	memset(value,0 ,sizeof(value));
	if( GetStrValue("Threshold",value,Data) == 0 )
		strncpy(PrivateEventFlow.Threshold,value,sizeof(PrivateEventFlow.Threshold)-1);

	memset(value,0 ,sizeof(value));
	if( GetStrValue("Unit",value,Data) == 0 )
		strncpy(PrivateEventFlow.Unit,value,sizeof(PrivateEventFlow.Unit)-1);
    //目标IP先取Anomaly Host IP,再取DIP1，均有值，以DIP1为基准值
	memset(value,0 ,sizeof(value));
	if( GetStrValue("Anomaly Host IP",value,Data) == 0 )
		strncpy(PrivateEventFlow.DstIP_1,value,sizeof(PrivateEventFlow.DstIP_1)-1);
	memset(value,0 ,sizeof(value));
	if( GetStrValue("DIP1",value,Data) == 0 )
		strncpy(PrivateEventFlow.DstIP_1,value,sizeof(PrivateEventFlow.DstIP_1)-1);

	memset(value,0 ,sizeof(value));
	if( GetStrValue("DIP2",value,Data) == 0 )
		strncpy(PrivateEventFlow.DstIP_2,value,sizeof(PrivateEventFlow.DstIP_2)-1);

	memset(value,0 ,sizeof(value));
	if( GetStrValue("DIP3",value,Data) == 0 )
		strncpy(PrivateEventFlow.DstIP_3,value,sizeof(PrivateEventFlow.DstIP_3)-1);
		
	memset(value,0 ,sizeof(value));
	if( GetStrValue("DPort1",value,Data) == 0 )
		strncpy(PrivateEventFlow.DstPort_1,value,sizeof(PrivateEventFlow.DstPort_1)-1);

	memset(value,0 ,sizeof(value));
	if( GetStrValue("DPort2",value,Data) == 0 )
		strncpy(PrivateEventFlow.DstPort_2,value,sizeof(PrivateEventFlow.DstPort_2)-1);

	memset(value,0 ,sizeof(value));
	if( GetStrValue("SIP1",value,Data) == 0 )
		strncpy(PrivateEventFlow.SrcIP_1,value,sizeof(PrivateEventFlow.SrcIP_1)-1);

	memset(value,0 ,sizeof(value));
	if( GetStrValue("SIP2",value,Data) == 0 )
		strncpy(PrivateEventFlow.SrcIP_2,value,sizeof(PrivateEventFlow.SrcIP_2)-1);

	memset(value,0 ,sizeof(value));
	if( GetStrValue("SIP3",value,Data) == 0 )
		strncpy(PrivateEventFlow.SrcIP_3,value,sizeof(PrivateEventFlow.SrcIP_3)-1);

	memset(value,0 ,sizeof(value));
	if( GetStrValue("SPort1",value,Data) == 0 )
		strncpy(PrivateEventFlow.SrcPort_1,value,sizeof(PrivateEventFlow.SrcPort_1)-1);

	memset(value,0 ,sizeof(value));
	if( GetStrValue("SPort2",value,Data) == 0 )
		strncpy(PrivateEventFlow.SrcPort_2,value,sizeof(PrivateEventFlow.SrcPort_2)-1);

	memset(value,0 ,sizeof(value));
	if( GetStrValue("Protocol",value,Data) == 0 )
		strncpy(PrivateEventFlow.Protocol,value,sizeof(PrivateEventFlow.Protocol)-1);

	memset(value,0 ,sizeof(value));
	if( GetStrValue("URL to Link the Report",value,Data) == 0 )
		strncpy(PrivateEventFlow.URL,value,sizeof(PrivateEventFlow.URL)-1);

													
	strncpy(PrivateEventFlow.Description,Data,sizeof(PrivateEventFlow.Description) - 1);
	if(strlen(PrivateEventFlow.AnomalyID) == 0)
		return 1 ;
		
	double event_Flowcurrent = 0;
	p = NULL;
	p = strchr(PrivateEventFlow.Current,' ');
	if(p != NULL)
	{
		event_Flowcurrent = strtod(PrivateEventFlow.Current,&unit);
		switch(unit[1])
		{      
			   case 'k':
			case 'K': event_Flowcurrent = event_Flowcurrent * 1024; break;
				   case 'm':
			case 'M': event_Flowcurrent = event_Flowcurrent * 1024 * 1024; break;
				   case 'g':
			case 'G': event_Flowcurrent = event_Flowcurrent * 1024 * 1024 * 1024; break;
			default: event_Flowcurrent = atof(PrivateEventFlow.Current);break;
		}
	}
	else
	{
		event_Flowcurrent = atof(PrivateEventFlow.Current);
	}
						
	snprintf(PrivateEventFlow.Current,sizeof(PrivateEventFlow.Current) - 1,"%f\0\n",event_Flowcurrent);
	/*pthread_mutex_lock(&g_LogFileMutex);
	FlowEventID = ++(g_EventInfo.FlowEventID);
	pthread_mutex_unlock(&g_LogFileMutex);
	PrivateEventFlow.EventId = FlowEventID;*/

	double a = 0;
	a = CountTime(PrivateEventFlow.CreationTime,PrivateEventFlow.UpdateTime);
	char duration[15];
	memset(duration,0,sizeof(duration));
	snprintf(duration,sizeof(duration),"%f",a);
	strncpy(PrivateEventFlow.Duration,duration,sizeof(duration) - 1);
	
	
	/*查询ip对应的地域*/
	Info_IP(PrivateEventFlow.SrcIP_1, SrcIP_1);
	Info_IP(PrivateEventFlow.SrcIP_2, SrcIP_2);
	Info_IP(PrivateEventFlow.SrcIP_3, SrcIP_3);
	Info_IP(PrivateEventFlow.DstIP_1, DstIP_1); 
	
	return 0;
}

/*****************************************************
 * 锟斤拷锟斤拷: ProcessLog
 *
 * 锟斤拷锟斤拷: 锟斤拷锟斤拷锟斤拷志锟斤拷锟斤拷锟斤拷
 *
 * 锟斤拷锟斤拷: (std::string mes)NTG锟斤拷志
 *
 * 锟斤拷锟斤拷值: 锟斤拷
 *****************************************************/

void CCollectLogMq::ProcessLog(std::string mes)
{
	//unsigned long	FlowEventID = 0;	/*锟斤拷锟斤拷锟斤拷锟斤拷EventID*/		
	STEVENTFLOW 	PrivateEventFlow;
	memset(&PrivateEventFlow,0,sizeof(PrivateEventFlow));
    
	bool Parseflag = g_ArborFlowRule.resolveSource(true,PrivateEventFlow,const_cast<char*>(mes.c_str()));
	if(!Parseflag)
	{
		info_log("recv genie log");
	    if(  ParseGennie(mes,PrivateEventFlow) != 0 )
		{
			return ;
		}
		
	}	
#ifdef _DEBUG_
		/*CSystemLog AnormalyID("Parse AnormalyID:",PrivateEventFlow.AnomalyID);
		CSystemLog CreationTime("Creation Time:",PrivateEventFlow.CreationTime);
		CSystemLog Type("Parse Type:",PrivateEventFlow.Type);
		CSystemLog SubType("Parse Sub-type:",PrivateEventFlow.SubType);
		CSystemLog Resource("Parse Resource:",PrivateEventFlow.Resource);
		CSystemLog Current("Parse Current:",PrivateEventFlow.Current);
		CSystemLog Unit("Parse Unit:",PrivateEventFlow.Unit);
		CSystemLog DstIP_1("Parse DstIP_1:",PrivateEventFlow.DstIP_1);
		CSystemLog DstPort_1("Parse DstPort_1:",PrivateEventFlow.DstPort_1);
		CSystemLog SrcIP_1("Parse SrcIP_1:",PrivateEventFlow.SrcIP_1);
		CSystemLog SrcPort_1("Parse SrcPort_1:",PrivateEventFlow.SrcPort_1);
		CSystemLog Protocol("Parse Protocol:",PrivateEventFlow.Protocol);
		CSystemLog Description("Parse Description:",PrivateEventFlow.Description);
		CSystemLog sumCurrent("SUM Current:",PrivateEventFlow.Current);*/
		info_log("Parse AnormalyID:%s",PrivateEventFlow.AnomalyID);
		info_log("Creation Time:%s",PrivateEventFlow.CreationTime);
		info_log("Parse Type:%s",PrivateEventFlow.Type);
		info_log("Parse Sub-type:%s",PrivateEventFlow.SubType);
		info_log("Parse Resource:%s",PrivateEventFlow.Resource);
		info_log("Parse Current:%s",PrivateEventFlow.Current);
		info_log("Parse Unit:%s",PrivateEventFlow.Unit);
		info_log("Parse DstIP_1:%s",PrivateEventFlow.DstIP_1);
		info_log("Parse DstPort_1:%s",PrivateEventFlow.DstPort_1);
		info_log("Parse SrcIP_1:%s",PrivateEventFlow.SrcIP_1);
		info_log("Parse SrcPort_1:%s",PrivateEventFlow.SrcPort_1);
		info_log("Parse Protocol:%s",PrivateEventFlow.Protocol);
		info_log("Parse Description:%s",PrivateEventFlow.Description);
		info_log("SUM Current:%s",PrivateEventFlow.Current);
		info_log("anormalyid:[%s],flow_max:[%f]",PrivateEventFlow.AnomalyID,PrivateEventFlow.flow_max1);
#endif	
	FLowToDB(&PrivateEventFlow);
	return;
}


//add by hsx 锟斤拷取 BW锟斤拷LowCFT锟斤拷MidCFT 锟斤拷HightCFT	
void GetFlowParameter(int nCropId,int nPlaceId,double &dLowcft,double &dMidcft,double &dHighcft)
{
	CDataBase *one = CDataBase::GetInstance();  
	float fBw(0.0f);
	string sqlStmt="select placeid,bw,lowcft,midcft,highcft from corp_info  where corpid= :1";
	ResultSet *rset = NULL;
	Statement *stmt = NULL;
	try
	{
		stmt = (one->g_Conn)->createStatement(sqlStmt);	
		stmt->setInt(1,nCropId);
		rset = stmt->executeQuery(); 
	  while (rset->next())
		{
			nPlaceId = rset->getFloat(1);
			fBw = rset->getFloat(2);
			dLowcft= rset->getFloat(3)*fBw;//*1024*1024;
			dMidcft = rset->getFloat(4)*fBw;//*1024*1024;
			dHighcft= rset->getFloat(5)*fBw;//*1024*1024;
		}
		stmt->closeResultSet(rset);
	 (one->g_Conn)->terminateStatement(stmt); 
	}
	catch(SQLException ex)
	{
		(one->g_Conn)->terminateStatement (stmt);
    cout << "Error number: " << ex.getErrorCode() << endl;
    cout << (ex.getMessage()).c_str() << endl;
    return;
	}
	
}

/*****************************************************
 * 锟斤拷锟斤拷: FLowToDB
 *
 * 锟斤拷锟斤拷: 锟斤拷锟斤拷锟铰硷拷锟斤拷EVENT_FLOW锟斤拷
 *
 * 锟斤拷锟斤拷: (STTCPFLOW Flowdata,unsigned int FlowEventID)
 *
 * 锟斤拷锟斤拷值: 锟斤拷
 *****************************************************/
#if 1
void CCollectLogMq::FLowToDB(STEVENTFLOW *Flowdata)
{
	int iCustomID(0);
	int nPlaceID(-1);
	int nSeverity(1);
	//InitCustom_Id_Ip();
	char buf[8];
	double flowCurrent = 0;    
	/*modify by yuzf 20160613 for soc_flow begin*/
	#if 0
	_Map<string,int>::iterator findIter;
	/*for(findIter = g_CustomIpIdMap.begin();findIter != g_CustomIpIdMap.end();++findIter)
	{    
		//CSystemLog info2("test",findIter->first.c_str());
		//debug_log("ip-customid,[%s],[%d]",findIter->first.c_str(),findIter->second);
	} */   
	_Map<string,int>::iterator Iter;
	Iter = g_CustomIpIdMap.find(Flowdata->DstIP_1);
	if(Iter != g_CustomIpIdMap.end())		
	{
		iCustomID = Iter->second;
		debug_log("[%s] get customID:[%d]",Flowdata->DstIP_1,iCustomID);
	}
	if(iCustomID==0)
	{
		debug_log("[%s]do not find customID",Flowdata->DstIP_1);
		return;
	}
/*	_Map<string,int>::iterator findIter;
	for(findIter = g_CustomIpIdMap.begin();findIter != g_CustomIpIdMap.end();++findIter)
	{
		 //CSystemLog info2("test",findIter->first.c_str());
		 myLog(INFO_LOG_LEVEL,g_p_defaultLog,"[%s],[%d]",findIter->first.c_str(),findIter->second);
	}*/
	double dLowcft(0),dMidcft(0),dHighcft(0);
	GetFlowParameter(iCustomID,nPlaceID,dLowcft,dMidcft,dHighcft);
		//modify by hsx 2015-5-27
	if(Flowdata->flow_max1 > 0)
  		flowCurrent = Flowdata->flow_max1;
	else
		flowCurrent = atof(Flowdata->Current);
  info_log("flow current max:[%f][%s-%f],threshold:[%f],[%f],[%f]",Flowdata->flow_max1,Flowdata->Current,flowCurrent,dLowcft,dMidcft,dHighcft);
  if(flowCurrent < dLowcft)
  	nSeverity =1;
  else if(flowCurrent >= dLowcft && flowCurrent <dMidcft)
  	nSeverity =2;
  else if(flowCurrent >= dMidcft &&  flowCurrent < dHighcft)
  	nSeverity = 3;
  else if(flowCurrent >=dHighcft)
  	nSeverity = 4;
  if(nSeverity==1)
  	return;
   #endif
  /*modify by yuzf 20160613 for soc_flow end*/
  //sprintf(buf,"%d",nSeverity);    
  sprintf(buf,"1");    
  CDataBase *one = CDataBase::GetInstance();
	Statement *stmt = NULL;
	string sqlStmt = "insert into EVENT_FLOW (EVENTID,ANOMALYID, CREATIONTIME, UPDATETIME, FLOW_TYPE, FLOW_SUBTYPE, SEVERITY,STATUS,DIRECTION,FLOW_RESOURCE,IMPORTANCE,"
				" FLOW_CURRENT,THRESHOLD,UNIT,SRCIP_1,DSTIP_1,SRCPORT_1,DSTPORT_1,SRCIP_2,SRCPORT_2,SRCIP_3,PROTOCOL,URL,CORSTATUS,DESCRIPTION,Duration,TOTAL_FLOW,"
				"SRCIP1_PARENTNAME, SRCIP1_NETWORKNAME, SRCIP2_PARENTNAME, SRCIP2_NETWORKNAME, SRCIP3_PARENTNAME, SRCIP3_NETWORKNAME, DSTIP1_PARENTNAME, DSTIP1_NETWORKNAME)"
				"srcip1_parentname, srcip1_networkname)"
        "values (seq_event_flow.nextval,:1, to_date(:2, 'yyyy/mm/dd hh24:mi:ss'), to_date(:3, 'yyyy/mm/dd hh24:mi:ss'), :4, :5, :6, :7, :8, :9, :10, :11, :12, :13, :14, :15, :16, :17, :18, :19, :20, :21, :22, 0, :23, :24,:25"
        ",:26,:27,:28,:29,:30,:31,:32,:33)";//原始25,新增8个参数
    int AssetCount = 0;
    char ErrLog[500];
	  char tempbuf[3000] = {0};
    memset(ErrLog , 0 ,500);
    char ct[20];
    char ut[20];
    memset(ct,0,sizeof(ct));
    memset(ut,0,sizeof(ut));
    char ct2[20];
    char ut2[20];
    memset(ct2,0,sizeof(ct2));
    memset(ut2,0,sizeof(ut2));
    long int Duration = 0;
    double totalFlow = 0;    
	try
	{
		
		stmt = (one->g_Conn)->createStatement(sqlStmt);		
		stmt->setNumber(1,atoi(Flowdata->AnomalyID));
    strncpy(ct,Flowdata->CreationTime,sizeof(ct));
    strncpy(ut,Flowdata->UpdateTime,sizeof(ut));
    if(ct[0] == ' ')
         strcpy(ct,&ct[1]);
    if(ut[0] == ' ')
         strcpy(ut,&ut[1]);
    TimeConver(ct, ct2, sizeof(ct2));
    ct2[19] = '\0';
    TimeConver(ut, ut2, sizeof(ut2));
    ut2[19] = '\0';
    stmt->setString(2, (char *)ct); 
    stmt->setString(3, (char *)ut); 
                 
		stmt->setCharSet(4,"UTF8");
		memset(tempbuf , 0 ,sizeof(tempbuf)); 
    code_convert("GB18030","UTF-8",Flowdata->Type, tempbuf);	
		stmt->setString(4, (char *)tempbuf);
		stmt->setCharSet(5,"UTF8");	
		memset(tempbuf , 0 ,sizeof(tempbuf)); 
    code_convert("GB18030","UTF-8",Flowdata->SubType, tempbuf);		
		stmt->setString(5, (char *)tempbuf);
		
		stmt->setString(6, buf);
		
		stmt->setString(7, (char *)Flowdata->Status);
		stmt->setString(8, (char *)Flowdata->Direction);
		
		memset(tempbuf , 0 ,sizeof(tempbuf)); 
		stmt->setCharSet(9,"UTF8");	
                
		/*if( code_convert("GB18030","UTF-8",Flowdata->Resource, tempbuf) == 0 )
		{		    
		  printf("----[tempbuf: %s]----\n",tempbuf);
			stmt->setString(9, (char *)tempbuf);
		}
		else
		{*/
     // printf("Flowdata->Resource:%s\n",Flowdata->Resource);
			stmt->setString(9, (char *)Flowdata->Resource);
		//}
               
		stmt->setString(10, (char *)Flowdata->Importance);
   
        //直接取current yb yuzf
		//stmt->setDouble(11,flowCurrent);
		stmt->setDouble(11,atof(Flowdata->Current));
		stmt->setString(12, (char *)Flowdata->Threshold);
		stmt->setString(13, (char *)Flowdata->Unit);
		stmt->setString(14, (char *)Flowdata->SrcIP_1);		
 		stmt->setString(15, (char *)Flowdata->DstIP_1);		
		stmt->setInt(16, atoi(Flowdata->SrcPort_1));
		stmt->setInt(17, atoi(Flowdata->DstPort_1));
 		stmt->setString(18, (char *)Flowdata->SrcIP_2);	
		stmt->setInt(19, atoi(Flowdata->SrcPort_2));
 		stmt->setString(20, (char *)Flowdata->SrcIP_3);	
 		stmt->setString(21, (char *)Flowdata->Protocol);
 		stmt->setString(22, (char *)Flowdata->URL);	
		//stmt->setInt(23, seq_event_flow.nextval);
		//printf("[FlowEventID:%d]\n",FlowEventID);

		memset(tempbuf , 0 ,sizeof(tempbuf)); 
    code_convert("GB18030","UTF-8",Flowdata->Description, tempbuf);	
        
		stmt->setCharSet(23,"UTF8");
		stmt->setString(23,Flowdata->Description);
		//stmt->setString(23, (char *)tempbuf);	
		//stmt->setInt(25, atoi(Flowdata->ResourceID));
		/*need change*/
		//stmt->setNumber(24, 1);
    Duration = 0;
    Duration = CountTime(Flowdata->CreationTime, Flowdata->UpdateTime);
    memset(tempbuf , 0 ,sizeof(tempbuf));   
    stmt->setNumber(24,Duration);
    totalFlow = flowCurrent * Duration;
    stmt->setDouble(25,totalFlow); 
    //stmt->setNumber(26,iCustomID);
		//stmt->setNumber(27,nPlaceID);
		
	// 添加新参数	
	stmt->setCharSet(26,"UTF8");
	stmt->setString(26,SrcIP_1.NETWORKNAME);
	stmt->setCharSet(27,"UTF8");
	stmt->setString(27,SrcIP_1.PARENTNAME);
	stmt->setCharSet(28,"UTF8");
	stmt->setString(28,SrcIP_2.NETWORKNAME);
	stmt->setCharSet(29,"UTF8");
	stmt->setString(29,SrcIP_2.PARENTNAME);	
	stmt->setCharSet(30,"UTF8");
	stmt->setString(30,SrcIP_3.NETWORKNAME);
	stmt->setCharSet(31,"UTF8");
	stmt->setString(31,SrcIP_3.PARENTNAME);		
	stmt->setCharSet(32,"UTF8");
	stmt->setString(32,DstIP_1.NETWORKNAME);
	stmt->setCharSet(33,"UTF8");
	stmt->setString(33,DstIP_1.PARENTNAME);	
		
		stmt->executeUpdate(); 
		(one->g_Conn)->terminateStatement(stmt);
    (one->g_Conn)->commit();	
	}
	catch(SQLException ex)
	{
		(one->g_Conn)->terminateStatement (stmt);
    cout << "Error number: " << ex.getErrorCode() << endl;
    cout << (ex.getMessage()).c_str() << endl;
		snprintf(ErrLog, sizeof(ErrLog), "Exception thrown insert into EVENT_FLOW fail at %s:%d, Error number is %d,Error is %s\n", __FILE__, __LINE__,ex.getErrorCode(),(ex.getMessage()).c_str());
		ErrLog[499] = '\0';
		CSystemLog info("Oracle error",ErrLog);
		return;
	}
	CSystemLog info("To DB END!",Flowdata->AnomalyID);
#if 0
	std::string strStatus = Flowdata->Status;
	transform(strStatus.begin(),strStatus.end(),strStatus.begin(), (int (*)(int))tolower);
	if(strStatus == "ongoing")
	{
		UpdateStatus(atoi(Flowdata->AnomalyID),0);
		UpdateStatus(atoi(Flowdata->AnomalyID),1);
		doCleanOrStop((char *)Flowdata->DstIP_1,1,atoi(Flowdata->AnomalyID));
	}
	else
	{
		UpdateStatus(atoi(Flowdata->AnomalyID),3);
		doCleanOrStop((char *)Flowdata->DstIP_1,0,atoi(Flowdata->AnomalyID));
	}
#endif
	return;
}
#endif

void CRabbitMqClient::InitCustom_Id_Ip()
{
		std::string sqlStmt = "select corpid,ip from flow_monitor_clean";
		Statement *stmt = NULL;
    ResultSet *rset = NULL;
		CDataBase *one = CDataBase::GetInstance();
		int iId(0);
		std::string strIp;		
		g_CustomIpIdMap.lock();
		g_CustomIpIdMap.unlock_clear();	
		try
		{
				stmt = (one->g_Conn)->createStatement(sqlStmt);
				rset = stmt->executeQuery ();
        while(rset->next())
        {
        		iId = rset->getInt(1);
        		strIp = rset->getString(2);
        		GetAllofIP(strIp,iId);
        }
				g_CustomIpIdMap.unlock();
				stmt->closeResultSet(rset);
        (one->g_Conn)->terminateStatement(stmt);	
		}
		catch(SQLException ex)
    {
       (one->g_Conn)->terminateStatement (stmt);
    		cout << "Error number: " << ex.getErrorCode() << endl;
    		cout << (ex.getMessage()).c_str() << endl;
    		char ErrLog[500] = {0};
				snprintf(ErrLog, sizeof(ErrLog), "Exception thrown select id_ip from custom_info fail at %s:%d, Error number is %d,Error is %s\n", __FILE__, __LINE__,ex.getErrorCode(),(ex.getMessage()).c_str());
				ErrLog[499] = '\0';
				CSystemLog info("Oracle error",ErrLog);     
				g_CustomIpIdMap.unlock();
				return;
    }		
}

void CRabbitMqClient::GetAllofIP(std::string ip,const int customid)
{
		std::string strIp = ip;
			
		size_t point;
		point = strIp.find(",");
		while(point != string::npos)
		{
				std::string tmp = strIp.substr(0,point);	
				GetNetsegmentIp(tmp,customid);
				strIp = strIp.substr(point + 1);
				point = strIp.find(",");
		}
		GetNetsegmentIp(strIp,customid);			
}

void CRabbitMqClient::GetNetsegmentIp(std::string ip,const int customid)
{
		//std::string strIp = ip;
		//std::string strFirst = strIp.substr(0,point);
		std::string strFirst = ip;
		if(string::npos != strFirst.find("-"))
		{
				//example 172.168.0.1-100
				size_t point = strFirst.find("-");
				std::string strPip = strFirst.substr(0,point);
				size_t rhs = strPip.rfind(".");
				int iMinip(0);
				if(rhs != string::npos)	
				{
						iMinip = atoi(strPip.substr(rhs + 1,strPip.length()).c_str());   //parse result:iMinip = 1
						strPip = strPip.substr(0,rhs);													 //parse result:strPip = 172.168.0
				}
				int iMaxip = atoi(strFirst.substr(point + 1,strFirst.length()).c_str());  //parse result:iMaxip = 100
				char tmp[20] = {0};
				_Map<string,int>::iterator iter;
				
				for(int i = iMinip;i < iMaxip + 1;i ++)
				{
					memset(tmp,0,sizeof(tmp));
					sprintf(tmp,"%s.%d",strPip.c_str(),i);
					//CSystemLog info("tmp",tmp);
					iter = g_CustomIpIdMap.unlock_find(tmp);
					if(iter == g_CustomIpIdMap.end())
						g_CustomIpIdMap.unlock_insert(make_pair(tmp,customid));
				}
		}	
		else if(string::npos != strFirst.find("/"))
		{
				//默锟斤拷为C锟斤拷锟斤拷址锟斤拷,锟斤拷然锟介烦	
				size_t lhs = strFirst.find("/");
				int netmask = 0;
				netmask = atoi(strFirst.substr(lhs + 1).c_str());
				if(netmask > 30)
				{
						_Map<string,int>::iterator iter;
						iter = g_CustomIpIdMap.unlock_find(strFirst.substr(0,lhs).c_str());
						if(iter == g_CustomIpIdMap.end())
	    				g_CustomIpIdMap.unlock_insert(make_pair(strFirst.substr(0,lhs).c_str(),customid));
	    			return ;
				}	
				
				char IPScope[256] = {0};    char tempip[60];
				strncpy(IPScope,strFirst.substr(0,lhs).c_str(),sizeof(IPScope));
				int i = 0;
				int count = 0;
				int IPFILED[4] = {0,0,0,0};
				for(i = 0 ; i < 256;i++)
				{
					if(IPScope[i] == '\0')
					{
						break;
					}
					if(IPScope[i] == '.')
					{
						IPScope[i] = ' ';
						count++;
					}
				}
				if(count != 3)
				{
					printf("no three . \n");
	    		return ;
				}
				char *TempIP = IPScope; 
				for(i = 0;i <4 ;i++)
				{
					IPFILED[i] = strtol (TempIP , &TempIP , 0);
					if( IPFILED[i] < 0 || IPFILED[i] > 255)
			  		return ;
				} 	
				int minmaks[4] = {0,0,0,0};
				int maxmaks[4] = {255,255,255,255};
				for(i = 0;i <4 ;i++)
				{
						printf("netmask/8 is[%d]\n",netmask/8);
						if(i < (netmask/8))
						{
			 				 minmaks[i] = ((int)pow(2.0,8.0)) - 1;
			 				 maxmaks[i] = 0;
						}
				}
				if( netmask%8 != 0 )
				{
		 				minmaks[netmask/8] = 255 - (((int)pow(2.0,(double)((8 - (netmask%8))))) - 1); 
		 				maxmaks[netmask/8] = (int)pow(2.0,(double)(8 - (netmask%8))) - 1;	
				}
				for(i = 0;i <4 ;i++)
				{
						printf("min max mask is [%d] [%d]\n",minmaks[i],maxmaks[i]);
				}	  
				int minip[4] = {0,0,0,0};
				int maxip[4] = {0,0,0,0};   	  
				for(i = 0;i <4 ;i++)
				{
						minip[i] = IPFILED[i] & minmaks[i];
						maxip[i] = IPFILED[i] | maxmaks[i];
						if(i == 3)
			  			minip[i] = minip[i] + 1;	  	  	    	  	  
						if(i == 3)
			 			  maxip[i] = maxip[i] - 1;	  	  	  
						printf("min max IP is [%d] [%d]\n",minip[i],maxip[i]);
				} 
				unsigned int uiStart = 0;
				unsigned int uiEnd = 0;
				char startip[32] = {0};
				char endip[32] = {0};
    		memset(tempip ,0 ,sizeof(tempip));	  
				snprintf(tempip,sizeof(tempip),"%d.%d.%d.%d",minip[0],minip[1],minip[2],minip[3]); 
				strncpy(startip,tempip,sizeof(tempip)); 
				memset(tempip ,0 ,sizeof(tempip));
				snprintf(tempip,sizeof(tempip),"%d.%d.%d.%d",maxip[0],maxip[1],maxip[2],maxip[3]);  	    
				strncpy(endip,tempip,sizeof(tempip));
    		uiStart = minip[0]*256*256*256+minip[1]*256*256+minip[2]*256+minip[3];
				uiEnd = maxip[0]*256*256*256+maxip[1]*256*256+maxip[2]*256+maxip[3];
				unsigned int ui = uiStart;
    		int ipnum[4] = {0,0,0,0};
    		//printf("tempip is [%lu] [%lu]\n",uiStart,uiEnd); 
				_Map<string,int>::iterator iter;
    		while(ui <= uiEnd)
    		{
        		memset(tempip ,0 ,sizeof(tempip));	
        		ipnum[0] = ui/(256*256*256);
        		ipnum[1] = (ui - 256*256*256*ipnum[0])/(256*256);
        		ipnum[2] = (ui - 256*256*256*ipnum[0] - 256*256*ipnum[1])/256;
        		ipnum[3] = ui - 256*256*256*ipnum[0] - 256*256*ipnum[1] - 256*ipnum[2];
	    			snprintf(tempip,sizeof(tempip),"%d.%d.%d.%d",ipnum[0] ,ipnum[1],ipnum[2],ipnum[3]); 
	    			//printf("tempip is [%s]\n",tempip); 
	    			//CSystemLog info("tempip",tempip);
	    			iter = g_CustomIpIdMap.unlock_find(tempip);
						if(iter == g_CustomIpIdMap.end())
	    				g_CustomIpIdMap.unlock_insert(make_pair(tempip,customid));
	    			ui++;        
    		}
		}
		else
		{
				//CSystemLog info("single ip",ip.c_str());
				_Map<string,int>::iterator iter;
				iter = g_CustomIpIdMap.unlock_find(ip);
				if(iter == g_CustomIpIdMap.end())
					g_CustomIpIdMap.unlock_insert(make_pair(ip.c_str(),customid));
				return;
		}
}
/*******************************************
FunctionName:InsertIntoTemp
Dsc:将event_flow两天前的数据插入到event_flow_temp1中
********************************************/
void CCollectLogMq::InsertIntoTemp()
{
    time_t time_now = time(0); 
    char tmp[12+1]; 
    char ErrLog[500];
	char tmp_HM[4+1];

    memset(tmp,0x00,sizeof(tmp));
    strftime( tmp, sizeof(tmp), "%Y%m%d%H%M",localtime(&time_now) ); 
    memset(tmp_HM,0x00,sizeof(tmp_HM));
	strcpy(tmp_HM,tmp+8);
    //info_log("time now:[%s],last_date[%s]\n",tmp_HM,LastDate);
	/*每天的00:05:00执行一次*/
	if ((strlen(LastDate)==0 || strncmp(LastDate,tmp,8)) && !strcmp(tmp_HM,"0005"))
	{
        CDataBase *one = CDataBase::GetInstance();
        Statement *stmt = NULL;
        Statement *stmt1 = NULL;
		string sqlStmt1="truncate table event_flow_temp1";
		string sqlStmt="insert into event_flow_temp1 select * from event_flow where creationtime>sysdate-2";
        
		memset(LastDate,0x00,sizeof(LastDate));
		strcpy(LastDate,tmp);

		try
		{
		    stmt1 = (one->g_Conn)->createStatement(sqlStmt1);
            stmt1->executeUpdate();
            (one->g_Conn)->terminateStatement(stmt1);
            (one->g_Conn)->commit();
            info_log("truncate temp success[%s]\n",sqlStmt1.c_str());
		    stmt = (one->g_Conn)->createStatement(sqlStmt);
            stmt->executeUpdate();
            (one->g_Conn)->terminateStatement(stmt);
            (one->g_Conn)->commit();
            info_log("insert into temp success[%s][%s]\n",sqlStmt.c_str(),LastDate);
		}
        catch(SQLException ex)
        {
           (one->g_Conn)->terminateStatement (stmt);
           cout << "Error number: " << ex.getErrorCode() << endl;
           cout << (ex.getMessage()).c_str() << endl;
           snprintf(ErrLog, sizeof(ErrLog), "Exception thrown insert into EVENT_FLOW_temp1 fail at %s:%d, Error number is %d,Error is %        s\n", __FILE__, __LINE__,ex.getErrorCode(),(ex.getMessage()).c_str());
           ErrLog[499] = '\0';
           CSystemLog info("Oracle error",ErrLog);
        }
		
	}
}
void rtrim(char *str)
{
    int len = strlen(str);
    int i;  

    for (i=len-1; i>=0; i--){
        if (str[i]!=' ' && str[i]!='\t'){
            break;  
        }       
    }
    str[i+1]=0;
}
void ltrim(char *str)
{
    int len = strlen(str);
    int i,n;

    for (i=0; i<len; i++){
        if (str[i]!=' ' && str[i]!='\t'){
            break;
        }
    }
    if (i == 0){
        return;
    }
    n = i;
    for (i=n; i<len; i++){
        str[i-n] = str[i];
    }
    str[len-n] = 0;
}
void trim(char *str)
{
    ltrim(str);
    rtrim(str);
}

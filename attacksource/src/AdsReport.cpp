#include<string>
#include<stdlib.h>
#include<math.h>
#include<iostream>
#include "AdsReport.h"
#include "Init.h"
#include "Map.h"
#include "DataDefine.h"
#include "tinyxml.h"

#define REPORTDIR ""
#define REPORTDIRNEW ""
extern _Map<string,int> g_CustomIpIdMap;
extern char ReportPath[256];
extern char ReportStorePath[256];
list<ADSINFO> g_AdsIpPortListTmp;
list<ADSINFO> g_AdsIpPortList;
static void GetNetsegmentIp(std::string ip,const int customid)
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
					CSystemLog info("tmp",tmp);
					iter = g_CustomIpIdMap.unlock_find(tmp);
					if(iter == g_CustomIpIdMap.end())
						g_CustomIpIdMap.unlock_insert(make_pair(tmp,customid));
				}
		}	
		else if(string::npos != strFirst.find("/"))
		{
				//默认为C类地址啊,不然麻烦	
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
    		printf("tempip is [%lu] [%lu]\n",uiStart,uiEnd); 
				_Map<string,int>::iterator iter;
    		while(ui <= uiEnd)
    		{
        		memset(tempip ,0 ,sizeof(tempip));	
        		ipnum[0] = ui/(256*256*256);
        		ipnum[1] = (ui - 256*256*256*ipnum[0])/(256*256);
        		ipnum[2] = (ui - 256*256*256*ipnum[0] - 256*256*ipnum[1])/256;
        		ipnum[3] = ui - 256*256*256*ipnum[0] - 256*256*ipnum[1] - 256*ipnum[2];
	    			snprintf(tempip,sizeof(tempip),"%d.%d.%d.%d",ipnum[0] ,ipnum[1],ipnum[2],ipnum[3]); 
	    			printf("tempip is [%s]\n",tempip); 
	    			CSystemLog info("tempip",tempip);
	    			iter = g_CustomIpIdMap.unlock_find(tempip);
						if(iter == g_CustomIpIdMap.end())
	    				g_CustomIpIdMap.unlock_insert(make_pair(tempip,customid));
	    			ui++;        
    		}
		}
		else
		{
				CSystemLog info("single ip",ip.c_str());
				_Map<string,int>::iterator iter;
				iter = g_CustomIpIdMap.unlock_find(ip);
				if(iter == g_CustomIpIdMap.end())
					g_CustomIpIdMap.unlock_insert(make_pair(ip.c_str(),customid));
				return;
		}
}

static void GetAllofIP(std::string ip,const int customid)
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

static void InitCustom_Id_Ip()
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




/*****************************************************
 * 函数	 : AttackGobalToDB
 *
 * 功能  : ads上传全局报表入库
 *
 * 参数  : 采集上传结构体
 *			 
 *
 * 返回值: 无
 *****************************************************/
static void AttackGobalToDB(ATTACKGOBAL *gobal)
{
		int iCustomID(0);
		InitCustom_Id_Ip();
		_Map<string,int>::iterator Iter;
		Iter = g_CustomIpIdMap.find(gobal->dstip);
		if(Iter != g_CustomIpIdMap.end())
			iCustomID = Iter->second;
		string sqlStmt = "insert into flow_attack_gobal_info(Id,Starttime,duration,endtime,dstip,totalbits,totalpkts,sendpkts,sendbits,repliedpkts,repliedbits,droppkts,dropbits,customid)values(:1, to_date(:2,'yyyy/mm/dd hh24:mi:ss'), :3, to_date(:4,'yyyy/mm/dd hh24:mi:ss'), :5, :6, :7, :8, :9, :10, :11, :12, :13, :14)";
		Statement *stmt = NULL;
		CDataBase *one = CDataBase::GetInstance();
		char ErrLog[500] = {0};
				
		try
		{
				stmt = (one->g_Conn)->createStatement(sqlStmt);
				stmt->setInt(1,atoi(gobal->FileId));
				stmt->setString(2,gobal->StartTime);
				stmt->setInt(3,atoi(gobal->Duration));
				stmt->setString(4,gobal->EndTime);
				stmt->setString(5,gobal->dstip);
				stmt->setInt(6,atoi(gobal->Totalpkts));
				stmt->setInt(7,atoi(gobal->Totalbits));
				stmt->setInt(8,atoi(gobal->Sendpkts));
				stmt->setInt(9,atoi(gobal->Sendbits));
				stmt->setInt(10,atoi(gobal->Repliedpkts));
				stmt->setInt(11,atoi(gobal->Repliedbits));
				stmt->setInt(12,atoi(gobal->Droppkts));
				stmt->setInt(13,atoi(gobal->Dropbits));
				stmt->setInt(14,iCustomID);
				
				stmt->executeUpdate ();    
				(one->g_Conn)->commit();    
        (one->g_Conn)->terminateStatement(stmt);
		}
		catch(SQLException ex)
    {
       (one->g_Conn)->terminateStatement (stmt);
    		cout << "Error number: " << ex.getErrorCode() << endl;
    		cout << (ex.getMessage()).c_str() << endl;
				snprintf(ErrLog, sizeof(ErrLog), "Exception thrown insert into flow_attack_gobal_info fail at %s:%d, Error number is %d,Error is %s\n", __FILE__, __LINE__,ex.getErrorCode(),(ex.getMessage()).c_str());
				ErrLog[499] = '\0';
				CSystemLog info("Oracle error",ErrLog);     
				return;
    }
    return;
}


/*****************************************************
 * 函数	 : AttackEventToDB
 *
 * 功能  : ads上传攻击事件报表入库
 *
 * 参数  : 采集上传结构体
 *			 
 *
 * 返回值: 无
 *****************************************************/
static void AttackEventToDB(ATTACKEVENT *event)
{
		string sqlStmt = "insert into attack_event_info(Id,Starttime,duration,attacktype,totalbits,totalpkts,percent,dstport)values(:1, to_date(:2,'yyyy/mm/dd hh24:mi:ss'), :3, :4, :5, :6, :7, :8)";
		Statement *stmt = NULL;
		CDataBase *one = CDataBase::GetInstance();
		char ErrLog[500] = {0};
				
		try
		{
				stmt = (one->g_Conn)->createStatement(sqlStmt);
				stmt->setInt(1,atoi(event->AttackID));
				stmt->setString(2,event->StartTime);
				stmt->setInt(3,atoi(event->Duration));
				stmt->setString(4,event->AttackType);
				stmt->setInt(5,atoi(event->Totalbits));
				stmt->setInt(6,atoi(event->Totalpkts));
				
				stmt->setInt(7,atof(event->Percent));
				stmt->setInt(8,atoi(event->Dstport));				
				
				stmt->executeUpdate ();     
				(one->g_Conn)->commit();   
        (one->g_Conn)->terminateStatement(stmt);
		}
		catch(SQLException ex)
    {
       (one->g_Conn)->terminateStatement (stmt);
    		cout << "Error number: " << ex.getErrorCode() << endl;
    		cout << (ex.getMessage()).c_str() << endl;
				snprintf(ErrLog, sizeof(ErrLog), "Exception thrown insert into attack_event_info fail at %s:%d, Error number is %d,Error is %s\n", __FILE__, __LINE__,ex.getErrorCode(),(ex.getMessage()).c_str());
				ErrLog[499] = '\0';
				CSystemLog info("Oracle error",ErrLog);     
				return;
    }
    
    stmt = NULL;
    sqlStmt = "insert into Flow_Attackid_Assoc(id,attackid)values(:1,:2)";
    try
    {
    		stmt = (one->g_Conn)->createStatement(sqlStmt);
    		stmt->setInt(1,atoi(event->FileId));
    		stmt->setInt(2,atoi(event->AttackID));	
    		
    		stmt->executeUpdate ();        
    		(one->g_Conn)->commit();
        (one->g_Conn)->terminateStatement(stmt);
    }
    catch(SQLException ex)
    {
       (one->g_Conn)->terminateStatement (stmt);
    		cout << "Error number: " << ex.getErrorCode() << endl;
    		cout << (ex.getMessage()).c_str() << endl;
				snprintf(ErrLog, sizeof(ErrLog), "Exception thrown insert into Flow_Attackid_Assoc fail at %s:%d, Error number is %d,Error is %s\n", __FILE__, __LINE__,ex.getErrorCode(),(ex.getMessage()).c_str());
				ErrLog[499] = '\0';
				CSystemLog info("Oracle error",ErrLog);     
				return;
    }
    return;
}

int ProcessReportFile()
{
	char FileName[260] = {0};
	char cTmp[256][256] = {0};
	int i(0);
	std::string Path = ReportPath;
	std::string StroePath = ReportStorePath;
	ATTACKGOBAL node;
	ATTACKEVENT event_;
	int iLen = trave_dir(ReportPath, 1,cTmp);
	if(iLen < 1)
		return 0;

	while(1)
	{
		memset(FileName,0,sizeof(FileName));
		memset(&node,0,sizeof(ATTACKGOBAL));
		
		strncpy(FileName,cTmp[i],strlen(cTmp[i]));
		//get_filename_from_list_dir(REPORTDIR,FileName);

		if(0 == strlen(FileName))
		{
			break;
		}
		Path = Path + FileName;
		StroePath = StroePath + FileName;
		TiXmlDocument* myDocument = new TiXmlDocument();
		myDocument->LoadFile(Path.c_str());
	
		TiXmlElement *RootElement = myDocument->RootElement();
	
		TiXmlElement *report = myDocument->FirstChildElement("report");
		if(!report)
		{
			return -1;
		}
		/***********************解析报表文件id***********************/
		TiXmlElement *id = report->FirstChildElement("id");
		if(!id)
		{
			return -1;
		}
		if(id->GetText() != NULL)
			strncpy(node.FileId,id->GetText(),sizeof(node.FileId));

		/***********************解析adsip***********************/
		TiXmlElement *adsip = report->FirstChildElement("ip");
		if(!adsip)
		{
			return -1;
		}

		/***********************解析攻击的开始时间***********************/
		TiXmlElement *attack_start_time = report->FirstChildElement("attack_start_time");
		if(!attack_start_time)
		{
			return -1;
		}
		if(attack_start_time->GetText() != NULL)
			strncpy(node.StartTime,attack_start_time->GetText(),sizeof(node.StartTime));

		/***********************解析攻击时长***********************/
		TiXmlElement *attack_duration = report->FirstChildElement("attack_duration");
		if(!attack_duration)
		{
			return -1;
		}
		if(attack_duration->GetText() != NULL)
			strncpy(node.Duration,attack_duration->GetText(),sizeof(node.Duration));

		/***********************解析攻击结束***********************/
		TiXmlElement *attack_end_time = report->FirstChildElement("attack_end_time");
		if(!attack_end_time)
		{
			return -1;
		}
		if(attack_end_time->GetText() != NULL)
			strncpy(node.EndTime,attack_end_time->GetText(),sizeof(node.EndTime));

		/***********************解析攻击目标ip***********************/
		TiXmlElement *dstip = report->FirstChildElement("dst_ip");
		if(!dstip)
		{
			return -1;
		}
		if(dstip->GetText() != NULL)
			strncpy(node.dstip,dstip->GetText(),sizeof(node.dstip));
		/***********************解析接收总流量***********************/
		TiXmlElement *total = report->FirstChildElement("total");
		if(!total)
		{
			return -1;
		}
		/***********************解析接收总流量***********************/
		TiXmlElement *receive = total->FirstChildElement("receive");
		if(!receive)
		{
			return -1;
		}
		TiXmlElement *retotal_pkts = receive->FirstChildElement("total_pkts");
		if(!retotal_pkts)
		{
			return -1;
		}
		if(retotal_pkts->GetText() != NULL)
			strncpy(node.Totalpkts,retotal_pkts->GetText(),sizeof(node.Totalpkts));
		TiXmlElement *retotal_bits = receive->FirstChildElement("total_bits");
		if(!retotal_bits)
		{
			return -1;
		}
		if(retotal_bits->GetText() != NULL)
			strncpy(node.Totalbits,retotal_bits->GetText(),sizeof(node.Totalbits));
		/***********************解析发送总流量***********************/
		TiXmlElement *send = total->FirstChildElement("send");
		if(!send)
		{
			return -1;
		}
		TiXmlElement *stotal_pkts = send->FirstChildElement("total_pkts");
		if(!stotal_pkts)
		{
			return -1;
		}
		if(stotal_pkts->GetText() != NULL)
			strncpy(node.Sendpkts,stotal_pkts->GetText(),sizeof(node.Sendpkts));
		TiXmlElement *stotal_bits = send->FirstChildElement("total_bits");
		if(!stotal_bits)
		{
			return -1;
		}
		if(stotal_bits->GetText() != NULL)
			strncpy(node.Sendbits,stotal_bits->GetText(),sizeof(node.Sendbits));
		/***********************解析回注总流量***********************/
		TiXmlElement *replied = total->FirstChildElement("replied");
		if(!replied)
		{
			return -1;
		}
		TiXmlElement *rtotal_pkts = replied->FirstChildElement("total_pkts");
		if(!rtotal_pkts)
		{
			return -1;
		}
		if(rtotal_pkts->GetText() != NULL)
			strncpy(node.Repliedpkts,rtotal_pkts->GetText(),sizeof(node.Repliedpkts));
		TiXmlElement *rtotal_bits = replied->FirstChildElement("total_bits");
		if(!rtotal_bits)
		{
			return -1;
		}
		if(rtotal_bits->GetText() != NULL)
			strncpy(node.Repliedbits,rtotal_bits->GetText(),sizeof(node.Repliedbits));
		/***********************解析丢弃总流量***********************/
		TiXmlElement *dropped = total->FirstChildElement("dropped");
		if(!dropped)
		{
			return -1;
		}
		TiXmlElement *dtotal_pkts = dropped->FirstChildElement("total_pkts");
		if(!dtotal_pkts)
		{
			return -1;
		}
		if(dtotal_pkts->GetText() != NULL)
			strncpy(node.Droppkts,dtotal_pkts->GetText(),sizeof(node.Droppkts));
		TiXmlElement *dtotal_bits = dropped->FirstChildElement("total_bits");
		if(!dtotal_bits)
		{
			return -1;
		}
		if(dtotal_bits->GetText() != NULL)
			strncpy(node.Dropbits,dtotal_bits->GetText(),sizeof(node.Dropbits));

		sprintf(node.MsgType,"%d",1);
		AttackGobalToDB(&node);
		//char tmp[300] = {0};
		//memcpy(tmp,&node,sizeof(ATTACKGOBAL));
		//g_Client_Report.RabbitMqSend(tmp,sizeof(ATTACKGOBAL),1);            //报表总情况发送
		TiXmlElement *attack_event = report->FirstChildElement("drop_attack_event");
		if(!attack_event)
		{
			return -1;
		}
		TiXmlElement *event1 = attack_event->FirstChildElement("event");
		if(!event1)
		{
			return -1;
		}
		while(event1)
		{
			memset(&event_,0,sizeof(ATTACKEVENT));
			strncpy(event_.FileId,id->GetText(),sizeof(event_.FileId));
			TiXmlElement *AttackID = event1->FirstChildElement("id");
			if(AttackID->GetText() != NULL)
				strncpy(event_.AttackID,AttackID->GetText(),sizeof(event_.AttackID));
			TiXmlElement *start_time = event1->FirstChildElement("start_time");
			if(start_time->GetText() != NULL)
				strncpy(event_.StartTime,start_time->GetText(),sizeof(event_.StartTime));
			TiXmlElement *duration = event1->FirstChildElement("duration");
			if(duration->GetText() != NULL)
				strncpy(event_.Duration,duration->GetText(),sizeof(node.Duration));
			TiXmlElement *type = event1->FirstChildElement("type");
			if(type->GetText() != NULL)
				strncpy(event_.AttackType,type->GetText(),sizeof(event_.AttackType));
			TiXmlElement *total_pkts = event1->FirstChildElement("total_pkts");
			if(total_pkts->GetText() != NULL)
				strncpy(event_.Totalpkts,total_pkts->GetText(),sizeof(event_.Totalpkts));
			TiXmlElement *total_bits = event1->FirstChildElement("total_bits");
			if(total_bits->GetText() != NULL)
				strncpy(event_.Totalbits,total_bits->GetText(),sizeof(event_.Totalbits));
			TiXmlElement *rate = event1->FirstChildElement("triggering_rate");
			if(rate->GetText() != NULL)
				strncpy(event_.Percent,rate->GetText(),sizeof(event_.Percent));
			//TiXmlElement *srcip = event1->FirstChildElement("srcip");
			TiXmlElement *dst_port = event1->FirstChildElement("dst_port");
			if(dst_port->GetText() != NULL)
				strncpy(event_.Dstport,dst_port->GetText(),sizeof(event_.Dstport));
			sprintf(event_.MsgType,"%d",2);
			//memset(tmp,0,sizeof(tmp));
			//memcpy(tmp,&event_,sizeof(ATTACKEVENT));
			//g_Client.RabbitMqSend(tmp,sizeof(ATTACKEVENT),1);     
			AttackEventToDB(&event_);
			event1 = event1->NextSiblingElement("event");
		}
		//bool bSuccess = ::MoveFile(Path.c_str(),StroePath.c_str());
		int iSuccess = rename(Path.c_str(),StroePath.c_str());
		i ++; //下个文件
	}
	return 0;
}

static void AdsStatusToDb(const char *ip,const int maxbps,const int actualbps,const float cpu,const float mem)
{
    CSystemLog info("AdsStatusToDb",ip);
    std::string sqlStmt = "SELECT adsid,adsname from ads_info where adsip = :1";
    Statement *stmt = NULL;
    ResultSet *rset = NULL;
    CDataBase *one = CDataBase::GetInstance();    
    char ErrLog[500] = {0};
    char c_AdsName[100] = {0};
    int Id = 0;
    try
    {
            stmt = (one->g_Conn)->createStatement(sqlStmt);
            stmt->setString(1,ip);
            rset = stmt->executeQuery ();
            while(rset->next())
           {
              Id = rset->getInt(1);
	      strncpy(c_AdsName,rset->getString(2).c_str(),sizeof(c_AdsName));
           }
           stmt->closeResultSet(rset);
           (one->g_Conn)->terminateStatement(stmt);
    }//end try
    catch(SQLException ex)
    {
        sprintf(ErrLog, "Exception thrown Read adsid from ads_info at %s:%d,Error number:%d:%s\n", __FILE__, __LINE__,ex.getErrorCode(),(ex.getMessage()).c_str());
        cout << "Error number: " << ex.getErrorCode() << endl;
        cout << (ex.getMessage()).c_str() << endl;
        (one->g_Conn)->terminateStatement(stmt);
        CSystemLog info("Oracle error",ErrLog);
        return;       
    }
    sprintf(ErrLog,"%d",Id);
    CSystemLog ID(c_AdsName,ErrLog);
    stmt = NULL;
    sqlStmt = "insert into ads_status_info(adsid,adsip,time,cpurate,memrate,bps,actual_bps,adsname)values(:1,:2,sysdate,:3,:4,:5,:6,:7)";
    try
    {
    		stmt = (one->g_Conn)->createStatement(sqlStmt);
    		stmt->setInt(1,Id);
    		stmt->setString(2,ip);
    		stmt->setFloat(3,cpu);
    		stmt->setFloat(4,mem);
    		stmt->setNumber(5,maxbps);
    		stmt->setNumber(6,actualbps);
				stmt->setString(7,c_AdsName);
    		
    		stmt->executeUpdate ();  
    		(one->g_Conn)->commit();      
        (one->g_Conn)->terminateStatement(stmt);
    }
    catch(SQLException ex)
   {
        sprintf(ErrLog, "Exception thrown insert into ads_status_info at %s:%d,Error number:%d:%s", __FILE__, __LINE__,ex.getErrorCode(),(ex.getMessage()).c_str());
        cout << "Error number: " << ex.getErrorCode() << endl;
        cout << (ex.getMessage()).c_str() << endl;
        (one->g_Conn)->terminateStatement(stmt);
        CSystemLog info("Oracle error",ErrLog);
        return;       
    }
    CSystemLog end("ADS status todb End!","");
    return;
}

void GetAdsLoadPerMin()
{
	list<ADSINFO>::iterator Iter;
	char Port[6] ={0};
	//ADSRESPONSE node;
	//char tmp[sizeof(ADSRESPONSE) + 1] = {0};
	float fCpuRate(0.0),fMemRate(0.0);
	int imaxbps = 0;
  int iactualbps = 0;
	for(Iter = g_AdsIpPortListTmp.begin(); Iter != g_AdsIpPortListTmp.end(); Iter ++)
	{
		std::string Url = "https://";
		std::string strResponse;
		sprintf(Port,"%d",Iter->Port);
		//if(Iter->Port == 0)
			Url = Url + Iter->AdsIP + "/facade/unifiedInterface.php?auth_key=" + Iter->Key + "&target=system&action_type=load";
		//else
		//	Url = Url + Iter->AdsIP + ":" + Port + "/facade/unifiedInterface.php?auth_key=" + Iter->Key + "&target=system&action_type=load";
		CSystemLog end("get ads status",Url.c_str());
		bool isSucess;// = HTTPSGet(Url,strResponse);
		CSystemLog res("ADS status response",strResponse.c_str());
		if(!isSucess)
			return;
		//memset(&node,0,sizeof(ADSRESPONSE));
		//strncpy(node.AdsIP,Iter->AdsIP,sizeof(node.AdsIP) );
		//sprintf(node.MsgType,"%d",0);
		//strncpy(node.Request,(char *)strResponse.c_str(),strResponse.length());
		
		imaxbps = atoi(getKeyFromString(strResponse.c_str(),"bps",":",",").c_str());
		CSystemLog Bps("bps",getKeyFromString(strResponse.c_str(),"bps",":",",").c_str());
		iactualbps = atoi(getKeyFromString(strResponse.c_str(),"actual_bps",":",",").c_str());
		fCpuRate = atof(getKeyFromString(strResponse.c_str(),"cpu",":",",").c_str());
		fMemRate = atof(getKeyFromString(strResponse.c_str(),"mem",":",",").c_str());
		AdsStatusToDb(Iter->AdsIP,imaxbps,iactualbps,fCpuRate,fMemRate);
		//memset(tmp,0,sizeof(tmp));
		//memcpy(tmp,&node,sizeof(ADSRESPONSE));
		//g_Client.RabbitMqSend(tmp,sizeof(ADSREQUEST),2);
	}
}


void *ReportThread(void *)
{
	while(1)
	{
			//ProcessReportFile();
			GetAdsLoadPerMin();
			sleep(60);
	}
}

/*
 * Copyright (c) 2015,������ʿͨ��Ϣ�������޹�˾IP&���簲ȫ��ҵ��
 * All rights reserved.
 *
 * �ļ����ƣ�ArborFlow
 * �ļ���ʶ�������ù����ƻ���
 * ժ    Ҫ��
 *
 * ��ǰ�汾��
 * ��    �ߣ�mumu <lanqb@ffcs.cn >
 * �������ڣ�2015��1��30��
 *
 * ȡ���汾��
 * ��    �ߣ�
 * �������ڣ�
 */
#include "ArborFlow.h"
#include <string>
#include <boost/regex.hpp>
#include <boost/date_time.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp> 
#include <boost/lexical_cast.hpp>
 
#include "Log.h"
#include "Tools.h"



using namespace std;
using namespace boost;

#define RegexConfigFile "../config/ArborFlowRegex.xml"

 
string chang_time(string m, string d, string now);
string destion_change(string des);

 
 
 
/********************************************
 * ��  ��: CArborFlow
 * ��  ��: ���캯��
 * ��  ��: ��
 * ����ֵ: ��
 ********************************************/
CArborFlow::CArborFlow()
{
	m_devicetype = 30301;
	initRegexConfig();
}

/********************************************
 * ��  ��: ~CArborFlow
 * ��  ��: ��������
 * ��  ��: ��
 * ����ֵ: ��
 ********************************************/
CArborFlow::~CArborFlow()
{

}
 bool CArborFlow::isArbFlow(uint32_t device)
 {
	 if (device == m_devicetype)
		 return true;
	 return false;
 }
int CArborFlow::initRegexConfig()
{
	boost::property_tree::ptree pt;
	try {
		boost::property_tree::read_xml(RegexConfigFile, pt);
		pt = pt.get_child("config");
		for (boost::property_tree::ptree::iterator itr = pt.begin(); itr != pt.end(); itr++)
		{
			if (itr->first == "devicetype")
			{
				m_devicetype = pt.get<uint32_t>("devicetype");
			}

			if (itr->first == "Parser")
			{
				insertReg(itr->second.data());
			}
		}
	}
	catch (std::exception &r_e)
	{
		//myLog(FATAL_LOG_LEVEL, g_p_defaultLog, "load config %s error : %s", RegexConfigFile, r_e.what());
		return 1;
	}
	//myLog(FATAL_LOG_LEVEL, g_p_defaultLog, "load config %s error : %s", p_configFile, r_e.what());
	return 0;
}


void CArborFlow::insertReg(std::string str)
{
	 boost::regex temp(str);
	m_RegArray.push_back(temp);
}

void Analyzestr(string str,double *Mbps,double *Kpps) //914.13 Mbps/127.20 Kpps
{
	int pos_Mbps=str.find(" ",0); 
	int pos_Kpps=str.find(" ",pos_Mbps+1);
	int pos=str.find("/",0);
	string Mbps_str=str.substr(0,pos_Mbps);
	string Kpps_str=str.substr(pos+1,pos_Kpps-pos-1);

	double Mpbs_num=atof(Mbps_str.c_str());
	if(str.find("Gbps",pos_Mbps)!=string::npos)
	{
		*Mbps=1024*Mpbs_num;
	}else
	if(str.find("Mbps",pos_Mbps)!=string::npos)
	{
		*Mbps=Mpbs_num;
	}else
	if(str.find("Kbps",pos_Mbps)!=string::npos)
	{
		*Mbps=Mpbs_num/1024.0;
	}else
	 if(str.find("bps",pos_Mbps)!=string::npos)
        {   
                *Mbps=Mpbs_num/1024.0/1024.0;
        } 
		
	double Kpps_num=atof(Kpps_str.c_str());
	if(str.find("G",pos_Kpps)!=string::npos)
	{
		*Kpps=1024*Kpps_num;
	}else
	 if(str.find("M",pos_Kpps)!=string::npos)           //	if(str.find("M",pos1,4))
	{
		*Kpps=Kpps_num;
	}else
	if(str.find("K",pos_Kpps)!=string::npos)
	{
		*Kpps=Kpps_num/1024.0;
	}else
	{
		*Kpps=Kpps_num/1024.0/1024.0;
	}
	
	 
}

/********************************************
 * ��  ��: resolveSource
 * ��  ��: ����Դ��־
 * ��  ��: o_rFlowNode--�����ڵ㣻i_pData--����
 * ����ֵ: 0--�����ɹ���-1--����ʧ��
 ********************************************/
bool CArborFlow::resolveSource(bool m_debug_log, STEVENTFLOW  &o_rFlowNode, char *i_pData)
{
	 	
	bool bFlag = false;

	try
	{ 
		boost::cmatch mat;
		for (vector<boost::regex>::iterator itr = m_RegArray.begin(); itr != m_RegArray.end(); itr++)
		{
		string strYear = "";
		string strMonth = "";
		string strDate = "";
		boost::regex temp(*itr);
		bFlag = boost::regex_match(i_pData, mat,temp);
	 
		 
		if (bFlag)
		{
			if (mat[posDescription].matched)
			{
				string temp = destion_change(mat[posDescription].str());
				strncpy(o_rFlowNode.Description, const_cast<char *>(temp.c_str()), sizeof(o_rFlowNode.Description));
			}
			if (mat[posType].matched)
			{
				strncpy(o_rFlowNode.Type, const_cast<char *>(mat[posType].str().c_str()), sizeof(o_rFlowNode.Type));
				strncpy(o_rFlowNode.SubType, const_cast<char *>(mat[posType].str().c_str()), sizeof(o_rFlowNode.SubType));
			}
			if (mat[posAnomalyID].matched)
			{
				strncpy(o_rFlowNode.AnomalyID, const_cast<char *>(mat[posAnomalyID].str().c_str()), sizeof(o_rFlowNode.AnomalyID));
			}
			if (mat[posStatus].matched)
			{
				strncpy(o_rFlowNode.Status, const_cast<char *>(mat[posStatus].str().c_str()), sizeof(o_rFlowNode.Status));
			}
			if (mat[posSeverity].matched)
			{
				int level = atoi(mat[posSeverity].str().c_str());
				switch (level)
				{
				case 1:
					level = 3;
					break;
				case 3:
					level = 4;
					break;
				case 5:
					level = 5;
					break;
				default:
					break;
				}
				snprintf(o_rFlowNode.Severity, sizeof(o_rFlowNode.Severity), "%d", level);
			}
			if (mat[posImportance].matched)
			{
				strncpy(o_rFlowNode.Importance, const_cast<char *>(mat[posImportance].str().c_str()), sizeof(o_rFlowNode.Importance));

			}

			if (mat[posImpact].matched)
			{
				strncpy(o_rFlowNode.impact, const_cast<char *>(mat[posImpact].str().c_str()), sizeof(o_rFlowNode.impact));
				Analyzestr(mat[posImpact].str().c_str(), &o_rFlowNode.flow_max1, &o_rFlowNode.flow_max2);
			}
			if (mat[posSrcIP_1].matched)
			{
				strncpy(o_rFlowNode.SrcIP_1, const_cast<char *>(mat[posSrcIP_1].str().c_str()), sizeof(o_rFlowNode.SrcIP_1));
			}
			if (mat[posSrcPort_1].matched)
			{
                
                strncpy(o_rFlowNode.SrcPort_1, const_cast<char *>(mat[posSrcPort_1].str().c_str()), 
                    sizeof(o_rFlowNode.SrcPort_1));
				 
			}

			if (mat[posDstIP_1].matched)
			{
				strncpy(o_rFlowNode.DstIP_1, const_cast<char *>(mat[posDstIP_1].str().c_str()), sizeof(o_rFlowNode.DstIP_1));
			}

			if (mat[posDstPort_1].matched)
			{
                strncpy(o_rFlowNode.DstPort_1, const_cast<char *>(mat[posDstPort_1].str().c_str()),
                    sizeof(o_rFlowNode.DstPort_1));
			}

			if (mat[posResource].matched)
			{
		 
				strncpy(o_rFlowNode.Resource, const_cast<char *>(mat[posResource].str().c_str()), sizeof(o_rFlowNode.Resource));
			}
			if (mat[posYear].matched)
			{
				strYear = mat[posYear].str();
			}
			if (mat[posMon].matched)
			{
				strMonth = mat[posMon].str();
			}
			if (mat[posDate].matched)
			{
				strDate = mat[posDate].str();
			}
			if (mat[posTime].matched)
			{
				string strCreateTime = strYear + "-";
				strCreateTime += strMonth + "-";
				strCreateTime += strDate + " ";
				strCreateTime += mat[posTime];
				//strCreateTime = GreenToBeijing(const_cast<char*>(strCreateTime.c_str()));
				info_log("strCreateTime:%s",strCreateTime.c_str());
				strncpy(o_rFlowNode.CreationTime, strCreateTime.c_str(), sizeof(o_rFlowNode.CreationTime));
				info_log("o_rFlowNode.CreationTime:%s",o_rFlowNode.CreationTime);
			}
			if (mat[posDuration].matched)
			{
				strncpy(o_rFlowNode.Duration, const_cast<char *>(mat[posDuration].str().c_str()), sizeof(o_rFlowNode.Duration));
			}
			if (mat[posPercent].matched)
			{
				double percent;			 
				percent = boost::lexical_cast<double>(mat[posPercent].str());
				
				if (mat[posRate].matched)
				{
					 
					int nRate = 1;
					double fBpsRate = boost::lexical_cast<double>(mat[posRate].str());

					if (boost::icontains(mat[posUnit].str(), "k")  )
					{
						nRate = nRate * 1024;
					}
					if (boost::icontains(mat[posUnit].str(), "m") )
					{
						nRate = nRate * 1024 * 1024;
					}
					if (boost::icontains(mat[posUnit].str(), "pps")  )
					{
						//     �񾭲��Ĺ�ʽ
						fBpsRate = fBpsRate  * nRate * percent / (100000 * 1000.0);
					}
					else
					{
						//      fBpsRate = atol(mat[14].str().c_str()) * nRate;
						fBpsRate = fBpsRate*nRate;

					}
					strncpy(o_rFlowNode.Current, boost::lexical_cast<string>(fBpsRate).c_str(), boost::lexical_cast<string>(fBpsRate).size());
				 
				}
			}
			if (mat[posUnit].matched)
			{
				strncpy(o_rFlowNode.Unit, "Mpps", sizeof(o_rFlowNode.Unit));
			}
			if (mat[posProtocol].matched)
			{
				strncpy(o_rFlowNode.Protocol, const_cast<char *>(mat[posProtocol].str().c_str()), sizeof(o_rFlowNode.Protocol));
			}
			if (mat[posURL].matched)
			{
				strncpy(o_rFlowNode.URL, const_cast<char *>(mat[posURL].str().c_str()), sizeof(o_rFlowNode.URL));
			}
			

//			if (m_debug_log)
//				ffcsns::myLog(log4cplus::DEBUG_LOG_LEVEL, g_p_debugLog, "%s:%d|%s <DEBUG>is  match \t\n{%s}", __FILE__, __LINE__, __FUNCTION__, temp.str().c_str());
			break;
		}
		else
		{
		//	 if(m_debug_log)
		//	 ffcsns::myLog(log4cplus::DEBUG_LOG_LEVEL, g_p_debugLog, "%s:%d|%s <DEBUG>is not match \t\n{%s}", __FILE__, __LINE__, __FUNCTION__, temp.str().c_str());
		}
		}
	}
	catch (std::exception &e)
	{
//		myLog(FATAL_LOG_LEVEL, g_p_defaultLog, "FUNC:%s error : %s",__FUNCTION__, e.what());
	}
	 
	if (!bFlag)
	{
//		if (m_debug_log)
//		ffcsns::myLog(log4cplus::DEBUG_LOG_LEVEL, g_p_debugLog, "%s:%d|%s <DEBUG>the regex has not match source", __FILE__, __LINE__, __FUNCTION__);
		 
		return bFlag;
	}
	else	
	return true;
}
//�ı�ԭʼ��־����
string destion_change(string des)
{
	string temp;
	boost::smatch mat;
	const string strReg = "^(.*?<\\d+>)(\\w+\\s+)(\\d+\\s+)(\\d+:\\d+:\\d+)(\\s+pfsp.*?start\\s+)(.*\\s+\\d+:\\d+:\\d+)(.*?duration\\s+.*)$";
	boost::regex reg(strReg);
	try{
		bool bFlag = boost::regex_match(des, mat, reg);

		if (bFlag == true)
		{
			temp = mat[1].str();
			temp += chang_time(mat[2].str(), mat[3].str(), mat[4].str());
			temp += mat[5].str();
			temp += GreenToBeijing(const_cast<char*>( mat[6].str().c_str()));
			temp += mat[7].str();
			return temp;

		}

	}
	catch (std::exception &e)
	{
		cout << e.what();
		return des;
	}
	
	return des;
}


string chang_time(string m, string d, string now)
{
	string changtime = "";
	
	boost::posix_time::time_duration tnow = boost::posix_time::duration_from_string(now);

	tnow +=boost::posix_time::hours(8);
	if (tnow > boost::posix_time::hours(24))
	{
		tnow -= boost::posix_time::hours(24);
	}

	boost::posix_time::ptime today(boost::posix_time::second_clock::local_time());

	changtime = today.date().month().as_short_string();
	changtime += " " + boost::lexical_cast<string>(today.date().day().as_number()) + " ";
	changtime += to_simple_string(tnow);
	return changtime;
}

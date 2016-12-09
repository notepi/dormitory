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
#ifndef ARBOR_FLOW_H
#define ARBOR_FLOW_H
#include <vector>
#include <boost/regex.hpp>
#include "DataDefine.h"
enum
{
	posDescription=0,
	posType=1,
	posAnomalyID=2,
	posStatus=3,
	posSeverity =4,
	posImportance=5,
	posImpact=6,
	posSrcIP_1=7,
	posSrcPort_1=8,
	posDstIP_1=9,
	posDstPort_1=10,
	posResource=11,
	posYear=12,
	posMon=13,
	posDate=14,
	posTime=15,
	posDuration=16,
	posPercent=17,
	posRate=18,
	posUnit=19,
	posProtocol=20,
	posURL=21
};















class CArborFlow
{
public:
	CArborFlow();
	~CArborFlow();
	bool isArbFlow(uint32_t);

	int initRegexConfig();
	bool resolveSource(bool m_debug_log, STEVENTFLOW  & o_rFlowNode, char * i_pData);

protected:
	void insertReg(std::string str);

private:
	
	uint32_t m_devicetype ;
	std::vector<boost::regex> m_RegArray;
	
};

 
#endif 

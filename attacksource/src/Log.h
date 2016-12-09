/*
 * Copyright (c) 2014,������ʿͨ��Ϣ������޹�˾IP&���簲ȫ��ҵ��
 * All rights reserved.
 *
 * �ļ����ƣ�Log
 * �ļ���ʶ�������ù���ƻ���
 * ժ    Ҫ����־��Ϣ��ӡ��
 *
 * ��ǰ�汾��1.0
 * ��    �ߣ�mumu <lanqb@ffcs.cn >
 * ������ڣ�2014��2��15��
 *
 * ȡ���汾��
 * ��    �ߣ�
 * ������ڣ�
 */
#ifndef UTIL_LOG_H
#define UTIL_LOG_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "util/Log4cplusUtil.h"
#include "util/DefaultLog.h"
using namespace ffcsns;
using namespace log4cplus;
/*��ʾ��Ϣ����*/
extern int Debug_On;
/*����log��Ϣ����*/
extern int Info_On;
extern Logger *g_p_debugLog;


/*�ļ�log��ӡ��Ϣ��*/
#define debug_log(format,...) myLog(DEBUG_LOG_LEVEL,g_p_debugLog,"<DEBUG>"format,##__VA_ARGS__)
#define info_log(format,...) myLog(INFO_LOG_LEVEL,g_p_defaultLog,"<INFO>"format,##__VA_ARGS__)
#define warn_log(format,...) myLog(WARN_LOG_LEVEL,g_p_defaultLog,"<WARN>"format,##__VA_ARGS__)
#define err_log(format,...) myLog(ERROR_LOG_LEVEL,g_p_defaultLog,"<ERR>"format,##__VA_ARGS__)
#define fatal_log(format,...) myLog(FATAL_LOG_LEVEL,g_p_defaultLog,"<FATAL>"format,##__VA_ARGS__)
#endif


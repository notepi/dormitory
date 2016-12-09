/*
 * Copyright (c) 2014,福建富士通信息软件有限公司IP&网络安全事业部
 * All rights reserved.
 *
 * 文件名称：Log
 * 文件标识：见配置管理计划书
 * 摘    要：日志信息打印宏
 *
 * 当前版本：1.0
 * 作    者：mumu <lanqb@ffcs.cn >
 * 完成日期：2014年2月15日
 *
 * 取代版本：
 * 作    者：
 * 完成日期：
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
/*提示信息开关*/
extern int Debug_On;
/*调试log信息开关*/
extern int Info_On;
extern Logger *g_p_debugLog;


/*文件log打印信息宏*/
#define debug_log(format,...) myLog(DEBUG_LOG_LEVEL,g_p_debugLog,"<DEBUG>"format,##__VA_ARGS__)
#define info_log(format,...) myLog(INFO_LOG_LEVEL,g_p_defaultLog,"<INFO>"format,##__VA_ARGS__)
#define warn_log(format,...) myLog(WARN_LOG_LEVEL,g_p_defaultLog,"<WARN>"format,##__VA_ARGS__)
#define err_log(format,...) myLog(ERROR_LOG_LEVEL,g_p_defaultLog,"<ERR>"format,##__VA_ARGS__)
#define fatal_log(format,...) myLog(FATAL_LOG_LEVEL,g_p_defaultLog,"<FATAL>"format,##__VA_ARGS__)
#endif


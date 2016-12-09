#ifndef _CURLHTTPS_H_
#define _CURLHTTPS_H_

int CurlHttpsGet(std::string strUrl,std::string& strResponse,bool bPlus);
int CurlHttps(std::string strUrl,std::string& strResponse,bool bPlus,std::string strPostString="",std::string strToken="");
int CurlHttpsWithFile(std::string strUrl,std::string strPath,std::string& strResponse,std::string& strUser,std::string& strPwd);
std::string CurlHttpsPost(std::string strUrl,bool bPlus,std::string strPostString,std::string strToken);
void ClearResultXml();















#endif


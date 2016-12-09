#include <stdio.h>
#include<string>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <iconv.h>
#include <dirent.h>
#include <time.h>
#include <sched.h>
#include <iostream>
//#include <boost/network/protocol/http/client.hpp>
//#include "Init.h"
#include "UDPClient.h"

#define BUF_MAX_LEN 40960
#define NOTFOUNDSTRING "not found"
using namespace std;
//using namespace boost::network;
//using namespace boost::network::http;
	
//typedef basic_client<http::tags::http_keepalive_8bit_tcp_resolve, 1, 1> kaclient;
int trave_dir(char* path, int depth,char cArray[][256])
{
	    DIR *d;     struct dirent *file;     struct stat sb;   
	     	
	       int len =0;	
	        if(!(d = opendir(path)))
		{
			printf("error opendir %s!!!\n",path);
			return -1;
		}
		while((file = readdir(d)) != NULL)
		{
			if(strncmp(file->d_name, ".", 1) == 0)
			continue;
		
			strcpy(cArray[len++], file->d_name);    
		   	if(stat(file->d_name, &sb) >= 0 && S_ISDIR(sb.st_mode) && depth <= 3)
			{
				trave_dir(file->d_name, depth + 1,cArray);
			}
		}
		        closedir(d);
			return len;
}
int TimeConver(const char *Time,char *StaTime,int Length)
{ 
	char *p = NULL;
	char Temp[100];
	char TmpBuf[100];
	int Month = 0;
	char Year[10];
	int day;
	char date[15];
	p = strchr(Time,' ');
	if(p == NULL)
		return 1;
	memset(Temp,0,sizeof(Temp));
	strncpy(Temp,p + 1,strlen(p + 1));
	p = strchr(Temp,' ');
	if(p == NULL)
		return 1;
	memset(TmpBuf,0,sizeof(TmpBuf));
	strncpy(TmpBuf,p + 1,strlen(p + 1));
	if(TmpBuf[0] == ' ')
		strncpy(TmpBuf,p + 2,strlen(p + 2));
	Temp[p - Temp] = '\0';
	p = strchr(Temp,' ');
	if(strstr(Temp,"Jan") != NULL)
		Month = 1;
	else if(strstr(Temp,"Feb") != NULL)
		Month = 2;
	else if(strstr(Temp,"Mar") != NULL)
		Month = 3;
	else if(strstr(Temp,"Apr") != NULL)
		Month = 4;
	else if(strstr(Temp,"May") != NULL)
		Month = 5;
	else if(strstr(Temp,"Jun") != NULL)
		Month = 6;
	else if(strstr(Temp,"Jul") != NULL)
		Month = 7;	
	else if(strstr(Temp,"Aug") != NULL)
		Month = 8;
	else if(strstr(Temp,"Sep") != NULL)
		Month = 9;
	else if(strstr(Temp,"Oct") != NULL)
		Month = 10;
	else if(strstr(Temp,"Nov") != NULL)
		Month = 11;
	else if(strstr(Temp,"Dec") != NULL)
		Month = 12;	
	else
		return 1;
	p = strchr(TmpBuf,' ');
	if(p == NULL)
		return 1;
	memset(Temp,0,sizeof(Temp));
	strncpy(Temp,p + 1,strlen(p + 1));
	TmpBuf[p - TmpBuf] = '\0';	
	day = atoi(TmpBuf);
	p = strchr(Temp,' ');
	if(p == NULL)
		return 1;
	memset(Year,0,sizeof(Year));
	strncpy(Year,p + 1,strlen(p + 1));
	Year[4] = '\0';
	Temp[p - Temp] = '\0';
	strncpy(date,Temp,strlen(Temp));
	memset(Temp,0,sizeof(Temp));
	snprintf(Temp,sizeof(Temp),"%s/%02d/%02d %s",Year,Month,day,date);
	strncpy(StaTime,Temp,Length);
	return 0;														
}



//add by wanglihui
/*****************************************************
 * º¯Êý: long int CountTime(char* startTime,char* endTime)
 *
 * ¹¦ÄÜ: ¼ÆËãÊ±¼ä²î(µ¥Î»:Ãë)
 *
 * ²ÎÊý: startTime-->¿ªÊ¼Ê±¼ä£»endTime-->½áÊøÊ±¼ä
 *
 * ·µ»ØÖµ: Ê±¼ä²î(Ãë)
 *****************************************************/
long int CountTime(char* startTime,char* endTime)
{
    char st1[50];
    char et1[50];
    char st2[50];
    char et2[50];
    int week1 = 0;
    int week2 = 0;
    long int totalTime = 0;
    char *p = NULL;
    char *q = NULL;
    char temp[18];
    long int a = 0;
    long int sum1 = 0;
    long int sum2 = 0;
    
    memset(st1,0,sizeof(st1));
    memset(et1,0,sizeof(et1));
    memset(st2,0,sizeof(st2));
    memset(et2,0,sizeof(et2));

    strcpy(st1,startTime);
    strcpy(et1,endTime);

    p = strchr(st1,' ');
    if(p != NULL)
    {
        p = p + 1;        
        strcpy(st1,p);
        p = st1;
        printf("st1 = %s\n",st1);
        q = strchr(st1,':');
        if(q != NULL)
        {
            memset(temp,0,sizeof(temp));
            strncpy(temp,st1,q - p);
            printf("temp = %s\n",temp);
            a = atol(temp);
            sum1 = a*60*60;

            p = q + 1;
            strcpy(st1,p);
            p = st1;
            printf("st1 = %s\n",st1);
            q = strchr(st1,':');
            if(q != NULL)
            {
                memset(temp,0,sizeof(temp));
                strncpy(temp,st1,q - p);
                a = atol(temp);
                sum1 += a*60;
                
                p = q + 1;
                strcpy(st1,p);
                p = st1;
                printf("st1 = %s\n",st1);
                memset(temp,0,sizeof(temp));
                strcpy(temp,st1);
                a = atol(temp);
                sum1 += a;
            }
        }
    }
    p = NULL;
    q = NULL;
    p = strchr(et1,' ');
    if(p != NULL)
    {
        p = p + 1;        
        strcpy(et1,p);
        p = et1;
        printf("et1 = %s\n",et1);
        q = strchr(et1,':');
        if(q != NULL)
        {
            memset(temp,0,sizeof(temp));
            strncpy(temp,et1,q - p);
            a = atol(temp);
            sum2 = a*60*60;

            p = q + 1;
            strcpy(et1,p);
            p = et1;
            printf("et1 = %s\n",et1);
            q = strchr(et1,':');
            if(q != NULL)
            {
                memset(temp,0,sizeof(temp));
                strncpy(temp,et1,q - p);
                a = atol(temp);
                sum2 += a*60;

                p = q + 1;
                strcpy(et1,p);
                p = et1;
                printf("et1 = %s\n",et1);
                memset(temp,0,sizeof(temp));
                strcpy(temp,et1);
                a = atol(temp);
                sum2 += a;
            }
        }
    }

        if((sum2 - sum1) < 0)
            totalTime = (sum2 - sum1) + 24*60*60;
        else
            totalTime = sum2 - sum1;
    
        
    return totalTime;
}


/*
 *
 * º¯Êý: int code_convert(char *from_code, char *to_code, char *from, char *to)
 *
 * ¹¦ÄÜ:
 *      ±àÂë×ª»»º¯Êý
 *
 * ²ÎÊý:
 *      from_code:Ô´µÄ±àÂë¸ñÊ½,to_code: Ä¿±êµÄ±àÂë¸ñÊ½,from:Ô´×Ö·û´®,to:Ä¿±ê×Ö·û´®
 *
 * ·µ»ØÖµ:
 *     0 - ³É¹¦ ; 1,-1 - Ê§°Ü
 *
 */  
int code_convert(char *from_code, char *to_code, char *from, char *to)    
{
    iconv_t cd;
    char *tptr, *fptr;
    size_t ileft, oleft, ret;

    tptr = fptr =NULL;
    
	  if(from == NULL || to == NULL || strlen(from) == 0) 
	  {
	  	  if(to != NULL)
	  	  {
	  	  	  *to = 0;
	  	  }
	  	  return -1;
	  }

    cd = iconv_open((const char *)to_code, (const char *)from_code);
    if (cd == (iconv_t)-1) {

        return -1;
    }

    ileft = strlen(from);
    fptr = from;
    for (;;) 
    {
        tptr = to;
        oleft = BUF_MAX_LEN;

        ret = iconv(cd, &fptr, &ileft, &tptr, &oleft);
        if (ret != (size_t)-1) {
            *tptr = '\0';
            break;
        }
    
        //cout<<"code convert failed"<<endl;
        *tptr = '\0';
        if (errno == EINVAL) {
            cout<<"EINVAL ERROR"<<endl;
            (void) memmove(from, fptr, ileft);
            (void) iconv(cd, &fptr, &ileft, &tptr, &oleft);
            (void) iconv_close(cd);
            *tptr = '\0';
            return 1;
        } else if (errno == E2BIG) {
            cout<<"E2BIG"<<endl;
            continue;
        } else if (errno == EILSEQ) {
            //cout<<"EILSEQ"<<endl;
            (void) iconv(cd, &fptr, &ileft, &tptr, &oleft);
						(void) iconv_close(cd);
						*tptr = '\0';
            return 1;
        } else if (errno == EBADF) {
            cout<<"EBADF ERROR"<<endl;
			      (void) iconv_close(cd);
						*tptr = '\0';
            return -1;
        } else {
            cout<<"Unknown ERROR"<<endl;
			      (void) iconv_close(cd);
						*tptr = '\0';
            return -1;
        }
    }
    
    //(void) iconv(cd, &fptr, &ileft, &tptr, &oleft);
    (void) iconv_close(cd);
    return 0;
}   


int ChangeDateTimeFormat(char *srcTime,char *DstTime)
{//Fri Jan 18 08:01:00 2013
    char temp[20];
    memset(temp,0,sizeof(temp));
    char buf[100];
    memset(buf,0,sizeof(buf));
    char year[10];
    memset(year,0,sizeof(year));
    char month[10];
    memset(month,0,sizeof(month));
    char day[10];
    memset(day,0,sizeof(day));
    char nowtime[10];
    memset(nowtime,0,sizeof(nowtime));
    char *p;
    char *q;

    strncpy(buf,srcTime,sizeof(buf) - 1);
    p = strchr(buf,' ');
    if(p == NULL)
        return 1;
    p = p + 1;
    strcpy(buf,p);
	//printf("buf:%s\n",buf);

    p = buf;
    q = NULL;
    q = strchr(buf,' ');
    if(q == NULL)
        return 1;

    strncpy(temp,buf,q - p);

    if(strncmp(temp,"Jan",3) == 0)
        strncpy(month,"1",sizeof(month));
    else if(strncmp(temp,"Feb",3) == 0)
        strncpy(month,"2",sizeof(month));
    else if(strncmp(temp,"Mar",3) == 0)
        strncpy(month,"3",sizeof(month));
    else if(strncmp(temp,"Apr",3) == 0)
        strncpy(month,"4",sizeof(month));
    else if(strncmp(temp,"May",3) == 0)
        strncpy(month,"5",sizeof(month));
    else if(strncmp(temp,"Jun",3) == 0)
        strncpy(month,"6",sizeof(month));
    else if(strncmp(temp,"Jul",3) == 0)
        strncpy(month,"7",sizeof(month));
    else if(strncmp(temp,"Aug",3) == 0)
        strncpy(month,"8",sizeof(month));
    else if(strncmp(temp,"Sep",3) == 0)
        strncpy(month,"9",sizeof(month));
    else if(strncmp(temp,"Oct",3) == 0)
        strncpy(month,"10",sizeof(month));
    else if(strncmp(temp,"Nov",3) == 0)
        strncpy(month,"11",sizeof(month));
    else if(strncmp(temp,"Dec",3) == 0)
        strncpy(month,"12",sizeof(month));

    p = q + 1;
    if(*p == ' ')
    {
        p = p + 1;
    }
    strcpy(buf,p);
	//printf("buf:%s\n",buf);
    p = buf;
    q = NULL;
    q = strchr(buf,' ');
    if(q == NULL)
        return 1;
    
    
    strncpy(day,buf,q - p);

    p = q + 1;
    strcpy(buf,p);
    p = buf;
    q = NULL;
    q = strchr(buf,' ');
    if(q == NULL)
        return 1;
    
    strncpy(nowtime,buf,q - p);    

    p = q + 1;
    strcpy(buf,p);
    strncpy(year,buf,sizeof(year));

    memset(buf,0,sizeof(buf));
    snprintf(buf,sizeof(buf),"%s/%s/%s %s",year,month,day,nowtime);
    strcpy(DstTime,buf);
    
    return 0;
}


/*
 *
 * º¯Êý: GetSystemTime()
 *
 * ¹¦ÄÜ:
 *      »ñÈ¡µ±Ç°ÏµÍ³Ê±¼ä 
 *
 * ²ÎÊý:
 *      ÎÞ
 *
 * ·µ»ØÖµ:
 *       ÏµÍ³µ±Ç°Ê±¼ä,"YYYY/MM/DD HH:MM:SS"
 *
 */
std::string GetSystemTime()
{
   char Time[30];
   time_t timep;
   struct tm *p;
   time (&timep);
   p = localtime(&timep);
   snprintf(Time , sizeof(Time),"%4d/%02d/%02d %02d:%02d:%02d" ,1900+p->tm_year ,1+p->tm_mon ,p->tm_mday ,p->tm_hour ,p->tm_min ,p->tm_sec);
   std::string SystemTime(Time);
   return SystemTime;    
}

/*
 *
 * º¯Êý: GetSystemDay()
 *
 * ¹¦ÄÜ:
 *      »ñÈ¡µ±Ç°ÏµÍ³ÌìÊý 
 *
 * ²ÎÊý:
 *      ÎÞ
 *
 * ·µ»ØÖµ:
 *       ÏµÍ³µ±Ç°Ê±¼ä,"YYYY/MM/DD"
 *
 */
std::string GetSystemDay()
{
   char Time[30];
   time_t timep;
   struct tm *p;
   time (&timep);
   p = localtime(&timep);
   snprintf(Time ,sizeof(Time), "%4d%02d%02d" ,1900+p->tm_year ,1+p->tm_mon ,p->tm_mday);
   std::string SystemTime(Time);
   return SystemTime;    
}



/***********************************************************************
 * º¯Êý: int GetStrValue(const char *key,char *value,const char *buff)
 *
 * ¹¦ÄÜ:  È¡µÃÏàÓ¦×Ö¶ÎµÄÖµ
 *
 * ²ÎÊý:  ×Ö¶Î£»key£º±êÖ¾£»value: ×Ö¶ÎµÄÖµ buff: ´«ÈëÖµ
 *
 * ·µ»ØÖµ: ·µ»Ø0 ±íÊ¾³É¹¦£» ·µ»Ø-1 ±íÊ¾Ê§°Ü; 
 *
 ***********************************************************************/
int GetStrValue(const char *key,char *value,const char *buff)
{
	  char *pConfig, *pDetail, *pTemp;
	  char Item[100];
	  int i;

	  //search the string session "[%s]"
	  pConfig = strstr(buff, key);
	  if (pConfig == NULL)
	  {
	  	  return -1;
	  }
	  //search the string key ":"	  
	  pConfig = strchr(pConfig, ':');
	  if(pConfig == NULL)
	  {
	  	  return -1;
	  }
	  pConfig = pConfig + 1 ;
	  
	  //search the string key ";"
	  pDetail = strchr(pConfig, ';');
	  if(pDetail == NULL)
	  {
	  	  return -1;
	  }

    
	  //go to the end of "=  "
	  for (i = 0; i < strlen(pConfig); ++i)
	  {
	  	  if (pConfig[i] == ' ')
	  	  {
	  	  	  continue;
	  	  }
	  	  else
	  	  {
	  	  	  break;
	  	  }
	  }
	  pConfig = pConfig + i ;
    
	  strncpy(value,pConfig, pDetail - pConfig);
	  value[pDetail - pConfig] = '\0';
	  return 0;
}

std::string getKeyFromString(std::string _Resource,std::string _key,std::string _keyLeft,std::string _keyEnd)
{
	std::string strTmp = _Resource;
	std::string strKey = _key;

	size_t front = strTmp.find(strKey.c_str());
	if(front != std::string::npos)
	{
		//CSystemLog info("Key",strKey.c_str());
		strTmp = strTmp.substr(front);
		front = strTmp.find(_keyLeft.c_str());
		if(front != std::string::npos)
		{
			strTmp = strTmp.substr(front + _keyLeft.length());
			front = strTmp.find(_keyEnd.c_str());
			if(front != std::string::npos)
			{
				//CSystemLog info("value",strTmp.substr(0,front).c_str());
				return strTmp.substr(0,front);
			}
			else
				return NOTFOUNDSTRING; 
		}
		else
			return NOTFOUNDSTRING;
	}
	else
	{
		//CSystemLog info("Key",strKey.c_str());
		return NOTFOUNDSTRING;
	}
}

/*bool HTTPSGet(const std::string& url,std::string &response)
{
	try
	{
		kaclient::options options;
 		options.cache_resolved(true);
 		kaclient client_(options);	
		CSystemLog request("Url",url.c_str());	
		kaclient::request request_(url);
		request_ << header("Connection", "keep-alive");
		kaclient::response response_ = client_.get(request_);
		{
			boost::network::http::headers_range<kaclient::response>::type headers_ = response_.headers();
			typedef std::pair<std::string, std::string> header_type;
			BOOST_FOREACH(header_type const & header, headers_) {
					std::cout << header.first << ": " << header.second << std::endl;
					std::string Out = header.first + ":" +  header.second;
			}
			std::cout << std::endl;
		}		
		cout << response_.status() << endl;
		response = body(response_);	
		CSystemLog re("response",response.c_str());
		return true;
	}
	catch(std::exception& r_e)
	{
		cout << "get http error : " << r_e.what() << endl;	
		std::string Out = "get http error : ";
		Out = Out  +  r_e.what();
		CSystemLog info("error",Out.c_str());
		return false;
	}
}*/
#if 1
int SendSysLog(std::string AdsIP,char *Log,int iLen)
{
	CUdpClient SyslogClient;	
	int iRet = 0;	
	iRet = SyslogClient.m_iOpen(514,(char *)AdsIP.c_str());	
	if (iRet == 1)	
	{	  	
		fprintf(stderr, "%s:%d Intialize UDP client failed!\n", __FILE__, __LINE__);
		return 1;	
	}	
	iRet = SyslogClient.m_iSendTo(Log, iLen);
	SyslogClient.m_iClose();       
	return iRet;
		
}
#endif
//æ ¼æž—å°¼æ²»æ—¶é—´è½¬æ¢æˆåŒ—äº¬æ—¶é—´%Y-%m-%d %H:%M:%S æ ¼å¼çš„è½¬æ¢+8 å†è½¬æˆ%Y/%m/%d %H:%M:%S
std::string GreenToBeijing(char *time)
{

		//Fri Jan 18 08:01:00 2013
			char buf[21] = { 0 };
				struct tm  t = { 0 };
					
						if (strptime(time, "%Y-%m-%d %H:%M:%S", &t) == NULL) {
									return time;
										}
											t.tm_hour += 8;
#ifdef WIN32
												_snprintf_s(buf, sizeof(buf), 20, "%4d/%02d/%02d %02d:%02d:%02d",
														t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);

#else
	snprintf(buf, sizeof(buf), "%4d/%02d/%02d %02d:%02d:%02d",
			t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);

#endif

	std::string temp(buf);
		return  temp;
}
static inline bool is_base64(unsigned char c) {
	  return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
	  std::string ret="";;
	    int i = 0;
		  int j = 0;
		    unsigned char char_array_3[1024]={0};
			  unsigned char char_array_4[1024]={0};
			    const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
				  while (in_len--) {
					      char_array_3[i++] = *(bytes_to_encode++);
						      if (i == 3) {
								        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
										      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
											        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
													      char_array_4[3] = char_array_3[2] & 0x3f;

														        for(i = 0; (i <4) ; i++)
																	      {
																			  		  int j = char_array_4[i];
																					  		  ret += base64_chars[j];
																							  	  }
																								        i = 0;
																										    }
																											  }

																											    if (i)
																													  {
																														      for(j = i; j < 3; j++)
																																        char_array_3[j] = '/0';

																																		    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
																																			    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
																																				    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
																																					    char_array_4[3] = char_array_3[2] & 0x3f;

																																						    for (j = 0; (j < i + 1); j++)
																																								      ret += base64_chars[char_array_4[j]];

																																									      while((i++ < 3))
																																											        ret += '=';

																																													  }

																																													    return ret;

}

std::string base64_decode(std::string const& encoded_string) {
	  int in_len = encoded_string.size();
	    int i = 0;
		  int j = 0;
		    int in_ = 0;
			  unsigned char char_array_4[1024], char_array_3[1024];
			    const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
				  std::string ret;

				    while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
						    char_array_4[i++] = encoded_string[in_]; in_++;
							    if (i ==4) {
									      for (i = 0; i <4; i++)
											          char_array_4[i] = base64_chars.find(char_array_4[i]);

													        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
															      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
																        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

																		      for (i = 0; (i < 3); i++)
																				          ret += char_array_3[i];
																						        i = 0;
																								    }
																									  }

																									    if (i) {
																											    for (j = i; j <4; j++)
																													      char_array_4[j] = 0;

																														      for (j = 0; j <4; j++)
																																        char_array_4[j] = base64_chars.find(char_array_4[j]);

																																		    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
																																			    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
																																				    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

																																					    for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
																																						  }

																																						    return ret;
}

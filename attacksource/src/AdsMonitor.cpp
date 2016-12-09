//#include "AdsMonitor.h"

//extern CRabbitMqClient g_Client;

/*bool HTTPSGet(const string& url,std::string &response)
{
	try
	{
		kaclient::options options;
 		options.cache_resolved(true);
 		kaclient client_(options);	
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
		return true;
	}
	catch(std::exception& r_e)
	{
		cout << "get http error : " << r_e.what() << endl;	
		std::string Out = "get http error : ";
		return false;
	}
}*/
#if 0
void GetAdsLoad(const string& ip,const string& request)
{
	std::string AdsIP = ip;
	ADSRESPONSE node;
	char tmp[sizeof(ADSRESPONSE) + 1] = {0};
	list<ADSINFO>::iterator Iter;
	std::string Url = request;
	std::string Response = "";
	bool isSucess = HTTPSGet(Url,Response);
	if(!isSucess)
		return;
	memset(&node,0,sizeof(ADSRESPONSE));
	strncpy(node.AdsIP,ip.c_str(),sizeof(node.AdsIP) );
	sprintf(node.MsgType,"%d",3);
	strncpy(node.Request,(char *)Response.c_str(),Response.length());
		
	memset(tmp,0,sizeof(tmp));
	memcpy(tmp,&node,sizeof(ADSRESPONSE));
	g_Client.RabbitMqSend(tmp,sizeof(ADSREQUEST),2);
}


int SendSysLog(std::string AdsIP,char *Log,int iLen)
{
	/*CUdpServer SyslogClient;	
	int iRet = 0;	
	iRet = SyslogClient.m_iOpen(AdsIP.c_str(),514);	
	if (iRet == 1)	
	{	  	
		fprintf(stderr, "%s:%d Intialize UDP client failed!\n", __FILE__, __LINE__);
		return 1;	
	}	
	iRet = SyslogClient.m_iSendTo(AdsIP.c_str(),514,Log, iLen);
	SyslogClient.m_iClose();       
	return iRet;
	*/	
}
#endif

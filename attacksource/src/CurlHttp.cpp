/*
作者:huanglr
时间: 2015/10/20
http://www.cnblogs.com/lidabo/p/4159574.html
http://curl.haxx.se/libcurl/c/https.html
*/
#include <stdio.h>  
#include <string.h>    
#include <string>  
#include <iostream>
#include <curl/curl.h>
#include "Log.h"
using namespace std;
static size_t wrfu(void *ptr,  size_t  size,  size_t  nmemb,  void *stream)
{
  (void)stream;
  (void)ptr;
  return size * nmemb;
}
struct MemoryStruct {
	  char *memory;
	  size_t size;
};
size_t memory_callback(void *data, size_t size, size_t nmemb, void *response)
{
    /*size_t realsize = size * nmemb;	
    memcpy(response + realsize, data, realsize);
    //p[len + realsize] = '\0';
		*((char *)response + realsize) = '\0';*/
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)response;
	   
	mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
	if(mem->memory == NULL) {
	/* out of memory! */ 
	err_log("not enough memory (realloc returned NULL)\n");
	return 0;
	}

	memcpy(&(mem->memory[mem->size]), data, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

/*		FILE *fp = NULL;
		fp = fopen("result.xml","a+");
		if(fp == NULL)
			return realsize;
		fprintf(fp,(char *)response,strlen((char *)response));
		fclose(fp);
		fp = NULL;*/
    return  realsize;
}

size_t memory_callback_plus(void *data, size_t size, size_t nmemb, void *response)
{
    size_t realsize = size * nmemb;	
    memcpy(response, data, realsize);
    //p[len + realsize] = '\0';
		*((char *)response + realsize) = '\0';
		
		FILE *fp = NULL;
		fp = fopen("result.xml","w+");
		if(fp == NULL)
			return realsize;
		fprintf(fp,(char *)response,strlen((char *)response));
		fclose(fp);
		fp = NULL;
    return  realsize;
}

void ClearResultXml()
{
	FILE *fp = NULL;
	fp = fopen("result.xml","w");
	if(fp == NULL)
	  return ;
	fclose(fp);   
}

int CurlHttpsGet(std::string strUrl,std::string& strResponse,bool bPlus)  
{  
  CURL *curl;  
  CURLcode res;  
  
  //struct curl_httppost *formpost=NULL;  
  //struct curl_httppost *lastptr=NULL;  
  //struct curl_slist *headerlist=NULL;  
  //static const char buf[] = "Expect:";  
  
  //curl_global_init(CURL_GLOBAL_ALL);  
 curl_global_init(CURL_GLOBAL_DEFAULT);
 #if 0
  /* Fill in the file upload field */  
  curl_formadd(&formpost,  
               &lastptr,  
               CURLFORM_COPYNAME, "config_xml",  
               CURLFORM_FILE, strPath.c_str(),  
               CURLFORM_END);  
  
  /* Fill in the filename field */  
  curl_formadd(&formpost,  
               &lastptr,  
               CURLFORM_COPYNAME, "type",  
               CURLFORM_COPYCONTENTS, "2",  
               CURLFORM_END);  
  
  /* Fill in the submit field too, even if this is rarely needed */  
  /*curl_formadd(&formpost,  
               &lastptr,  
               CURLFORM_COPYNAME, "submit",  
               CURLFORM_COPYCONTENTS, "Submit",  
               CURLFORM_END);  */
  #endif
  curl = curl_easy_init();  
  /* initalize custom header list (stating that Expect: 100-continue is not 
     wanted */  
 	
  //	headerlist = curl_slist_append(headerlist, buf);  
  
  //printf("Url:%s\n",strUrl.c_str()); 
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL,strUrl.c_str()); //"https://58.53.185.44/api/task/create?format=xml&password=hbnoc301&username=admin");
//	curl_easy_setopt(curl, CURLOPT_URL, "https://58.53.185.44/api/task/status/90?username=admin&password=hbnoc301");
//	 curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, wrfu);  
   	
 //    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);  
 //    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);  
 		
//#ifdef SKIP_PEER_VERIFICATION
    /*
     * If you want to connect to a site who isn't using a certificate that is
     * signed by one of the certs in the CA bundle you have, you can skip the
     * verification of the server's certificate. This makes the connection
     * A LOT LESS SECURE.
     *
     * If you have a CA cert for the server stored someplace else than in the
     * default bundle, then the CURLOPT_CAPATH option might come handy for
     * you.
     */ 
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
//#endif
 
//#ifdef SKIP_HOSTNAME_VERIFICATION
    /*
     * If the site you're connecting to uses a different host name that what
     * they have mentioned in their server certificate's commonName (or
     * subjectAltName) fields, libcurl will refuse to connect. You can skip
     * this check, but this will make the connection less secure.
     */ 
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 300L);
	curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
//#endif
 //	 curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
  //  curl_easy_setopt(curl, CURLOPT_CERTINFO, 1L);
	//curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1);
	//char response[1024 * 1024] = {0};
	struct MemoryStruct chunk;
	 
	chunk.memory = (char *)malloc(2);  /* will be grown as needed by the realloc above */ 
	chunk.size = 0;    /* no data at this point */ 
	if(bPlus)
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, memory_callback);
	else	
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, memory_callback);
	res = curl_easy_setopt( curl, CURLOPT_WRITEDATA,&chunk);
    /* Perform the request, res will get the return code */ 
    res = curl_easy_perform(curl);
    /* Check for errors */ 
    if(res != CURLE_OK)
    {
	  fprintf(stderr, "curl_easy_perform() failed: %s,%d\n",
              curl_easy_strerror(res),res);
    	  curl_easy_cleanup(curl);
	  curl_global_cleanup();
	  return 0;
    }
    strResponse = chunk.memory;
	if(!bPlus)
    	printf("response:%s\n",strResponse.c_str());
    /* always cleanup */ 
    //curl_easy_cleanup(curl);
  }
  curl_easy_cleanup(curl); 
  /* free slist */ 
  curl_global_cleanup();
  return 1;  
}  
int CurlHttps(std::string strUrl,std::string& strResponse,bool bPlus,std::string strPostString,std::string strToken)  
{  
  CURL *curl;  
  CURLcode res;  
   

  struct curl_slist *headerlist=NULL;  
  //static const char buf[] = "Expect:";  
  
  //curl_global_init(CURL_GLOBAL_ALL);  
 curl_global_init(CURL_GLOBAL_DEFAULT);
 #if 0
  /* Fill in the file upload field */  
  curl_formadd(&formpost,  
               &lastptr,  
               CURLFORM_COPYNAME, "config_xml",  
               CURLFORM_FILE, strPath.c_str(),  
               CURLFORM_END);  
  
  /* Fill in the filename field */  
  curl_formadd(&formpost,  
               &lastptr,  
               CURLFORM_COPYNAME, "type",  
               CURLFORM_COPYCONTENTS, "2",  
               CURLFORM_END);  
  
  /* Fill in the submit field too, even if this is rarely needed */  
  /*curl_formadd(&formpost,  
               &lastptr,  
               CURLFORM_COPYNAME, "submit",  
               CURLFORM_COPYCONTENTS, "Submit",  
               CURLFORM_END);  */
 #endif
  curl = curl_easy_init();  
  /* initalize custom header list (stating that Expect: 100-continue is not 
     wanted */  
 	
  headerlist = curl_slist_append(headerlist, "Content-Type: application/json");  
  headerlist = curl_slist_append(headerlist, "Accept: application/json;version=1.0");
  if(strToken.length() != 0)
  {
	 string temp = "X-Auth-Token " + strToken;
	 headerlist = curl_slist_append(headerlist, temp.c_str());
  }
  
  myLog(INFO_LOG_LEVEL,g_p_defaultLog,"Url:[%s];poststr:[%s],header:[%s]",strUrl.c_str(),strPostString.c_str(),headerlist->data); 
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL,strUrl.c_str()); //"https://58.53.185.44/api/task/create?format=xml&password=hbnoc301&username=admin");

	/*--------------------post参数------------------*/
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);  
	if(strPostString.length() > 0)
	{
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPostString);  //参数内容
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strPostString.length());
	}
//#ifdef SKIP_PEER_VERIFICATION
    /*
     * If you want to connect to a site who isn't using a certificate that is
     * signed by one of the certs in the CA bundle you have, you can skip the
     * verification of the server's certificate. This makes the connection
     * A LOT LESS SECURE.
     *
     * If you have a CA cert for the server stored someplace else than in the
     * default bundle, then the CURLOPT_CAPATH option might come handy for
     * you.
     */ 
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
//#endif
 
//#ifdef SKIP_HOSTNAME_VERIFICATION
    /*
     * If the site you're connecting to uses a different host name that what
     * they have mentioned in their server certificate's commonName (or
     * subjectAltName) fields, libcurl will refuse to connect. You can skip
     * this check, but this will make the connection less secure.
     */ 
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
//#endif
	/***********************使用ssl或者tls************************/
	curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);   

  //curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
  //curl_easy_setopt(curl, CURLOPT_CERTINFO, 1L);
  //curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1);
	char response[1024 * 1024] = {0};
	if(bPlus)
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, memory_callback_plus);
	else	
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, memory_callback);
	res = curl_easy_setopt( curl, CURLOPT_WRITEDATA,&response);
    /* Perform the request, res will get the return code */ 
    res = curl_easy_perform(curl);
    /* Check for errors */ 
    if(res != CURLE_OK)
    {
		myLog(ERROR_LOG_LEVEL, g_p_defaultLog,"curl_easy_perform() failed: %s,%d\n",
              curl_easy_strerror(res),res);
      curl_easy_cleanup(curl);
	  curl_slist_free_all(headerlist);
	  curl_global_cleanup();
	  return 0;
    }
    strResponse = response;
    printf("response:%s\n",response);
	curl_easy_cleanup(curl); 
	 /* free slist */ 
	curl_slist_free_all(headerlist);
	curl_global_cleanup();
	return 1;
  }  //if (curl)
  else{
  curl_easy_cleanup(curl); 
  /* free slist */ 
  curl_slist_free_all(headerlist);
  curl_global_cleanup();
  return 0;  
  }
}  
std::string CurlHttpsPost(std::string strUrl,bool bPlus,std::string strPostString,std::string strToken)  
{  
  CURL *curl;  
  CURLcode res;  
   
  struct curl_slist *headerlist=NULL;  
 curl_global_init(CURL_GLOBAL_DEFAULT);

  curl = curl_easy_init();  

  if(strPostString.length() > 0)
  {
	  char tmp[100]={0};
	  sprintf(tmp,"Content-Length:%d",strPostString.length());
	  headerlist = curl_slist_append(headerlist, tmp);  
  }
  headerlist = curl_slist_append(headerlist, "Accept: application/json;");
   //  curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);  
 	if(strToken.length() != 0)
  {
	 string temp = "X-Cookie: token=" + strToken ;
	 headerlist = curl_slist_append(headerlist, temp.c_str());
  }	
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL,strUrl.c_str()); //"https://58.53.185.44/api/task/create?format=xml&password=hbnoc301&username=admin");
		if((int)strPostString.length() > 0)
		{
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPostString.c_str());  //参数内容
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strPostString.length());
		}
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
 
//#ifdef SKIP_PEER_VERIFICATION
    /*
     * If you want to connect to a site who isn't using a certificate that is
     * signed by one of the certs in the CA bundle you have, you can skip the
     * verification of the server's certificate. This makes the connection
     * A LOT LESS SECURE.
     *
     * If you have a CA cert for the server stored someplace else than in the
     * default bundle, then the CURLOPT_CAPATH option might come handy for
     * you.
     */ 
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
//#endif
 
//#ifdef SKIP_HOSTNAME_VERIFICATION
    /*
     * If the site you're connecting to uses a different host name that what
     * they have mentioned in their server certificate's commonName (or
     * subjectAltName) fields, libcurl will refuse to connect. You can skip
     * this check, but this will make the connection less secure.
     */ 
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);
	curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);

	//curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1);
	struct MemoryStruct chunk;
	 
	chunk.memory = (char *)calloc(1,2);  /* will be grown as needed by the realloc above */ 
	chunk.size = 0;    /* no data at this point */ 
	if(bPlus)
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, memory_callback);
	else	
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, memory_callback);
	res = curl_easy_setopt( curl, CURLOPT_WRITEDATA,&chunk);
    /* Perform the request, res will get the return code */ 
    res = curl_easy_perform(curl);
    /* Check for errors */ 
    std::string strResponse;
    if(res != CURLE_OK)
    {
		printf("curl_easy_perform() failed: %s,%d\n",
              curl_easy_strerror(res),res);
      curl_easy_cleanup(curl);
	  curl_slist_free_all(headerlist);
	  curl_global_cleanup();
	  return strResponse;
    }
    strResponse = chunk.memory;
    printf("response:%s\n",strResponse.c_str());
	curl_easy_cleanup(curl); 
	 /* free slist */ 
	curl_slist_free_all(headerlist);
	curl_global_cleanup();
	return strResponse;
  }  //if (curl)
  else{
  curl_easy_cleanup(curl); 
  /* free slist */ 
  curl_slist_free_all(headerlist);
  curl_global_cleanup();
  return "";  
  }
}  

int CurlHttpsWithFile(std::string strUrl,std::string strPath,std::string& strResponse,std::string& strUser,std::string& strPwd)  
{  
  CURL *curl;  
  CURLcode res;  
  
  struct curl_httppost *formpost=NULL;  
  struct curl_httppost *lastptr=NULL;  
  struct curl_slist *headerlist=NULL;  
  static const char buf[] = "Expect:";  
  
  //curl_global_init(CURL_GLOBAL_ALL);  
 curl_global_init(CURL_GLOBAL_DEFAULT);
  /* Fill in the file upload field */  
  curl_formadd(&formpost,  
               &lastptr,  
               CURLFORM_COPYNAME, "config_xml",  
               CURLFORM_FILE, strPath.c_str(),  
               CURLFORM_END);  
  
  /* Fill in the filename field */  
  curl_formadd(&formpost,  
               &lastptr,  
               CURLFORM_COPYNAME, "type",  
               CURLFORM_COPYCONTENTS, "4",  
               CURLFORM_END);  
  
  /* Fill in the submit field too, even if this is rarely needed */  
  /*curl_formadd(&formpost,  
               &lastptr,  
               CURLFORM_COPYNAME, "username",  
               CURLFORM_COPYCONTENTS, strUser.c_str(),  
               CURLFORM_END);  
   curl_formadd(&formpost,  
               &lastptr,  
               CURLFORM_COPYNAME, "password",  
               CURLFORM_COPYCONTENTS, strPwd.c_str(),  
               CURLFORM_END);  */
  curl = curl_easy_init();  
  /* initalize custom header list (stating that Expect: 100-continue is not 
     wanted */  
  headerlist = curl_slist_append(headerlist, buf);  
  debug_log("Url:%s\nPath:%s\n",strUrl.c_str(),strPath.c_str()); 
  
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL,strUrl.c_str()); //"https://58.53.185.44/api/task/create?format=xml&password=hbnoc301&username=admin");
//	curl_easy_setopt(curl, CURLOPT_URL, "https://58.53.185.44/api/task/status/90?username=admin&password=hbnoc301");
	 //curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, wrfu);  
     curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);  
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);  
 
//#ifdef SKIP_PEER_VERIFICATION
    /*
     * If you want to connect to a site who isn't using a certificate that is
     * signed by one of the certs in the CA bundle you have, you can skip the
     * verification of the server's certificate. This makes the connection
     * A LOT LESS SECURE.
     *
     * If you have a CA cert for the server stored someplace else than in the
     * default bundle, then the CURLOPT_CAPATH option might come handy for
     * you.
     */ 
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
//#endif
 
//#ifdef SKIP_HOSTNAME_VERIFICATION
    /*
     * If the site you're connecting to uses a different host name that what
     * they have mentioned in their server certificate's commonName (or
     * subjectAltName) fields, libcurl will refuse to connect. You can skip
     * this check, but this will make the connection less secure.
     */ 
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
//	curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
//#endif
 //	 curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
  //  curl_easy_setopt(curl, CURLOPT_CERTINFO, 1L);
	curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1);
	//char response[1024 * 1024] = {0};
	struct MemoryStruct chunk;
	 
	chunk.memory = (char *)malloc(1);  /* will be grown as needed by the realloc above */ 
	chunk.size = 0;    /* no data at this point */ 
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, memory_callback);
	res = curl_easy_setopt( curl, CURLOPT_WRITEDATA,&chunk);
    /* Perform the request, res will get the return code */ 
    res = curl_easy_perform(curl);
    /* Check for errors */ 
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s,%d\n",
              curl_easy_strerror(res),res);
    strResponse = chunk.memory;
     fprintf(stderr,"response:%s\n",strResponse.c_str());
    /* always cleanup */ 
    //curl_easy_cleanup(curl);
  }
  curl_formfree(formpost);
  curl_easy_cleanup(curl); 
  /* free slist */ 
  curl_slist_free_all(headerlist);
  curl_global_cleanup();
  return 0;  
}

int CurlHttpsWithForm(std::string strUrl,std::string& strResponse,std::string& strUser,std::string& strPwd)  
{  
  CURL *curl;  
  CURLcode res;  
  
  struct curl_httppost *formpost=NULL;  
  struct curl_httppost *lastptr=NULL;  
  struct curl_slist *headerlist=NULL;  
  static const char buf[] = "Expect:";  
  
  //curl_global_init(CURL_GLOBAL_ALL);  
 curl_global_init(CURL_GLOBAL_DEFAULT);
  /* Fill in the file upload field */  
  curl_formadd(&formpost,  
               &lastptr,  
               CURLFORM_COPYNAME, "username",  
               CURLFORM_FILE, strUser.c_str(),  
               CURLFORM_END);  
  
  /* Fill in the filename field */  
  curl_formadd(&formpost,  
               &lastptr,  
               CURLFORM_COPYNAME, "password",  
               CURLFORM_COPYCONTENTS, strPwd.c_str(),  
               CURLFORM_END);  
  
  /* Fill in the submit field too, even if this is rarely needed */  
  /*curl_formadd(&formpost,  
               &lastptr,  
               CURLFORM_COPYNAME, "username",  
               CURLFORM_COPYCONTENTS, strUser.c_str(),  
               CURLFORM_END);  
   curl_formadd(&formpost,  
               &lastptr,  
               CURLFORM_COPYNAME, "password",  
               CURLFORM_COPYCONTENTS, strPwd.c_str(),  
               CURLFORM_END);  */
  curl = curl_easy_init();  
  /* initalize custom header list (stating that Expect: 100-continue is not 
     wanted */  
  //headerlist = curl_slist_append(headerlist, buf);  
  debug_log("Url:%s",strUrl.c_str()); 
  
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL,strUrl.c_str()); //"https://58.53.185.44/api/task/create?format=xml&password=hbnoc301&username=admin");
//	curl_easy_setopt(curl, CURLOPT_URL, "https://58.53.185.44/api/task/status/90?username=admin&password=hbnoc301");
	 //curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, wrfu);  
     curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);  
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);  
 
//#ifdef SKIP_PEER_VERIFICATION
    /*
     * If you want to connect to a site who isn't using a certificate that is
     * signed by one of the certs in the CA bundle you have, you can skip the
     * verification of the server's certificate. This makes the connection
     * A LOT LESS SECURE.
     *
     * If you have a CA cert for the server stored someplace else than in the
     * default bundle, then the CURLOPT_CAPATH option might come handy for
     * you.
     */ 
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
//#endif
 
//#ifdef SKIP_HOSTNAME_VERIFICATION
    /*
     * If the site you're connecting to uses a different host name that what
     * they have mentioned in their server certificate's commonName (or
     * subjectAltName) fields, libcurl will refuse to connect. You can skip
     * this check, but this will make the connection less secure.
     */ 
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
//	curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
//#endif
 //	 curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
  //  curl_easy_setopt(curl, CURLOPT_CERTINFO, 1L);
	curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1);
	//char response[1024 * 1024] = {0};
	struct MemoryStruct chunk;
	 
	chunk.memory = (char *)malloc(1);  /* will be grown as needed by the realloc above */ 
	chunk.size = 0;    /* no data at this point */ 
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, memory_callback);
	res = curl_easy_setopt( curl, CURLOPT_WRITEDATA,&chunk);
    /* Perform the request, res will get the return code */ 
    res = curl_easy_perform(curl);
    /* Check for errors */ 
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s,%d\n",
              curl_easy_strerror(res),res);
    strResponse = chunk.memory;
     fprintf(stderr,"response:%s\n",strResponse.c_str());
    /* always cleanup */ 
    //curl_easy_cleanup(curl);
  }
  curl_formfree(formpost);
  curl_easy_cleanup(curl); 
  /* free slist */ 
  curl_slist_free_all(headerlist);
  curl_global_cleanup();
  return 0;  
}
#if 0
int main(int argc,char *argv[])
{
	std::string strUrl = "https://172.22.6.30/httpRpc/newtask/?user=admin&password=xjsoc@2016&targets=135.241.108.65;10.60.75.136;135.241.9.14;135.241.9.15;135.241.9.16;135.241.9.17;10.60.74.124;135.241.9.13;135.241.9.18;135.241.11.29;135.241.11.28;135.241.10.9;135.241.10.8;135.241.10.7;135.241.10.6;135.241.10.5;135.241.10.3;135.241.10.2;135.241.10.1;10.60.74.38;10.60.74.37;10.60.74.33;135.241.9.207;10.60.74.154;135.241.141.229;135.241.141.228;135.241.9.89;135.241.9.88;135.241.9.87;135.241.9.86;135.241.9.84;135.241.141.227;135.241.141.226;135.241.9.81;135.241.94.25;10.60.74.122;135.241.9.201;135.241.11.125;10.60.74.88;10.60.74.80;10.60.74.213;135.241.9.116;135.241.9.117;135.241.9.114;135.241.9.1;135.241.9.112;135.241.9.7;135.241.9.110;135.241.9.111;135.241.9.118;135.241.9.119;135.241.67.111;135.241.9.238;135.241.9.233;135.241.9.232;135.241.9.231;135.241.9.230;135.241.9.237;135.241.9.236;13 5.241.9.235;135.241.9.234;135.241.9.50;135.241.9.51;135.241.9.52;135.241.9.53;135.241.9.54;135.241.9.55;135.241.10.19;135.241.10.18;135.241.10.12;135.241.10.11;135.241.10.10;135.241.10.17;135.241.10.16;10.60.75.216;135.241.70.33;135.241.11.112;135.241.11.111;135.241.9.78;10.60.74.117;135.241.108.20;135.241.100.41;135.241.100.40;135.241.100.45;135.241.100.44;135.241.67.112;135.241.67.30;10.60.74.42;10.60.74.49;135.241.9.152;135.241.9.151;135.241.9.156;135.241.9.154;135.241.9.155;10.60.74.182;135.241.67.82;135.241.67.81;10.60.74.239;10.60.74.233;10.60.74.2 32;10.60.74.237;10.60.74.236;135.241.10.250;222.83.31.230;135.241.5.21;135.241.5.20;10.60.74.240;10.60.74.243;135.241.33.10;135.241.100.38;10.60.74.184;135.241.94.1;10.60.74.157;10.60.74.120;135.241.141.225;10.60.74.146;135.241.141.230;135.241.253.4;135.241.253.2;135.241.251.1;135.241.9.123;135.241.9.121;135.241.9.120;135.241.9.127;135.241.9.126;135.241.9.125;135.241.9.124;135.241.9.128;135.241.67.5;135.241.67.6;10.60.75.119;135.241.9.224;135.241.9.225;10.60.74.202;135.241.9. 227;135.241.9.220;135.241.9.222;135.241.9.223;135.241.1.10;135.241.9.48;135.241. 9.47;135.241.9.46;135.241.9.44;135.241.106.65;135.241.106.67;135.241.106.60;135. 241.106.63;135.241.70.21;135.241.70.23;135.241.106.69;135.241.11.101;135.241.11. 104;135.241.11.106;10.60.74.109;10.60.74.108;10.60.74.102;10.60.74.101;10.60.74. 100;10.60.74.105;135.241.11.190;135.241.11.8;135.241.11.2;135.241.11.3;135.241.1 1.1;135.241.11.6;135.241.11.4;10.60.74.58;135.241.67.21;135.241.67.23;10.60.74.5 7;135.241.22.18;135.241.22.12;135.241.22.13;135.241.22.11;135.241.22.17;135.241. 22.14;135.241.22.15;135.241.9.161;135.241.9.160;135.241.9.168;135.241.9.189;135. 241.9.188;135.241.9.185;135.241.9.184;135.241.9.187;135.241.9.186;135.241.9.181; 135.241.9.180;135.241.9.182;135.241.9.2;135.241.33.2;135.241.33.3;135.241.9.115; 135.241.9.245;135.241.33.4;135.241.9.113;135.241.70.17;135.241.70.16;10.60.75.12 8;10.60.75.123;135.241.5.189;135.241.9.218;135.241.9.215;135.241.9.214;135.241.9 .216;135.241.9.210;135.241.9.213;135.241.9.33;135.241.9.30;135.241.9.31;135.241. 9.36;135.241.9.37;135.241.9.34;135.241.9.39;135.241.10.35;135.241.10.34;135.241. 10.36;135.241.10.31;135.241.10.30;135.241.10.33;135.241.10.32;10.60.74.12;10.60. 74.172;10.60.74.171;10.60.74.175;135.241.100.28;135.241.100.24;135.241.100.23;10 .60.74.61;10.60.74.60;135.241.9.139;135.241.9.136;10.60.75.96;135.241.9.131;135. 241.9.133;135.241.67.68;135.241.67.60;135.241.67.61;135.241.67.62;135.241.67.63; 135.241.67.64;135.241.67.65;135.241.5.90;135.241.5.91;135.241.5.93;135.241.5.95; 135.241.5.96;135.241.5.97;135.241.9.251;10.60.74.212;135.241.9.253;10.60.74.217; 10.60.74.214;10.60.74.218;135.241.9.76;135.241.9.77;135.241.9.74;135.241.9.75;13 5.241.9.72;135.241.9.73;135.241.9.70;135.241.114.8;135.241.114.7;135.241.114.6;1 35.241.114.5;135.241.114.4;135.241.114.3;135.241.114.2;135.241.114.1;135.241.9.7 9;135.241.100.16;135.241.100.12;135.241.100.10;135.241.33.33;10.60.74.132;135.25 5.224.11;10.60.74.139;135.241.100.60;10.60.74.25;10.60.74.21;10.60.74.20;135.241 .91.2;135.241.22.26;135.241.22.25;135.241.22.23;135.241.22.22;135.241.22.21;135. 241.22.20;10.60.74.4;135.241.9.98;135.241.9.99;135.241.9.94;135.241.9.97;135.241 .9.90;135.241.9.93;135.241.9.197;135.241.9.194;135.241.9.195;135.241.9.190;135.2 41.9.191;135.241.1.30;135.241.9.106;135.251.5.91;135.241.9.103;135.241.9.102;135 .241.9.109;135.241.9.108;135.241.93.55;10.60.75.130;135.241.100.8;135.241.100.9; 135.241.9.208;135.241.9.206;135.241.9.101;135.241.9.204;135.241.9.205;135.241.9. 202;135.241.9.200;222.83.31.234;135.241.9.29;135.241.9.25;135.241.9.24;135.241.9 .27;135.241.9.26;135.241.9.20;135.241.9.22;135.241.108.27;135.241.10.28;135.241. 10.29;135.241.10.27;135.241.10.24;135.241.10.25;135.241.10.22;135.241.10.20;135. 241.33.42;135.241.33.41;135.241.33.40;135.241.33.44;10.60.74.167;10.60.74.166;10 .60.74.160;10.60.74.163;135.226.109.23;135.226.109.22;135.226.109.27;135.226.109 .26;135.226.109.25;135.226.109.24;135.226.109.29;135.226.109.28;135.241.100.51;1 35.241.100.56;10.60.74.74;135.241.5.180;135.241.5.187;135.241.5.186;135.241.9.14 1;135.241.9.143;135.241.9.145;135.241.9.147;135.241.9.146;10.60.74.194;135.241.2 2.1;135.241.22.2;135.241.22.3;135.241.22.4;135.241.22.5;135.241.22.6;135.241.22. 7;135.241.22.8;135.241.22.9;135.241.5.82;135.241.9.242;135.241.9.243;135.241.9.2 41;135.241.9.246;135.241.9.247;135.241.9.244;10.60.74.3;135.241.9.248;135.241.9. 61;135.241.9.63;135.241.9.62;135.241.9.65;135.241.9.64;135.241.9.67;135.241.9.66 ;135.241.9.69;135.241.70.3;135.241.70.2;135.241.108.66;135.241.106.44 &templateid=0";//https://172.22.6.30/api/task/create?&format=xml&username=admin&password=xjsoc@2016";	
	size_t front = strUrl.find(" ");
	while(front != string::npos)
	{
		strUrl.replace(front,1,"");	
		front = strUrl.find(" ");
	}
	printf("url:%s\n",strUrl.c_str());
	std::string strResponse;
	//std::string strPath = argv[1];
	std::string strUser,strPwd;
	//CurlHttpsWithFile(strUrl,strPath,strResponse,strUser,strPwd);
	CurlHttps(strUrl,strResponse,true);
	printf("response:%s\n",strResponse.c_str());
	CurlHttpsPost(strUrl,strResponse,true);
	printf("response:%s\n",strResponse.c_str());
	/*strUrl="";
	strUrl = "https://172.22.6.30/api/task/status/17?&format=xml&username=admin&password=xjsoc@2016";
	CurlHttps(strUrl,strResponse,true);
	printf("response:%s\n",strResponse.c_str());
	strUrl = "https://172.22.6.30/api/report/task/17?&format=xml&username=admin&password=xjsoc@2016";
	CurlHttps(strUrl,strResponse,false);
	printf("response:%s\n",strResponse.c_str());*/
	return 0;
}
#endif

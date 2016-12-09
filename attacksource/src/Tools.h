#ifndef _TOOLS_H_
#define _TOOLS_H_
using namespace std;

int TimeConver(const char *Time,char *StaTime,int Length);
long int CountTime(char* startTime,char* endTime);
int code_convert(char *from_code, char *to_code, char *from, char *to);
int ChangeDateTimeFormat(char *srcTime,char *DstTime);
int GetStrValue(const char *key,char *value,const char *buff);
std::string GetSystemTime();
string GetSystemDay();
std::string getKeyFromString(std::string _Resource,std::string _key,std::string _keyLeft,std::string _keyEnd);
int trave_dir(char* path, int depth,char cArray[][256]);
bool HTTPSGet(const string& url,std::string &response);
int SendSysLog(std::string AdsIP,char *Log,int iLen);
std::string GreenToBeijing(char *time);
std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
#endif

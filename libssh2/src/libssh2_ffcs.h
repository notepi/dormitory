
#ifndef LIB_SSH2_FFCS_H
#define LIB_SSH2_FFCS_H
//#include "libssh2_config.h"
#include <libssh2.h>
#include <libssh2_sftp.h>
#include <string>
#include <list>

int ssh_scp(char* hostname,char* username,char* password,
	char* scppath,char* filename,char* destpath);


int sbs_ssh_sftp_open(char* hostname,char* username,char* password,
	LIBSSH2_SESSION **psession,int *psock);
int sbs_ssh_sftp(LIBSSH2_SESSION *session,char* scppath,char* filename,char* destpath);
void sbs_ssh_sftp_close(LIBSSH2_SESSION *session,int sock);

//dragon 用到的函数 begin
int ssh_exec(std::string& r_strResult, const char* hostname, const char* username,
			 const char* password, const char* commandline);

int sbs_ssh_exec_open(const char* hostname,
	const char* username, const char* password,LIBSSH2_SESSION **psession,int* psock);

int sbs_ssh_exec(std::string& r_strResult, LIBSSH2_SESSION *session,int sock, const char* commandline);

void sbs_ssh_exec_close(LIBSSH2_SESSION *session,int sock);

int ssh_sftp_download_open(char* hostname,char* username,char* password,
	LIBSSH2_SESSION **psession,int *psock);
int ssh_sftp_download(LIBSSH2_SESSION *session,char* scppath,char* filename,char* destpath);
void ssh_sftp_download_close(LIBSSH2_SESSION *session,int sock);

int listRemoteFileName(std::list<std::string>& r_fileName, const char* p_ip, const char* p_userName, 
					   const char* p_password,const char* p_dir);
//dragon 用到的函数 end
#endif




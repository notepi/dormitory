#include <iostream>
#include "libssh2_ffcs.h"

using namespace std;
			 
int main()
{
	
	string r_strResult;
	const char *hostname = "127.0.0.1";
	const char *username = "root";
	const char* password = "admin";
	const char* commandline = "uptime";
	
	ssh_exec(r_strResult, hostname, username, password, commandline);
	cout << r_strResult << endl;
	
	
	
}

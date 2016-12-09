#include<iostream>
#include <string>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "command.h"

using namespace std;
                                                                                                                                                                                                                                                                                                                                                                        
int main()
{
	Process_Info ocj("6788", "./a.txt");
	ocj.Info_all();
	cout << ocj.command << endl;
	return 0;
}

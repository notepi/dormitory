#include <string>
#include <vector>
#include <iostream>

using namespace::std;
int main()
{
	vector < string > buff;
	buff.push_back("hello world!");
	buff.push_back("hello");
	cout << buff[0] << endl;
	cout << *++buff << endl;
}

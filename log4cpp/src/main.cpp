#include "log4cpp/Category.hh"
#include "log4cpp/PropertyConfigurator.hh"

#include <stdlib.h>
int main(int argc, char* argv[])
{
	system("pwd");
    //��ȡ���������ļ�
    log4cpp::PropertyConfigurator::configure("../conf/log4cpp.conf");
    //ʵ����category����
    // log4cpp::Category* root = &log4cpp::Category::getRoot();
    log4cpp::Category& root = log4cpp::Category::getRoot();
    log4cpp::Category& main = log4cpp::Category::getInstance(std::string("MAIN"));

    //ʹ��category���������־����
    main.debug("This is debug");
    main.info("This is info");
    main.alert("This is alert");

    return 0;
}
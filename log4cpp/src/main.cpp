#include "log4cpp/Category.hh"
#include "log4cpp/PropertyConfigurator.hh"

#include <stdlib.h>
int main(int argc, char* argv[])
{
	system("pwd");
    //读取解析配置文件
    log4cpp::PropertyConfigurator::configure("../conf/log4cpp.conf");
    //实例化category对象
    // log4cpp::Category* root = &log4cpp::Category::getRoot();
    log4cpp::Category& root = log4cpp::Category::getRoot();
    log4cpp::Category& main = log4cpp::Category::getInstance(std::string("MAIN"));

    //使用category对象进行日志处理
    main.debug("This is debug");
    main.info("This is info");
    main.alert("This is alert");

    return 0;
}
# 生成时间
BUILD_TIME := '$(shell date '+%Y-%m-%d %T')'



#编译工具变量
CPP = g++
CC=gcc
LD = g++
RM = rm -f
NULL = /dev/null

#编译参数变量
CFLAGS = -c -g -w 
ASFLAGS = -c
SYSLIBS=
LDFLAGS = 
OPT_NORMAL = -funroll-loops 							# -funroll-loops编译选项使得程序中的循环步骤完全展开。
														# 这样会增加汇编代码的长度，并且攻击者需要隔离出每个循环，进行单独的修改，从而增大了攻击难度
OPT_CPP = -std=c++11									# cpp编译
PROJ = ../bin/sys										# 编译生成文件

# 文件依赖
PROJ_SOURCE   =$(wildcard *.cpp *.c)					# 查找目录下的所有.cpp和.c文件并记录文件名
PROJ_CPPOBJECT=$(PROJ_SOURCE:%.cpp=%.o)					# 变量中的.cpp文件名换成.o存在变量中
PROJ_OBJECT =$(PROJ_CPPOBJECT:%.c=%.o)					# 变量中的.c文件名换成.o存在变量中
PROJ_OBJS=$(PROJ_OBJECT)								# 

.PHONY : all

all : $(PROJ)

$(PROJ) : $(PROJ_OBJS)
	$(LD) $(LDFLAGS) $(PROJ_OBJS) $(SYSLIBS) -o $(PROJ)	
	@echo "$(BUILD_TIME)"								# 打印编译时间
	
.c.o:													# 查找所以.c编译生成.o
	@$(CC)  $(CFLAGS) $(OPT_NORMAL) -o $@ -c $<

.cpp.o:													# 查找所有.cpp编译生成.o
	@$(CPP) $(CFLAGS) $(OPT_NORMAL) $(OPT_CPP) -o $@ -c $<

clean:
	$(RM) *.o PROJ
	
	

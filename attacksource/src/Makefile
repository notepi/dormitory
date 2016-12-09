#
# This file is part of John the Ripper password cracker,
# Copyright (c) 1996-2003 by Solar Designer
#
# Modified  by dengwt 20090106  
# Make John The Ripper be incorporate in The project 
#
# 将VERSION变量传递到VersionMake.mk中
 
# 根据version.txt生成新的版本号，保存在VERSION变量中

# 生成时间
BUILD_TIME := '$(shell date '+%Y-%m-%d %T')'

CPP = g++
CC = gcc
AS = gcc
LD = g++

CP = cp
LN = ln -sf
RM = rm -f
TR = tr
SED = sed
NULL = /dev/null
CPPFLAGS = -E

CFLAGS = -c -g -w -fomit-frame-pointer \
	-I/opt/instantclient_10_2/sdk/include -I/usr/local/include -I/usr/include/oracle/11.2/client64 -I/home/dragon/opensource/boost_1_53_0 \
	-I$(MY_COMMON_LIB)/src -I/usr/include -I/home/dragon/opensource/SimpleAmqpClient-master/SimpleAmqpClient/src/ -I$(BOOST_INCLUDE_DIR)\
	-Wl,/opt/soc_flow/dll/ \
	 
ASFLAGS = -c
LDFLAGS = 
OPT_NORMAL = -funroll-loops 
OPT_INLINE = -finline-functions
SYSLIBS =-L/usr/local/lib/ -lrabbitmq -L/usr/lib/oracle/11.2/client64/lib -lclntsh -lpthread -locci -L/usr/lib64 -lssl -liconv -L../dll/ -lSimpleAmqpClient\
-L$(BOOST_LIB_DIR) -l boost_system -l boost_thread -l boost_filesystem -l boost_date_time  -l boost_regex \
-llog4cplus -lcurl


PROJ = ../bin/flow_for_attacksource
TARGET=$(PROJ)V$(VERSION)
BUILD_TARGET=$(PROJ)V$(LAST_VERSION)

.PHONY : all
PROJ_SOURCE   =$(wildcard *.cpp *.c)
PROJ_CPPOBJECT=$(PROJ_SOURCE:%.cpp=%.o)
PROJ_OBJECT =$(PROJ_CPPOBJECT:%.c=%.o)
PROJ_OBJS=$(PROJ_OBJECT)

.c.o:
	@$(CC)  $(CFLAGS) $(OPT_NORMAL) -o $@ -c $<

.cpp.o:
	@$(CPP) $(CFLAGS) $(OPT_NORMAL) -o $@ -c $<
all : $(PROJ)
  
$(PROJ) : $(PROJ_OBJS)
	$(LD) $(LDFLAGS) $(PROJ_OBJS) $(SYSLIBS) -o $(PROJ)	
#	@echo "$(VERSION)" > ../version/version.txt #输出新版本号到version.txt
#	chmod a+x $(PROJ)
	@echo "$(BUILD_TIME)"
	@echo "$(LAST_VERSION)"
	

.PHONY : build

build : $(TARGET)

$(TARGET) : $(OBJS)
	$(LD) $(LDFLAGS) $(PROJ_OBJS) $(SYSLIBS) -o $(TARGET)	
#	chmod a+x $(BUILD_TARGET)
	mv $(TARGET) $(PROJ)
		
#绝对路
#PROJ_OBJS=$(PROJ_OBJECT)

#PROJ = ../bin/nsssvr

#$(PROJ): $(PROJ_OBJS)
#	$(LD) $(LDFLAGS) $(PROJ_OBJS) $(SYSLIBS) -o $(PROJ)



# Inlining the S-boxes produces faster code, as long as they fit in the cache
# (that is, on RISC with at least 8 KB of L1 code cache).

#.c.o:
#	@$(CC)  $(CFLAGS)  $(OPT_NORMAL) -o $@ -c $<

#.cpp.o:
#	@$(CPP) $(CFLAGS)  $(OPT_NORMAL) -o $@ -c $<


depend:
	makedepend -fMakefile.dep -Y *.c 2>> /dev/null

clean:
	$(RM) $(PROJ) $(PROJ_DOS) $(PROJ_WIN32)
	$(RM)  *.o *.bak 

	$(CP) $(NULL) Makefile.dep

include Makefile.dep

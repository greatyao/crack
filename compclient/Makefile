OS_NAME = $(shell uname -o)
LC_OS_NAME = $(shell echo $(OS_NAME) | tr '[A-Z]' '[a-z]')

MC_NAME = $(shell uname -m)
LC_MACH_NAME = $(shell echo $(MC_NAME) | tr '[A-Z]' '[a-z]')

ifeq ($(LC_MACH_NAME), i686)
ARCH = x86
else
ARCH = x86_64
endif

ifeq ($(LC_OS_NAME), cygwin)
OCLLIBS = -L ../libs/cygwin/$(ARCH)/ -lOpenCL
else
OCLLIBS = -L ../libs/linux/$(ARCH)/ -lOpenCL
endif


SRCS+= Crack.cpp
SRCS+= HashKill.cpp
SRCS+= launcher.cpp
SRCS+= resourceslotpool.cpp
SRCS+= coordinator.cpp
SRCS+= main.cpp
 
VPATH+= ../common/
SRCS+= CLog.cpp
 
CPPFLAGS+= -D_XOPEN_SOURCE -DTEST
CPPFLAGS+= -fPIC
CPPFLAGS+= -I.
CPPFLAGS+= -I../common/include
CPPFLAGS+= -I../common
CPPFLAGS+= -O2 -g -Wall -Wno-unused-parameter
LIBS+= -lrt -lpthread -lcrypto -ldl
LIBS+= $(OCLLIBS)
 
#TOOLCHAIN= arm-linux-
TOOLCHAIN= 
CC= $(TOOLCHAIN)g++
LD= $(TOOLCHAIN)g++

OBJS= $(SRCS:.cpp=.o)
 
all: compclient 
 
compclient: $(OBJS)
	$(LD) -o $@ $(LDFLAGS) $(OBJS) $(LIBS)
	mv compclient ../bin/$(ARCH)/

clean:
	rm -rf $(OBJS)

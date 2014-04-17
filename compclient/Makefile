MKDIR_P = mkdir -p

OS_NAME = $(shell uname -o)
LC_OS_NAME = $(shell echo $(OS_NAME) | tr '[A-Z]' '[a-z]')

MC_NAME = $(shell uname -m)
LC_MACH_NAME = $(shell echo $(MC_NAME) | tr '[A-Z]' '[a-z]')

ifeq ($(LC_MACH_NAME), i686)
ARCH = x86
else
ARCH = x86_64
CPPFLAGS+=-DX64
endif

ifeq ($(LC_OS_NAME), cygwin)
OCLLIBS = -L ../libs/cygwin/$(ARCH)/ -lOpenCL
else
OCLLIBS = -L ../libs/linux/$(ARCH)/ -lOpenCL
endif


SRCS+= main.cpp
SRCS+= launcher.cpp
SRCS+= resourceslotpool.cpp
SRCS+= coordinator.cpp
SRCS+= oclHashcat.cpp
SRCS+= Crack.cpp
SRCS+= CrackManager.cpp
SRCS+= HashKill.cpp
SRCS+= Client.cpp
SRCS+= Config.cpp
 
VPATH+= ../common/
SRCS+= CLog.cpp

VPATH+= ../plugins/
SRCS+= $(shell find ../plugins/ -name "*.cpp")
#SRCS+= plugin.cpp
#SRCS+= loadfiles.cpp
#SRCS+= md5.cpp

 
CPPFLAGS+= -D_XOPEN_SOURCE -D_GNU_SOURCE -DTEST
CPPFLAGS+= -fPIC
CPPFLAGS+= -I.
CPPFLAGS+= -I../common/include
CPPFLAGS+= -I../common
CPPFLAGS+= -I../plugins
CPPFLAGS+= -O2 -g -Wall -Wno-unused-parameter
LIBS+= -lpthread -ldl -lz
LIBS+= $(OCLLIBS)
 
#TOOLCHAIN= arm-linux-
TOOLCHAIN= 
CC= $(TOOLCHAIN)g++
LD= $(TOOLCHAIN)g++

OBJS= $(SRCS:.cpp=.o)
 
all: compclient 
 
compclient: $(OBJS)
	$(LD) -o $@ $(LDFLAGS) $(OBJS) $(LIBS)
	test -z "../bin/$(ARCH)" || { \
	  echo " $(MKDIR_P) '../bin/$(ARCH)'"; \
	  $(MKDIR_P) "../bin/$(ARCH)"; \
	}
	mv compclient ../bin/$(ARCH)/

clean:
	rm -rf $(OBJS)

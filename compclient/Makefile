MKDIR_P = mkdir -p
INSTALL = install
DEST_DIR = /usr/local/trimps

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
	
install:	
	test -z "$(DEST_DIR)" || { \
	  $(MKDIR_P) "$(DEST_DIR)"; \
	}
	$(INSTALL) -m 777 ../bin/$(ARCH)/compclient $(DEST_DIR)
	$(INSTALL) -m 644 example.conf $(DEST_DIR)/compclient.conf
	$(INSTALL) -m 777 compclient.sh /etc/init.d/
	update-rc.d compclient defaults 98 0 1 2
	update-rc.d compclient start 98 0 1 2
	
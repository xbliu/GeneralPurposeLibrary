TOPDIR := $(shell /bin/pwd)

CROSS_COMPILE ?= #$(patsubst %g++,%,$(TARGET_CXX))

#
# Include the make variables (CC, etc...)
#

AS              := $(CROSS_COMPILE)as
LD              := $(CROSS_COMPILE)ld
CC              := $(CROSS_COMPILE)gcc
CXX             := $(CROSS_COMPILE)g++ 
AR              := $(CROSS_COMPILE)ar
NM              := $(CROSS_COMPILE)nm
STRIP           := $(CROSS_COMPILE)strip
OBJCOPY         := $(CROSS_COMPILE)objcopy
OBJDUMP         := $(CROSS_COMPILE)objdump

COMMON_FLAGS=-Wall -g -rdynamic -Wl,--as-needed -I$(TOPDIR) -I$(TOPDIR)/..
CFLAGS := $(COMMON_FLAGS) 
CXXFLAGS := $(COMMON_FLAGS) -std=gnu++11
LDFLAGS := -lpthread
SNIFFER_TEST_OBJS= sniffer_framework.o  sniffer_test.o $(TOPDIR)/../pthread/pthread_model.o 
UDP_CLIENT_TEST_OBJS = udp_client.o  udp_test.o 


all: sniffer_test udp_client_test
	echo "#######compile over#######"

sniffer_test:$(SNIFFER_TEST_OBJS)
	$(CC) $(CFLAGS) $(SNIFFER_TEST_OBJS) -o $@ $(LDFLAGS)

udp_client_test:$(UDP_CLIENT_TEST_OBJS)
	$(CC) $(CFLAGS) $(UDP_CLIENT_TEST_OBJS) -o $@ $(LDFLAGS)
	
clean:
	find . \( -name '*.o' \) -type f -print \
	| grep -v lxdialog/ | xargs rm -f
	rm -rf sniffer_test udp_client_test $(TOPDIR)/../pthread/pthread_model.o
	
%.o:%.c
	$(CC) $(CFLAGS) $(EX_FLAGS) -c -o $@ $<

%.o:%.cpp
	$(CXX) $(CXXFLAGS) $(EX_FLAGS) -c -o $@ $<

%.o:%.cc
	$(CXX) $(CXXFLAGS) $(EX_FLAGS) -c -o $@ $<



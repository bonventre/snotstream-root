CC			= g++
CFLAGS  = -g 
LFLAGS  = -L/usr/local/lib -L$(ROOTSYS)/lib -L$(AVALANCHE) -L$(RATROOT)/lib
INCLUDE = -I$(ROOTSYS)/include -I$(AVALANCHE) -I$(RATROOT)/include

LIBS	  = -lzmq -lavalanche -ljson_linux-gcc-4.4.3_libmt -lRATEvent_Linux-g++ 

SRCS		= main.C mainFrame.C
HDRS		= $(SRCS:.C=.h)
OBJS		= $(SRCS:.C=.o) dict.o

all:		main

main:		dict
	$(CC) -o $@ main.C mainFrame.C dict.C $(INCLUDE) $(CFLAGS) $(LFLAGS) $(LIBS) `root-config --cflags --glibs` 

dict:
	rm -f dict.*
	rootcint dict.C -c mainFrame.h

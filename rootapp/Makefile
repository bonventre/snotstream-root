CC			= g++
CFLAGS  = -g 
LFLAGS  = -L/usr/local/lib -L$(ROOTSYS)/lib -L$(AVALANCHE) -L$(RATROOT)/lib
INCLUDE = -I$(ROOTSYS)/include -I$(AVALANCHE) -I$(RATROOT)/include

LIBS	  = -lzmq -lavalanche -ljson_linux-gcc-4.4.3_libmt -lRATEvent_Linux-g++ 

SRCS		= RootApp.cpp Plot.cpp
HDRS		= $(SRCS:.cpp=.h)
OBJS		= $(SRCS:.cpp=.o) dict.o

all:		RootApp

RootApp:		dict
	$(CC) -o $@ $(SRCS) $(INCLUDE) $(CFLAGS) $(LFLAGS) $(LIBS) `root-config --cflags --glibs` 

dict:
	rm -f dict.*
	rootcint dict.C -c RootApp.h Plot.h

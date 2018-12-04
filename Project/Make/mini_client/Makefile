#
#	Makefile of Embedded-MQTT-Client-Library Sample Program
#	mini_client
#

TOP				= ../../../

OUTPUTDIR		= $(TOP)Project/Make/Output/

GCC_CFLAGS		= 

DEBUG			= 

ifeq ($(PLATFORM), LINUX) 
INCLUDES		= -I$(TOP)MQTTClient/interface -I$(TOP)Platform/Linux
SOURCES_M		= $(TOP)Sample/mini_client.c $(TOP)Platform/Linux/wrapper.c
CFLAGS			= -fPIC -Wall -std=c99 -fno-strict-aliasing \
					$(INCLUDES) -O2 $(GCC_CFLAGS) $(DEBUG) -DPLATFORM_LINUX
else
ifeq ($(PLATFORM), WINDOWS)
INCLUDES		= -I$(TOP)MQTTClient/interface -I$(TOP)Platform/Windows
CFLAGS			= -fPIC -Wall -std=c99 -fno-strict-aliasing \
					$(INCLUDES) -O2 $(GCC_CFLAGS) $(DEBUG) -DPLATFORM_WINDOWS
else
INCLUDES		= -I$(TOP)MQTTClient/interface -I$(TOP)Platform/Embedded
CFLAGS			= -fPIC -Wall -std=c99 -fno-strict-aliasing \
					$(INCLUDES) -O2 $(GCC_CFLAGS) $(DEBUG) -DPLATFORM_OTHER
endif
endif

OBJS_M			= mini_client.o wrapper.o 

MAKEFILE 		= Makefile

CC				?= gcc

AR				?= ar

SOLIBS			= -lMqc -lCCommon

SOLIBDIR		= -L$(OUTPUTDIR)lib

#
# Compile Menu
#

.PHONY			:	all mini_client cleanmini_client clean

all				:	mini_client

clean			:	cleanmini_client

mini_client		:	$(OBJS_M)
	$(CC) -o mini_client $(OBJS_M) $(SOLIBS) $(SOLIBDIR)
	if [ ! -d $(OUTPUTDIR)test ]; then mkdir -p $(OUTPUTDIR)test; fi;
	cp -rfp mini_client $(OUTPUTDIR)test

$(OBJS_M) 		:	$(SOURCES_M)
	$(CC) $(CFLAGS) -c $(SOURCES_M)
    
cleanmini_client:
	rm -f *.o *.Z* *~ mini_client
	rm -f $(OUTPUTDIR)test/mini_client 

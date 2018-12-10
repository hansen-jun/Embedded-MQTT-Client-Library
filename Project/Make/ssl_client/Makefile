#
#	Makefile of Embedded-MQTT-Client-Library Sample Program
#	ssl_client
#

TOP				= ../../../

OUTPUTDIR		= $(TOP)Project/Make/Output/

GCC_CFLAGS		= 

DEBUG			= 

ifeq ($(PLATFORM), LINUX) 
INCLUDES		= -I$(TOP)MQTTClient/interface -I$(TOP)Platform/Linux -I$(TOP)Platform/Mbedtls
SOURCES_M		= $(TOP)Sample/ssl_client.c $(TOP)Platform/Linux/wrapper.c $(TOP)Platform/Mbedtls/wrapper_ssl.c
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

OBJS_M			= ssl_client.o wrapper.o wrapper_ssl.o

MAKEFILE 		= Makefile

CC				?= gcc

AR				?= ar

SOLIBS			= -lMqc -lCCommon -lmbedtls -lmbedcrypto -lmbedx509

SOLIBDIR		= -L$(OUTPUTDIR)lib

#
# Compile Menu
#

.PHONY			:	all ssl_client cleanssl_client clean

all				:	ssl_client

clean			:	cleanssl_client

ssl_client		:	$(OBJS_M)
	$(CC) -o ssl_client $(OBJS_M) $(SOLIBS) $(SOLIBDIR)
	if [ ! -d $(OUTPUTDIR)test ]; then mkdir -p $(OUTPUTDIR)test; fi;
	cp -rfp ssl_client $(OUTPUTDIR)test

$(OBJS_M) 		:	$(SOURCES_M)
	$(CC) $(CFLAGS) -c $(SOURCES_M)
    
cleanssl_client	:
	rm -f *.o *.Z* *~ ssl_client
	rm -f $(OUTPUTDIR)test/ssl_client 

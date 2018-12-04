#
#	Makefile of Embedded-MQTT-Client-Library
#	Mqc Library
#

TOP			= ../../../

OUTPUTDIR	= $(TOP)Project/Make/Output/lib/

SRCDIR		= $(TOP)MQTTClient/src/src/

SOURCES		= $(SRCDIR)MQC_api.c $(SRCDIR)MQC_core.c $(SRCDIR)MQC_net.c \
				$(SRCDIR)MQC_queue.c 

OBJS		= MQC_api.o MQC_core.o MQC_net.o MQC_queue.o 

TARGET_D	= share

TARGET_S	= static

LIB_SO		= libMqc.so

LIB_A		= libMqc.a

ifeq ($(PLATFORM), LINUX) 
INCLUDES	= -I$(TOP)MQTTClient/interface -I$(TOP)Platform/Linux
else
ifeq ($(PLATFORM), WINDOWS)
INCLUDES	= -I$(TOP)MQTTClient/interface -I$(TOP)Platform/Windows
else
INCLUDES	= -I$(TOP)MQTTClient/interface -I$(TOP)Platform/Embedded
endif
endif

MAKEFILE 	= Makefile

CC			?= gcc

AR			?= ar

GCC_CFLAGS	= 

DEBUG		= 

CFLAGS		= -fPIC -Wall -std=c99 -fno-strict-aliasing \
				$(INCLUDES) -O2 $(GCC_CFLAGS) $(DEBUG)

#
# Compile Menu
#

.PHONY		: all $(TARGET_D) $(TARGET_S) clean

all			: $(TARGET_D) $(TARGET_S)

$(TARGET_D)	: $(OBJS)
	$(CC) -shared -o $(LIB_SO) $(OBJS)
	if [ ! -d $(OUTPUTDIR) ]; then mkdir -p $(OUTPUTDIR); fi;
	cp -rfp $(LIB_SO) $(OUTPUTDIR)
    
$(TARGET_S)	: $(OBJS)
	$(AR) -crv $(LIB_A) $(OBJS)
	if [ ! -d $(OUTPUTDIR) ]; then mkdir -p $(OUTPUTDIR); fi;
	cp -rfp $(LIB_A) $(OUTPUTDIR)

${OBJS} 	: ${SOURCES}
	$(CC) $(CFLAGS) -c $(SOURCES)
    
clean		:
	rm -f *.o *.Z* *~ $(LIB_SO) $(LIB_A)
	rm -f $(OUTPUTDIR)$(LIB_SO) $(OUTPUTDIR)$(LIB_A)

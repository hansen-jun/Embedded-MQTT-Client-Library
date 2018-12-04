#
#	Makefile of Embedded-MQTT-Client-Library
#	CCommon Library
#

TOP			= ../../../

OUTPUTDIR	= $(TOP)Project/Make/Output/lib/

SRCDIR		= $(TOP)CommonLib/

SOURCES		= $(SRCDIR)CLIB_heap.c $(SRCDIR)CLIB_net.c 

OBJS		= CLIB_heap.o CLIB_net.o 

TARGET_D	= share

TARGET_S	= static

LIB_SO		= libCCommon.so

LIB_A		= libCCommon.a

INCLUDES	=

MAKEFILE 	= Makefile

CC			?= gcc

AR			?= ar

GCC_CFLAGS	= 

DEBUG		= 

CFLAGS		= -fPIC -Wall -fno-strict-aliasing \
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

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
SOURCES_M		= $(TOP)Tests/Paho_Interoperability_Testing/client_test_suite.c \
					$(TOP)Platform/Linux/wrapper.c \
					$(TOP)Tests/Paho_Interoperability_Testing/test_case_1.c \
					$(TOP)Tests/Paho_Interoperability_Testing/test_case_2.c \
					$(TOP)Tests/Paho_Interoperability_Testing/test_case_3.c \
                    $(TOP)Tests/Paho_Interoperability_Testing/test_case_4.c \
                    $(TOP)Tests/Paho_Interoperability_Testing/test_case_5.c \
                    $(TOP)Tests/Paho_Interoperability_Testing/test_case_6.c \
                    $(TOP)Tests/Paho_Interoperability_Testing/test_case_7.c \
                    $(TOP)Tests/Paho_Interoperability_Testing/test_case_8.c \
                    $(TOP)Tests/Paho_Interoperability_Testing/test_case_9.c \
                    $(TOP)Tests/Paho_Interoperability_Testing/test_case_10.c \
                    $(TOP)Tests/Paho_Interoperability_Testing/test_case_11.c \
                    $(TOP)Tests/Paho_Interoperability_Testing/test_case_12.c
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

OBJS_M			= client_test_suite.o wrapper.o \
					test_case_1.o test_case_2.o test_case_3.o test_case_4.o \
					test_case_5.o test_case_6.o test_case_7.o test_case_8.o \
					test_case_9.o test_case_10.o test_case_11.o test_case_12.o

MAKEFILE 		= Makefile

CC				?= gcc

AR				?= ar

SOLIBS			= -lMqc -lCCommon

SOLIBDIR		= -L$(OUTPUTDIR)lib

#
# Compile Menu
#

.PHONY			:	all paho_test cleanpaho_test clean

all				:	paho_test

clean			:	cleanpaho_test

paho_test		:	$(OBJS_M)
	$(CC) -o paho_test $(OBJS_M) $(SOLIBS) $(SOLIBDIR)
	if [ ! -d $(OUTPUTDIR)test ]; then mkdir -p $(OUTPUTDIR)test; fi;
	cp -rfp paho_test $(OUTPUTDIR)test

$(OBJS_M) 		:	$(SOURCES_M)
	$(CC) $(CFLAGS) -c $(SOURCES_M)
    
cleanpaho_test:
	rm -f *.o *.Z* *~ paho_test
	rm -f $(OUTPUTDIR)test/paho_test 

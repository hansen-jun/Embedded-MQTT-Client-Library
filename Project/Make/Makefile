#
#	Makefile of Embedded-MQTT-Client-Library
#	Project
#

MQCDIR			= ./MQTTClient

CCOMMONDIR		= ./CommonLib

MINICLIENTDIR	= ./mini_client

SSLCLIENTDIR	= ./ssl_client

PAHOTESTDIR		= ./paho_test

#
# Compile Menu
#

.PHONY				:	all clean wsc cleanwsc ccommon cleanccommon mini_client cleanmini_client ssl_client cleanssl_client paho_test cleanpaho_test

all					:	ccommon mqc

clean				:	cleanccommon cleanmqc

mqc					:
	make -C $(MQCDIR) all

cleanmqc			: 
	make -C $(MQCDIR) clean
    
ccommon				:
	make -C $(CCOMMONDIR) all
    
cleanccommon		:
	make -C $(CCOMMONDIR) clean

mini_client			:	all
	make -C $(MINICLIENTDIR) all
    
cleanmini_client	:
	make -C $(MINICLIENTDIR) clean

ssl_client			:	all
	make -C $(SSLCLIENTDIR) all

cleanssl_client		:
	make -C $(SSLCLIENTDIR) clean

paho_test			:	all
	make -C $(PAHOTESTDIR) all
    
cleanpaho_test		:
	make -C $(PAHOTESTDIR) clean

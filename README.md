# Embedded-MQTT-Client-Library

[![Build Status](https://travis-ci.com/otakann/Embedded-MQTT-Client-Library.svg?branch=master)](https://travis-ci.com/otakann/Embedded-MQTT-Client-Library)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/4391203ee05e479d9cefa372f8767e26)](https://www.codacy.com/app/otakann/Embedded-MQTT-Client-Library?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=otakann/Embedded-MQTT-Client-Library&amp;utm_campaign=Badge_Grade)

# What is this?
An Embedded MQTT Client Library with C Language

# Features
* Strictly follow [MQTT v3.1.1](http://docs.oasis-open.org/mqtt/mqtt/v3.1.1/os/mqtt-v3.1.1-os.html).
* Support QoS0 QoS1 and QoS2.
* Almost no dependencies except libc and easily transplanted.
* No need an operating system.
* Friendly Interface for embedded network stack and TLS library.

# Notice
Compiler tool chain need support C99 

# Compiling
There is no any other dependencies to Compile the library.  
You only need two folders : "**CommonLib**" and "**MQTTClient**".
1. Copy these two folders and add the .c files inside to your project.
2. Rename the file "**MQC_config_templete.h**" to "**MQC_config.h**" (in MQTTClient\interface).
3. Change the configuration in "**MQC_config.h**" and "**CLIB_def.h**" (in CommonLib) if necessary.
4. Add "**MQTTClient\interface**" to the include folder in your project.
5. Make sure your Compiler support **C99**.
6. Then you can build the library.

There are currently two active build systems used within the releases for a build example
## CMake
```
cd Project/CMake
mkdir Build && cd Build
cmake ..
make
```
When the library is compiled, **libMqc.so/.a** and **libCCommon.so/.a** will be generated in the "Output/lib" folder  
In order to use OS wrapper (ex. Linux), enter:  
```
cmake -DPLATFORM:STRING=LINUX ..
make
```
In order to build sample program (ex. mini_client), enter: 
```
cmake -DPLATFORM:STRING=LINUX -DMINI_CLIENT=On ..
make
```
## Make
```
cd Project/Make
make
```
When the library is compiled, **libMqc.so/.a** and **libCCommon.so/.a** will be generated in the "Output/lib" folder  
In order to use OS wrapper (ex. Linux), enter:  
```
make PLATFORM=LINUX
```
In order to build sample program (ex. mini_client), enter: 
```
make PLATFORM=LINUX mini_client
```

# Example programs
There are example programs for some features and uses in Sample/. .  

# License
see [LICENSE](./LICENSE). (Apache2.0 LICENSE)

#CMakeLists.txt
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)
Project(Embedded-MQTT-Client-Library)
option(MINI_CLIENT "Build mini_client example." OFF)
option(SSL_CLIENT "Build ssl_client example." OFF)
option(PAHO_TEST "Build Paho Interoperability Testing suite." OFF)
add_subdirectory(CommonLib)
add_subdirectory(MQTTClient)
if(MINI_CLIENT)
    add_subdirectory(mini_client)
endif()
if(SSL_CLIENT)
    add_subdirectory(ssl_client)
endif()
if(PAHO_TEST)
    add_subdirectory(paho_test)
endif()

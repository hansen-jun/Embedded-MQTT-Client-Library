#CMakeLists.txt
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)
set(LIBRARY_OUTPUT_PATH ../Output/lib)
set(CMAKE_C_FLAGS "-Wall -std=c99")
set(LIBMQC_SRC  ../../../MQTTClient/src/src/MQC_api.c
                ../../../MQTTClient/src/src/MQC_core.c
                ../../../MQTTClient/src/src/MQC_queue.c
                ../../../MQTTClient/src/src/MQC_net.c
)
include_directories(../../../MQTTClient/interface)
if(PLATFORM MATCHES "LINUX")
    include_directories(../../../Platform/Linux)
elseif(PLATFORM MATCHES "WINDOWS")
    include_directories(../../../Platform/Windows)
else()
    include_directories(../../../Platform/Embedded)
endif()
add_library(Mqc SHARED ${LIBMQC_SRC})
add_library(Mqc_static STATIC ${LIBMQC_SRC})
#set_target_properties(Wsc PROPERTIES VERSION 1.0 SOVERSION 1)
set_target_properties(Mqc_static PROPERTIES OUTPUT_NAME "Mqc")

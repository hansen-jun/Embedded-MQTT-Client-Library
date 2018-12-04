#CMakeLists.txt
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)
if(PLATFORM MATCHES "LINUX")
    add_definitions(-DPLATFORM_LINUX)
    include_directories(../../../MQTTClient/interface ../../../Platform/Linux)
    set(MINICLIENT_SRC  ../../../Sample/mini_client.c
                        ../../../Platform/Linux/wrapper.c
    )
elseif(PLATFORM MATCHES "WINDOWS")
    add_definitions(-DPLATFORM_WINDOWS)
else()
    add_definitions(-DPLATFORM_OTHER)
endif()
set(EXECUTABLE_OUTPUT_PATH ../Output/test)
set(CMAKE_C_FLAGS "-Wall")
link_directories(MQTTClient)
add_executable(mini_client ${MINICLIENT_SRC})
target_link_libraries(mini_client Mqc;CCommon)

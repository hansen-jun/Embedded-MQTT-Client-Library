#CMakeLists.txt
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)
if(PLATFORM MATCHES "LINUX")
    add_definitions(-DPLATFORM_LINUX)
    include_directories(../../../MQTTClient/interface ../../../Platform/Linux)
    set(SSLCLIENT_SRC  ../../../Sample/ssl_client.c
                        ../../../Platform/Linux/wrapper.c
                        ../../../Platform/Mbedtls/wrapper_ssl.c
    )
elseif(PLATFORM MATCHES "WINDOWS")
    add_definitions(-DPLATFORM_WINDOWS)
else()
    add_definitions(-DPLATFORM_OTHER)
endif()
set(EXECUTABLE_OUTPUT_PATH ../Output/test)
set(CMAKE_C_FLAGS "-Wall")
link_directories(WebSocketClient)
add_executable(ssl_client ${SSLCLIENT_SRC})
target_link_libraries(ssl_client Mqc;CCommon)
target_link_libraries(ssl_client mbedtls mbedcrypto mbedx509)

#CMakeLists.txt
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)
if(PLATFORM MATCHES "LINUX")
    add_definitions(-DPLATFORM_LINUX)
    include_directories(../../../MQTTClient/interface ../../../Platform/Linux)
    set(PAHO_TEST_SRC   ../../../Platform/Linux/wrapper.c
                        ../../../Tests/Paho_Interoperability_Testing/client_test_suite.c
                        ../../../Tests/Paho_Interoperability_Testing/test_case_1.c
                        ../../../Tests/Paho_Interoperability_Testing/test_case_2.c
                        ../../../Tests/Paho_Interoperability_Testing/test_case_3.c
                        ../../../Tests/Paho_Interoperability_Testing/test_case_4.c
                        ../../../Tests/Paho_Interoperability_Testing/test_case_5.c
                        ../../../Tests/Paho_Interoperability_Testing/test_case_6.c
                        ../../../Tests/Paho_Interoperability_Testing/test_case_7.c
                        ../../../Tests/Paho_Interoperability_Testing/test_case_8.c
                        ../../../Tests/Paho_Interoperability_Testing/test_case_9.c
                        ../../../Tests/Paho_Interoperability_Testing/test_case_10.c
                        ../../../Tests/Paho_Interoperability_Testing/test_case_11.c
                        ../../../Tests/Paho_Interoperability_Testing/test_case_12.c
                        ../../../Tests/Paho_Interoperability_Testing/test_case_13.c
                        ../../../Tests/Paho_Interoperability_Testing/test_case_14.c
                        ../../../Tests/Paho_Interoperability_Testing/test_case_15.c
    )
elseif(PLATFORM MATCHES "WINDOWS")
    add_definitions(-DPLATFORM_WINDOWS)
else()
    add_definitions(-DPLATFORM_OTHER)
endif()
set(EXECUTABLE_OUTPUT_PATH ../Output/test)
set(CMAKE_C_FLAGS "-Wall")
link_directories(MQTTClient)
add_executable(paho_test ${PAHO_TEST_SRC})
target_link_libraries(paho_test Mqc;CCommon)

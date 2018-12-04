#CMakeLists.txt
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)
set(LIBRARY_OUTPUT_PATH ../Output/lib)
set(CMAKE_C_FLAGS "-Wall")
set(LIBCOMMON_SRC   ../../../CommonLib/CLIB_heap.c
                    ../../../CommonLib/CLIB_net.c
)
add_library(CCommon SHARED ${LIBCOMMON_SRC})
add_library(CCommon_static STATIC ${LIBCOMMON_SRC})
#set_target_properties(CCommonLib PROPERTIES VERSION 1.0 SOVERSION 1)
set_target_properties(CCommon_static PROPERTIES OUTPUT_NAME "CCommon")

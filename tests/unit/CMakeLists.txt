cmake_minimum_required(VERSION 3.16)
project(WzUnitTests)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHAVE_INTTYPES_H")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/../../3rdparty/Catch2/contrib")

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../3rdparty/Catch2 build)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../../)

FILE(GLOB TEST_SOURCES lib/*/*.cpp)
add_executable(tests main.cpp ${CMAKE_CURRENT_LIST_DIR}/../../lib/widget/gridallocation.cpp ${TEST_SOURCES})

target_link_libraries(tests Catch2::Catch2)
include(CTest)
include(Catch)

catch_discover_tests(tests)

INCLUDE (CheckIncludeFiles)

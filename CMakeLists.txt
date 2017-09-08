cmake_minimum_required(VERSION 3.3)
project(threadpool)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(${CMAKE_SOURCE_DIR})
#include_directories(${PROJECT_SOURCE_DIR})

# google tests
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# boost
find_package( Boost 1.40 COMPONENTS system thread REQUIRED ) # program_options filesystem
include_directories(${Boost_INCLUDE_DIR})

# project files
set(HEADER_FILES utils.hpp)
set(SOURCE_FILES test.cpp)

add_executable(googletest ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(googletest ${Boost_LIBRARIES} ${GTEST_BOTH_LIBRARIES} pthread )


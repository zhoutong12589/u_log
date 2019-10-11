cmake_minimum_required(VERSION 2.8)

PROJECT(test_log)
SET(CMAKE_VERBOSE_MAKEFILE ON)
SET(CMAKE_BUILD_TYPE DEBUG)
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -Wall ")
add_compile_options(-std=c++11)

MESSAGE(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
MESSAGE(STATUS "CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")

#include_directories("${ZMQ}/include" )

#link_directories need to be put before ADD_EXECUTABLE
#link_directories("${ZMQ}/lib64")

ADD_EXECUTABLE(${PROJECT_NAME} main.cpp logging.cpp configurator.cpp)


#target_link_libraries(${PROJECT_NAME} zmq ${EXTRAL_LIBS})


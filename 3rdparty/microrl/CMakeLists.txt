cmake_minimum_required(VERSION 2.8)
project(microrl)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -Wall --std=gnu99")
add_library(${PROJECT_NAME} STATIC ${CMAKE_CURRENT_SOURCE_DIR}/microrl.c)
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

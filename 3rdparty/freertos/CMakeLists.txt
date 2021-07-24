cmake_minimum_required(VERSION 2.8)
project(freertos C ASM)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall --std=gnu99")

file(GLOB_RECURSE SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/portable/GCC/ARM_CM4F/port.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/portable/MemMang/heap_4.c"
)

add_library(${PROJECT_NAME} STATIC ${SRC})

target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/portable/GCC/ARM_CM4F"
)

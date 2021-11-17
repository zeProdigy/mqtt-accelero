set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Cortex-M4.cmake")

cmake_minimum_required(VERSION 2.8)

project(stm32f4xx_base C CXX ASM)

option(FREERTOS "Build target with include FreeRTOS" OFF)

set(LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/STM32F407VGTx_FLASH.ld")

add_definitions(-DSTM32F407xx)
add_definitions(-DUSE_FULL_LL_DRIVER)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -Wall --std=gnu99 -fdiagnostics-color=always")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall --std=gnu++11 -fdiagnostics-color=always")

set(CMAKE_EXE_LINKER_FLAGS "-T ${LINKER_SCRIPT}")


# -- 3rdparty
add_subdirectory(3rdparty/tiny-printf)
add_subdirectory(3rdparty/microrl)
add_subdirectory(3rdparty/freertos)


# -- Includes
include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/hal"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/hal/st-hal"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/cmsis"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/usb"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/usb/core"
)

set(LWIP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/lwip)
set(LWIP_INCLUDE_DIRS
    "${LWIP_DIR}/src/include"
    "${LWIP_DIR}/contrib"
    "${LWIP_DIR}/contrib/ports/freertos/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/3rdparty/lwip"
)


# -- ST HAL
file(GLOB_RECURSE STM32F4XX_HAL_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/hal/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/hal/st-hal/*.c"
)

add_library(st-hal STATIC ${STM32F4XX_HAL_SRC})


# -- System
file(GLOB_RECURSE SYSTEM_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/system/*.c"
)

add_library(system STATIC ${SYSTEM_SRC})
target_include_directories(system PRIVATE ${LWIP_INCLUDE_DIRS})
target_link_libraries(system st-hal st-usb osal tiny-printf microrl lwipcore)


# -- OSAL
file(GLOB_RECURSE OSAL_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/osal/*.c"
)
add_library(osal STATIC ${OSAL_SRC})
target_link_libraries(osal freertos)


# -- lwip
include(${LWIP_DIR}/src/Filelists.cmake)

set(LWIP_FREERTOS_SRC "${LWIP_DIR}/contrib/ports/freertos/sys_arch.c")

add_library(lwip_contrib_port_freertos ${LWIP_FREERTOS_SRC})
target_include_directories(lwip_contrib_port_freertos PRIVATE
    "${LWIP_INCLUDE_DIRS}"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/freertos/Source/include")
target_link_libraries(lwip_contrib_port_freertos freertos)

target_link_libraries(lwipcore lwip_contrib_port_freertos system)


# -- USB
file(GLOB_RECURSE LIB_USB_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/usb/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/usb/core/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/usb/class/cdc_ecm/*.c"
)

add_library(st-usb STATIC ${LIB_USB_SRC})
target_link_libraries(st-usb st-hal tiny-printf)

target_compile_definitions(st-usb PUBLIC USE_USB_FS)
target_include_directories(st-usb PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include/usb/class/cdc_ecm"
    "${LWIP_INCLUDE_DIRS}"
)


# -- APP
file(GLOB_RECURSE APP_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/app/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/app/shell/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/startup_stm32f407xx.s"
)


# -- BUILD
add_executable(${PROJECT_NAME} ${APP_SRC})

target_include_directories(${PROJECT_NAME} PRIVATE ${LWIP_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} system st-hal st-usb osal lwipallapps)

set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".elf")


# -- POST BUILD
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND arm-none-eabi-size "${PROJECT_NAME}.elf")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} ARGS -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin)

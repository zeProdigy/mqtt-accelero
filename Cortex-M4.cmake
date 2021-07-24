set(CMAKE_SYSTEM_PROCESSOR  arm)
set(CMAKE_CROSSCOMPILING    ON)
set(CMAKE_SYSTEM_NAME       Generic)
set(CMAKE_C_COMPILER        arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER      arm-none-eabi-g++)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_CORTEX_M4_C_FLAGS " \
    --specs=nosys.specs \
    -mthumb \
    -mcpu=cortex-m4 \
    -mfpu=fpv4-sp-d16 \
    -mfloat-abi=hard \
    -fomit-frame-pointer \
    -ffunction-sections \
    -fdata-sections \
    -Wl,--gc-sections,-Map=output.map" CACHE STRING "" FORCE)


set(CMAKE_CORTEX_M4_CXX_FLAGS " \
    --specs=nosys.specs \
    -mthumb \
    -mcpu=cortex-m4 \
    -mfpu=fpv4-sp-d16 \
    -mfloat-abi=soft" CACHE STRING "" FORCE)

set(CMAKE_C_FLAGS   "${CMAKE_CORTEX_M4_C_FLAGS}"
    CACHE STRING "" FORCE)

set(CMAKE_ASM_FLAGS "${CMAKE_CORTEX_M4_C_FLAGS}"
    CACHE STRING "" FORCE)

set(CMAKE_CXX_FLAGS "${CMAKE_CORTEX_M4_CXX_FLAGS}"
    CACHE STRING "" FORCE)

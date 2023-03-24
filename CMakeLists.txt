cmake_minimum_required(VERSION 3.22)

# Setup compiler settings
set(CMAKE_C_STANDARD                11)
set(CMAKE_C_STANDARD_REQUIRED       ON)
set(CMAKE_C_EXTENSIONS              ON)
set(CMAKE_CXX_STANDARD              17)
set(CMAKE_CXX_STANDARD_REQUIRED     ON)
set(CMAKE_CXX_EXTENSIONS            ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS	ON)
set(PROJ_PATH                       ${CMAKE_CURRENT_SOURCE_DIR})
message("Build type: "              ${CMAKE_BUILD_TYPE})

# Setup .camke file location
set(CMAKE_TOOLCHAIN_FILE			"${CMAKE_CURRENT_SOURCE_DIR}/cmake/gcc-arm-none-eabi.cmake")


# Core project settings
project(L432KC)
enable_language(C CXX ASM)


# Core MCU flags, CPU, instruction set and FPU setup
# Needs to be set properly for your MCU
set(CPU_PARAMETERS
    -mthumb
	
	# This needs attention to properly set for used MCU
    -mcpu=cortex-m4
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
)

# Set linker script
set(linker_script_SRC               ${PROJ_PATH}/Core/STM32G431KBTx_FLASH.ld)
set(EXECUTABLE                      ${CMAKE_PROJECT_NAME})


# List of source files to compile
set(sources_SRCS
    # Put here your source files, one in each line, relative to CMakeLists.txt file location
	${PROJ_PATH}/Core/Inc/lwshell/lwshell.c
	${PROJ_PATH}/Core/Src/CLI.cpp
	${PROJ_PATH}/Core/Src/gpio.c
	${PROJ_PATH}/Core/Src/LED.cpp
	${PROJ_PATH}/Core/Src/main.cpp
	${PROJ_PATH}/Core/Src/SerialCOM.cpp
	${PROJ_PATH}/Core/Src/stm32g4xx_hal_msp.c
	${PROJ_PATH}/Core/Src/stm32g4xx_hal_timebase_tim.c
	${PROJ_PATH}/Core/Src/stm32g4xx_it.c
	${PROJ_PATH}/Core/Src/syscalls.c
	${PROJ_PATH}/Core/Src/system_stm32g4xx.c
	${PROJ_PATH}/Core/Src/Thread.cpp
	${PROJ_PATH}/Core/Src/tim.c
	${PROJ_PATH}/Core/Src/usart.c
	${PROJ_PATH}/Core/startup_stm32g431xx.s
	${PROJ_PATH}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal.c
	${PROJ_PATH}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_cortex.c
	${PROJ_PATH}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_dma.c
	${PROJ_PATH}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_dma_ex.c
	${PROJ_PATH}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_exti.c
	${PROJ_PATH}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_flash.c
	${PROJ_PATH}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_flash_ex.c
	${PROJ_PATH}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_flash_ramfunc.c
	${PROJ_PATH}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_gpio.c
	${PROJ_PATH}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_pwr.c
	${PROJ_PATH}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_pwr_ex.c
	${PROJ_PATH}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_rcc.c
	${PROJ_PATH}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_rcc_ex.c
	${PROJ_PATH}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_tim.c
	${PROJ_PATH}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_tim_ex.c
	${PROJ_PATH}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_uart.c
	${PROJ_PATH}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_uart_ex.c
	${PROJ_PATH}/Middlewares/Third_Party/FreeRTOS/Source/croutine.c
	${PROJ_PATH}/Middlewares/Third_Party/FreeRTOS/Source/event_groups.c
	${PROJ_PATH}/Middlewares/Third_Party/FreeRTOS/Source/list.c
	${PROJ_PATH}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c
	${PROJ_PATH}/Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c
	${PROJ_PATH}/Middlewares/Third_Party/FreeRTOS/Source/queue.c
	${PROJ_PATH}/Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c
	${PROJ_PATH}/Middlewares/Third_Party/FreeRTOS/Source/tasks.c
	${PROJ_PATH}/Middlewares/Third_Party/FreeRTOS/Source/timers.c
)


# Include directories
set(include_path_DIRS
    # Put here your include dirs, one in each line, relative to CMakeLists.txt file location
	${PROJ_PATH}/Core/Inc
	${PROJ_PATH}/Core/Inc/lwshell
	${PROJ_PATH}/Drivers/CMSIS/Device/ST/STM32G4xx/Include
	${PROJ_PATH}/Drivers/CMSIS/Include
	${PROJ_PATH}/Drivers/STM32G4xx_HAL_Driver/Inc
	${PROJ_PATH}/Drivers/STM32G4xx_HAL_Driver/Inc/Legacy
	${PROJ_PATH}/Middlewares/Third_Party/FreeRTOS/Source/include
	${PROJ_PATH}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F
)


# Symbols definition
set(symbols_SYMB
    # Put here your symbols (preprocessor defines), one in each line
    # Encapsulate them with double quotes for safety purpose
    "USE_HAL_DRIVER"
    "STM32G431xx"
)

# Executable files
add_executable(${EXECUTABLE} ${sources_SRCS})

# Include paths
target_include_directories(${EXECUTABLE} PRIVATE ${include_path_DIRS})

# Project symbols
target_compile_definitions(${EXECUTABLE} PRIVATE ${symbols_SYMB})

# Compiler options
target_compile_options(${EXECUTABLE} PRIVATE
    ${CPU_PARAMETERS}
    -Wall
    # -Wextra
    -Wpedantic
    -Wno-unused-parameter
)

# Linker options
target_link_options(${EXECUTABLE} PRIVATE
    -T${linker_script_SRC}
    ${CPU_PARAMETERS}
    -Wl,-Map=${CMAKE_PROJECT_NAME}.map
    --specs=nosys.specs
    -Wl,--start-group
    -lc
    -lm
    -lstdc++
    -lsupc++
    -Wl,--end-group
    -Wl,--print-memory-usage
)

# Execute post-build to print size
add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${EXECUTABLE}>
)

# Convert output to hex and binary
add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${EXECUTABLE}> ${EXECUTABLE}.hex
)

# Convert to bin file -> add conditional check?
add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${EXECUTABLE}> ${EXECUTABLE}.bin
)
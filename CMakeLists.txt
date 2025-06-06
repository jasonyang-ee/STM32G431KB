cmake_minimum_required(VERSION 3.22)

# Setup cmake module path and compiler settings
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
message("Build type: "              ${CMAKE_BUILD_TYPE})
set(CMAKE_C_STANDARD                17)
set(CMAKE_C_STANDARD_REQUIRED       ON)
set(CMAKE_C_EXTENSIONS              ON)
set(CMAKE_CXX_STANDARD              23)
set(CMAKE_CXX_STANDARD_REQUIRED     ON)
set(CMAKE_CXX_EXTENSIONS            ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS	ON)
set(PROJ_PATH                       ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_TOOLCHAIN_FILE			gcc-arm-none-eabi)



# Project Name    --- MUST EDIT ---
project(G431-Playground)
enable_language(C CXX ASM)

# Linker File     --- MUST EDIT ---
set(linker_script_SRC               ${PROJ_PATH}/Core/STM32G431KBTx_FLASH.ld)
set(EXECUTABLE                      ${CMAKE_PROJECT_NAME})



# USE .\getIncludeList.sh and .\getSourceList.sh to generate file list
# Include List, Source List, and MCU Sepecific Setting
# Must Update these files in cmake/ when adding new source files
include(STM32G431xx_HAL_PARA)
include(SourceList)
include(IncludeList)

# Executable files
add_executable(${EXECUTABLE} ${source_list})

# Include paths
target_include_directories(${EXECUTABLE} PRIVATE ${include_list})

# Project symbols
target_compile_definitions(${EXECUTABLE} PRIVATE ${compiler_define})

# Compiler options
target_compile_options(${EXECUTABLE} PRIVATE
	${CPU_PARAMETERS}
	-Wall
	-Wpedantic
	-Wno-unused-parameter
    -ffunction-sections
    -fdata-sections
)

# Linker options
target_link_options(${EXECUTABLE} PRIVATE
	-T${linker_script_SRC}
	${CPU_PARAMETERS}
	--specs=nosys.specs	  # Will remove support for SWO.
	-u _printf_float
	-lc
	-lm
	-lstdc++
	-lsupc++
	-Wl,--start-group
	-Wl,--end-group
	-Wl,--print-memory-usage
	-Wl,-Map=${CMAKE_PROJECT_NAME}.map
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
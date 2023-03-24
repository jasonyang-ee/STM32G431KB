#!/bin/bash

# Run .\getIncludeList.sh in terminal to generate an updated list of source for CMake

CORE_PATH='Core/*'
CUSTOM_PATH='Core/Inc/*'
DRIVERS_PATH='Drivers/*'
FATFS_PATH='FATFS/*'
MIDWARE_PATH='Middlewares/*'
RTOS='Middlewares/Third_Party/FreeRTOS/*'
USB_PATH='USB_Device/*'

CORE_INC="$(eval find "$CORE_PATH" -type d -name 'Inc' -o -name 'Include')"
CUSTOM_INC="$(eval find "$CUSTOM_PATH" -type d)"
DRIVERS_INC="$(eval find "$DRIVERS_PATH" -type d -name 'Inc' -o -name 'Include' -o -name 'Legacy')"
RTOS="$(eval find "$MIDWARE_PATH" -type d -name 'include' -o -name 'CMSIS_RTOS' -o -name 'ARM_CM4F')"
MIDWARE_INC="$(eval find "$MIDWARE_PATH" -type d -name 'Inc' -o -name 'src')"
USB_INC="$(eval find "$USB_PATH" -type d)"
FATFS_INC="$(eval find "$FATFS_PATH" -type d)"

# Print find result
printf "$CORE_INC\n$CUSTOM_INC\n$DRIVERS_INC\n$RTOS\n$MIDWARE_INC\n$USB_INC\n$FATFS_INC" > cmake/IncludeList.txt

# Adding ${PROJ_PATH}/ to beginning of each line
sed -i 's/^/${PROJ_PATH}\//' cmake/IncludeList.txt
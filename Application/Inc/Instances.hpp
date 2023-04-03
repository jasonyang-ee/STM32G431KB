#ifndef APPLICATION_INC_INSTANCES
#define APPLICATION_INC_INSTANCES

#include "LED.hpp"
#include "SerialCOM.hpp"
#include "CLI.hpp"
#include "Thread.hpp"
#include "Flash_STM32G431KB.hpp"

#define UART_BUFFER 64

extern LED led_user;
extern SerialCOM serialCOM;
extern CLI cli;
extern Thread thread;
extern Flash flash;

#endif    /* APPLICATION_INC_INSTANCES */

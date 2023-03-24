#ifndef CORE_INC_INSTANCES
#define CORE_INC_INSTANCES

#include "LED.hpp"
#include "SerialCOM.hpp"
#include "CLI.hpp"
#include "Thread.hpp"

#define UART_BUFFER 64

extern LED led_user;
extern SerialCOM serialCOM;
extern CLI cli;
extern Thread thread;


#endif    /* CORE_INC_INSTANCES */

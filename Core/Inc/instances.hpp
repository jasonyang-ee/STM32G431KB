#ifndef CORE_INC_INSTANCES
#define CORE_INC_INSTANCES

#include "LED.hpp"
#include "SerialCOM.hpp"
#include "CLI.hpp"
#include "Thread.hpp"
#include "Flash.hpp"

#define UART_BUFFER 64

extern LED led_user;
extern SerialCOM serialCOM;
extern CLI cli;
extern Thread thread;
extern Flash flash;

#endif    /* CORE_INC_INSTANCES */

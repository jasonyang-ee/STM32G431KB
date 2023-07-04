#ifndef APPLICATION_INC_INSTANCES
#define APPLICATION_INC_INSTANCES

#include "LED.hpp"
#include "SerialCOM.hpp"
#include "CLI.hpp"
#include "Thread.hpp"
#include "DAC.hpp"
#include "ADC.hpp"
#include "Flash_STM32G431KB.hpp"
#include "State.hpp"

#define UART_BUFFER 64

extern LED led_user;
extern SerialCOM serialCOM;
extern CLI cli;
extern Thread thread;
extern Flash flash;
extern CustomDAC dac;
extern CustomADC adc;

extern sml::sm<StreamState> stream_sm;
extern sml::sm<MainState, sml::process_queue<std::queue>> main_sm;

#endif    /* APPLICATION_INC_INSTANCES */

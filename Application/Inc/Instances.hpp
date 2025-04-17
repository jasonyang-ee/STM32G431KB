#ifndef APPLICATION_INC_INSTANCES
#define APPLICATION_INC_INSTANCES

#include "ADC.hpp"
#include "CLI.hpp"
// #include "CRC.hpp"
#include "DAC.hpp"
#include "Flash.hpp"
#include "LED.hpp"
#include "RTC.hpp"
#include "Serial.hpp"
#include "Thread.hpp"

extern LED led_user;
extern Serial serial;
extern CLI cli;
extern Thread thread;
extern Flash flash;
extern CustomDAC dac;
extern CustomADC adc;
extern CustomRTC rtc;
// extern CustomCRC crc;

#endif /* APPLICATION_INC_INSTANCES */

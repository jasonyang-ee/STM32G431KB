#ifndef APPLICATION_INC_RTC
#define APPLICATION_INC_RTC

#include "array"
#include "main.h"

class CustomRTC {
   public:
    CustomRTC();
    ~CustomRTC();
    void setPort(RTC_HandleTypeDef *port);

    void setTime(uint8_t hour, uint8_t minute, uint8_t second);
    void setDate(uint8_t day, uint8_t month, uint8_t year);
    std::array<uint8_t, 3> getDate();
    std::array<uint8_t, 3> getTime();

   private:
    RTC_HandleTypeDef m_port;
    RTC_TimeTypeDef m_time;
    RTC_DateTypeDef m_date;
};

#endif /* APPLICATION_INC_RTC */


// fill all functions in this file

#include "RTC.hpp"

#include "Instances.hpp"

CustomRTC::CustomRTC() {}

CustomRTC::~CustomRTC() {}

void CustomRTC::setPort(RTC_HandleTypeDef *port) { m_port = *port; }

void CustomRTC::setTime(uint8_t hour, uint8_t minute, uint8_t second) {
    m_time.Hours = hour;
    m_time.Minutes = minute;
    m_time.Seconds = second;
    if (HAL_RTC_SetTime(&m_port, &m_time, RTC_FORMAT_BIN) != HAL_OK) {
        serialCOM.sendString("RTC Set Time Error\n");
    }
}

void CustomRTC::setDate(uint8_t day, uint8_t month, uint8_t year) {
    m_date.Date = day;
    m_date.Month = month;
    m_date.Year = year;
    if (HAL_RTC_SetDate(&m_port, &m_date, RTC_FORMAT_BIN) != HAL_OK) {
        serialCOM.sendString("RTC Set Date Error\n");
    }
}

std::array<uint8_t, 3> CustomRTC::getDate() {
    HAL_RTC_GetTime(&m_port, &m_time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&m_port, &m_date, RTC_FORMAT_BIN);
    return {m_date.Month, m_date.Date, m_date.Year};
}

std::array<uint8_t, 3> CustomRTC::getTime() {
    HAL_RTC_GetTime(&m_port, &m_time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&m_port, &m_date, RTC_FORMAT_BIN);
    return {m_time.Hours, m_time.Minutes, m_time.Seconds};
}

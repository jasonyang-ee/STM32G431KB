// fill all functions in this file

#include "RTC.hpp"

#include "Instances.hpp"

/// @brief Default constructor for RTC interface wrapper.
CustomRTC::CustomRTC() {}

/// @brief Destructor for RTC interface wrapper.
CustomRTC::~CustomRTC() {}

/// @brief Assign the HAL RTC handle for subsequent operations.
/// @param port Pointer to RTC handle initialized by HAL.
void CustomRTC::setPort(RTC_HandleTypeDef *port) { m_port = *port; }

/// @brief Set the current RTC time.
/// @param hour Hour value (0-23).
/// @param minute Minute value (0-59).
/// @param second Second value (0-59).
/// @note On failure, an error message is sent over serial.
void CustomRTC::setTime(uint8_t hour, uint8_t minute, uint8_t second) {
    m_time.Hours = hour;
    m_time.Minutes = minute;
    m_time.Seconds = second;
    if (HAL_RTC_SetTime(&m_port, &m_time, RTC_FORMAT_BIN) != HAL_OK) {
        serial.sendString("RTC Set Time Error\n");
    }
}

/// @brief Set the current RTC date.
/// @param day Day of month (1-31).
/// @param month Month value (1-12).
/// @param year Year offset from 2000.
/// @note On failure, an error message is sent over serial.
void CustomRTC::setDate(uint8_t day, uint8_t month, uint8_t year) {
    m_date.Date = day;
    m_date.Month = month;
    m_date.Year = year;
    if (HAL_RTC_SetDate(&m_port, &m_date, RTC_FORMAT_BIN) != HAL_OK) {
        serial.sendString("RTC Set Date Error\n");
    }
}

/// @brief Retrieve the current date from RTC.
/// @return std::array {month, day, year} in binary format.
std::array<uint8_t, 3> CustomRTC::getDate() {
    HAL_RTC_GetTime(&m_port, &m_time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&m_port, &m_date, RTC_FORMAT_BIN);
    return {m_date.Month, m_date.Date, m_date.Year};
}

/// @brief Retrieve the current time from RTC.
/// @return std::array {hours, minutes, seconds} in binary format.
std::array<uint8_t, 3> CustomRTC::getTime() {
    HAL_RTC_GetTime(&m_port, &m_time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&m_port, &m_date, RTC_FORMAT_BIN);
    return {m_time.Hours, m_time.Minutes, m_time.Seconds};
}

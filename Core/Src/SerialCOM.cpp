#include "SerialCOM.hpp"

SerialCOM::SerialCOM() {}

SerialCOM::~SerialCOM() {}

/**
 * @brief Initialize with uart handler reference and start device.
 *
 * @param port
 */
void SerialCOM::setPort(UART_HandleTypeDef *port) { m_port = port; }

/**
 * @brief Append new line to buffer
 *
 */
void SerialCOM::sendLn() { m_buffer_msg.append("\n"); };

/**
 * @brief Append string to buffer.
 *
 * @param msg C++ string to send.
 */
void SerialCOM::sendString(std::string msg) { m_buffer_msg.append(msg); }

/**
 * @brief Append single number to buffer.
 *
 * @param value all kinds of numbers.
 */
void SerialCOM::sendNumber(int value) { m_buffer_msg.append(std::to_string(value)); }
void SerialCOM::sendNumber(float value) { m_buffer_msg.append(std::to_string(value)); }
void SerialCOM::sendNumber(double value) { m_buffer_msg.append(std::to_string(value)); }
void SerialCOM::sendNumber(uint8_t value) { m_buffer_msg.append(std::to_string(value)); }
void SerialCOM::sendNumber(uint16_t value) { m_buffer_msg.append(std::to_string(value)); }
void SerialCOM::sendNumber(uint32_t value) { m_buffer_msg.append(std::to_string(value)); }
void SerialCOM::sendNumber(int8_t value) { m_buffer_msg.append(std::to_string(value)); }
void SerialCOM::sendNumber(int16_t value) { m_buffer_msg.append(std::to_string(value)); }
void SerialCOM::sendNumber(int32_t value) { m_buffer_msg.append(std::to_string(value)); }

/**
 * @brief Periodically send uart in DMA mode if buffer !empty.
 *
 * @note Place this in a timer interrupt.
 */
bool SerialCOM::sendOut() {
    if (!m_buffer_msg.empty()) {
        if (m_buffer_msg.size() < (UART_BUFFER)) {
            std::copy(m_buffer_msg.begin(), m_buffer_msg.end(), m_tx_data);
            m_tx_data_size = m_buffer_msg.size();
            m_buffer_msg.clear();
        } else {
            std::copy(m_buffer_msg.begin(), m_buffer_msg.begin() + UART_BUFFER,
                      m_tx_data);
            m_tx_data_size = UART_BUFFER;
            m_buffer_msg.erase(0, UART_BUFFER);
        }
        if (HAL_UART_Transmit_IT(m_port, m_tx_data, m_tx_data_size) == HAL_OK)
            return true;
    }
    return false;
}
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
 * @brief Append bool to buffer as text.
 * 
 * @param value bool value.
 */
void SerialCOM::sendNumber(bool value) {
    value ? m_buffer_msg.append("ON") : m_buffer_msg.append("OFF");
}


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
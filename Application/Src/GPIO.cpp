#include "GPIO.hpp"

CustomGPIO::CustomGPIO() {}
CustomGPIO::CustomGPIO(uint16_t value) { m_roll_period = value; }
CustomGPIO::~CustomGPIO() {}

void CustomGPIO::setPort(GPIO_TypeDef *port, uint16_t pin) {
    m_port = port;
    m_pin = pin;
}

void CustomGPIO::on() { HAL_GPIO_WritePin(m_port, m_pin, GPIO_PIN_SET); }

void CustomGPIO::off() { HAL_GPIO_WritePin(m_port, m_pin, GPIO_PIN_RESET); }

void CustomGPIO::toggle() { HAL_GPIO_TogglePin(m_port, m_pin); }

void CustomGPIO::schedulerLowActive() {
    if (m_stable) {
        if (!HAL_GPIO_ReadPin(m_port, m_pin))
            m_stable = false;
        else
            m_roll_timer = 0;
    }

    else if (HAL_GPIO_ReadPin(m_port, m_pin) || ++m_roll_timer > m_roll_period) {
        m_stable = true;
        m_pressed = true;
        m_roll_timer = m_roll_period * 0.8;
    }
}

void CustomGPIO::schedulerHighActive() {
    if (m_stable) {
        if (HAL_GPIO_ReadPin(m_port, m_pin))
            m_stable = false;
        else
            m_roll_timer = 0;
    }

    else if (!HAL_GPIO_ReadPin(m_port, m_pin) || ++m_roll_timer > m_roll_period) {
        m_stable = true;
        m_pressed = true;
        m_roll_timer = m_roll_period * 0.8;
    }
}

bool CustomGPIO::getStatus() {
    if (m_pressed) {
        m_pressed = !m_pressed;
        return true;
    } else
        return false;
}
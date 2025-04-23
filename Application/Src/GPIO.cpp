#include "GPIO.hpp"

/// @brief Default constructor for CustomGPIO.
/// Initializes rollover period and state variables.
CustomGPIO::CustomGPIO() {}

/// @brief Constructor with custom debounce period.
/// @param value Number of ticks for stable state detection.
CustomGPIO::CustomGPIO(uint16_t value) { m_roll_period = value; }

/// @brief Destructor for CustomGPIO.
CustomGPIO::~CustomGPIO() {}

/// @brief Configure the GPIO port and pin for this instance.
/// @param port Pointer to the GPIO port (e.g., GPIOA).
/// @param pin GPIO pin identifier (e.g., GPIO_PIN_0).
void CustomGPIO::setPort(GPIO_TypeDef *port, uint16_t pin) {
    m_port = port;
    m_pin = pin;
}

/// @brief Set the GPIO pin to a high logic level.
void CustomGPIO::on() { HAL_GPIO_WritePin(m_port, m_pin, GPIO_PIN_SET); }

/// @brief Set the GPIO pin to a low logic level.
void CustomGPIO::off() { HAL_GPIO_WritePin(m_port, m_pin, GPIO_PIN_RESET); }

/// @brief Toggle the current state of the GPIO pin.
void CustomGPIO::toggle() { HAL_GPIO_TogglePin(m_port, m_pin); }

/// @brief Debounce scheduler for active-low inputs.
/// Monitors pin for stable falling-edge and marks press event.
/// @note Call periodically to update internal status.
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

/// @brief Debounce scheduler for active-high inputs.
/// Monitors pin for stable rising-edge and marks press event.
/// @note Call periodically to update internal status.
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

/// @brief Retrieve and clear the press event flag.
/// @return true if a press event was detected since last call; false otherwise.
bool CustomGPIO::getStatus() {
    if (m_pressed) {
        m_pressed = !m_pressed;
        return true;
    } else
        return false;
}
#include "GPIO.hpp"

/// @brief Default constructor for CustomGPIO.
/// Initializes rollover period and state variables.
CustomGPIO::CustomGPIO() {
    // State Machine
    // clang-format off
    gpio_sm = {
        State::OFF,
        { // Entry definition: (TargetState, GuardFunction, ActionFunction, Injection)
			{ State::OFF, {}, actionOff(), {} },
			{ State::LOW_INACTIVE, {}, {}, {} },
			{ State::HIGH_INACTIVE, {}, {}, {} },
        },
        { // Transition definition: (CurrentState, Event, NextState, GuardFunction, ActionFunction, Injection)
            { State::ON, Event::OFF, State::OFF, {}, actionOff(), {} },
            { State::OFF, Event::ON, State::ON, {}, actionOn(), {} },
            { State::ON, Event::TOGGLE, State::OFF, {}, actionToggle(), {} },
            { State::OFF, Event::TOGGLE, State::ON, {}, actionToggle(), {} },
            { State::LOW_INACTIVE, Event::POLL, State::LOW_SETTLING, guardLowSense(), {}, {} },
            { State::LOW_SETTLING, Event::POLL, State::LOW_ACTIVE, guardStable(), {}, {} },
            { State::LOW_ACTIVE, Event::POLL, State::LOW_INACTIVE, {}, guardHighSense(), {} },
			{ State::HIGH_INACTIVE, Event::POLL, State::HIGH_SETTLING, guardHighSense(), {}, {} },
			{ State::HIGH_SETTLING, Event::POLL, State::HIGH_ACTIVE, guardStable(), {}, {} },
			{ State::HIGH_ACTIVE, Event::POLL, State::HIGH_INACTIVE, {}, guardLowSense(), {} },
        }
    };
    // clang-format on
}

/// @brief Constructor with custom debounce period.
/// @param value Number of ticks for stable state detection.
CustomGPIO::CustomGPIO(uint16_t PollPeriod) { roll_period = PollPeriod; }

/// @brief Destructor for CustomGPIO.
CustomGPIO::~CustomGPIO() {}

/// @brief Configure the GPIO port and pin for this instance.
/// @param port Pointer to the GPIO port (e.g., GPIOA).
/// @param pin GPIO pin identifier (e.g., GPIO_PIN_0).
void CustomGPIO::setPort(GPIO_TypeDef *Port, uint16_t Pin) {
    port = Port;
    pin = Pin;
}

/// @brief Set the GPIO pin to a high logic level.
void CustomGPIO::on() { gpio_sm.triggerEvent(Event::ON); }

/// @brief Set the GPIO pin to a low logic level.
void CustomGPIO::off() { gpio_sm.triggerEvent(Event::OFF); }

/// @brief Toggle the current state of the GPIO pin.
void CustomGPIO::toggle() { gpio_sm.triggerEvent(Event::TOGGLE); }

bool CustomGPIO::getStatus() {
    if (gpio_sm.currentState == State::ON) {
        return true;
    } else if (gpio_sm.currentState == State::LOW_ACTIVE) {
        return true;
    } else if (gpio_sm.currentState == State::HIGH_ACTIVE) {
        return true;
    }
    return false;
}

/// @brief Scheduler for state-driven GPIO updates.
void CustomGPIO::scheduler() { gpio_sm.triggerEvent(Event::POLL); }

Action CustomGPIO::actionOn() {
    return [this]() { HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET); };
}

Action CustomGPIO::actionOff() {
    return [this]() { HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET); };
}

Action CustomGPIO::actionToggle() {
    return [this]() { HAL_GPIO_TogglePin(port, pin); };
}

Guard CustomGPIO::guardLowSense() {
    return [this]() { return !HAL_GPIO_ReadPin(port, pin); };
}

Guard CustomGPIO::guardHighSense() {
    return [this]() { return HAL_GPIO_ReadPin(port, pin); };
}

Guard CustomGPIO::guardStable() {
    return [this]() {
        if (roll_timer++ > roll_period) {
            roll_timer = 0;
            return true;
        } else {
            return false;
        }
    };
}
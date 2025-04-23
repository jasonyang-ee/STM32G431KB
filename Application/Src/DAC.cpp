#include "DAC.hpp"
#include "Instances.hpp"
#include "cmath"

/// @brief Default constructor for CustomDAC state machine.
/// Initializes state transitions and sets initial OFF state.
CustomDAC::CustomDAC() {
    // State Machine
    // clang-format off
    dac_sm = {
        State::OFF,
        { // Entry definition: (TargetState, GuardFunction, ActionFunction, Injection)
            { State::OFF, {}, actionOff(), {} },
            { State::ON, {}, actionOn(), {} },
            { State::BREATH, {}, actionBreath(), {} },
            { State::SINE, {}, actionSine(), {} },
        },
        { // Transition definition: (CurrentState, Event, NextState, GuardFunction, ActionFunction, Injection)
            { State::OFF, Event::START, State::ON, {}, actionOn(), {} },
            { State::ON, Event::STOP, State::OFF, {}, actionOff(), {} },
            { State::ON, Event::BREATH, State::BREATH, {}, actionBreath(), {} },
            { State::ON, Event::SINE, State::SINE, {}, actionSine(), {} },
            { State::BREATH, Event::STOP, State::OFF, {}, actionOff(), {} },
            { State::SINE, Event::STOP, State::OFF, {}, actionOff(), {} },
        }
    };
    // clang-format on

    // Set initial state
    SM<CustomDAC>::setState(State::OFF, dac_sm);
}

/// @brief Destructor for CustomDAC.
CustomDAC::~CustomDAC() {}

/// @brief Assign HAL DAC handle and channel for output.
/// @param port Pointer to DAC handle.
/// @param channel DAC channel identifier.
void CustomDAC::setPort(DAC_HandleTypeDef *port, uint32_t channel) {
    this->port = port;
    this->channel = channel;
}

/// @brief Set reference voltage for DAC conversions.
/// @param value Reference voltage in volts.
void CustomDAC::setVoltRef(double value) { vref = value; }

/// @brief Start the DAC peripheral.
/// @note Calls HAL_DAC_Start with configured port and channel.
void CustomDAC::init() { HAL_DAC_Start(port, channel); }

/// @brief Set output level and trigger DAC output.
/// @param value Voltage level to set (<= vref).
void CustomDAC::setLevel(double value) {
    level = value;
    on();
}

/// @brief Increment output level with bounds checking.
/// @param value Voltage to add to current level.
void CustomDAC::addLevel(double value) {
    level += value;
    if (level > vref) level = vref;
    if (level < 0) level = 0;
    on();
}

/// @brief Retrieve the current output voltage level.
/// @return Current level in volts.
double CustomDAC::getLevel() { return level; }

/// @brief Trigger state machine START event to enable output.
void CustomDAC::on() { SM<CustomDAC>::triggerEvent(Event::START, dac_sm); }

/// @brief Trigger state machine STOP event to disable output.
void CustomDAC::off() { SM<CustomDAC>::triggerEvent(Event::STOP, dac_sm); }

/// @brief Trigger state machine BREATH event for breathing effect.
void CustomDAC::breath() { SM<CustomDAC>::triggerEvent(Event::BREATH, dac_sm); }

/// @brief Trigger state machine SINE event for sine wave output.
void CustomDAC::sine() { SM<CustomDAC>::triggerEvent(Event::SINE, dac_sm); }

/// @brief Generate a sine wave lookup table.
/// @param amplitude Peak voltage of sine wave.
/// @param frequency Frequency in Hz.
/// @param samplingRate Sampling rate in samples per second.
void CustomDAC::generateSineWave(double amplitude, double frequency, double samplingRate) {
    this->amplitude = amplitude;
    this->frequency = frequency;
    this->samplingRate = samplingRate;
    for (int i = 0; i < 1000; i++) {
        double time = i / samplingRate;
        sineWaveArray[i] = amplitude * std::sin(2 * M_PI * frequency * time);
    }
}

/// @brief Scheduler for state-driven DAC updates.
/// @note Handles BREATH state level changes periodically.
void CustomDAC::schedule() {
    if (dac_sm.currentState == State::BREATH) {
        level = abs(sin(HAL_GetTick() % breath_period)) * 3.3;
    }
    applyLevel();
}

// Private Functions

/// @brief Apply the current level to the DAC peripheral.
void CustomDAC::applyLevel() {
    uint32_t count_value = static_cast<uint32_t>(level * 4096 / vref);
    HAL_DAC_SetValue(port, channel, alignment, count_value);
}

/// @brief Zero the DAC output level.
void CustomDAC::zeroLevel() { HAL_DAC_SetValue(port, channel, alignment, 0); }

/// @brief Action to apply the configured output level.
Action CustomDAC::actionOn() {
    return [this]() { applyLevel(); };
}

/// @brief Action to zero the output level.
Action CustomDAC::actionOff() {
    return [this]() { zeroLevel(); };
}

/// @brief Action for breath mode (alias to zero output).
Action CustomDAC::actionBreath() {
    return [this]() { zeroLevel(); };
}

/// @brief Action for sine mode (no-op placeholder).
Action CustomDAC::actionSine() {
    return [this]() { /* Implement sine wave action if needed */ };
}

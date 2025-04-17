#include "DAC.hpp"
#include "Instances.hpp"
#include "cmath"

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

CustomDAC::~CustomDAC() {}

void CustomDAC::setPort(DAC_HandleTypeDef *port, uint32_t channel) {
    this->port = port;
    this->channel = channel;
}

void CustomDAC::setVoltRef(double value) { vref = value; }

void CustomDAC::init() { HAL_DAC_Start(port, channel); }

void CustomDAC::setLevel(double value) {
    level = value;
    on();
}

void CustomDAC::addLevel(double value) {
    level += value;
    if (level > vref) level = vref;
    if (level < 0) level = 0;
    on();
}

double CustomDAC::getLevel() { return level; }

void CustomDAC::on() { SM<CustomDAC>::triggerEvent(Event::START, dac_sm); }

void CustomDAC::off() { SM<CustomDAC>::triggerEvent(Event::STOP, dac_sm); }

void CustomDAC::breath() { SM<CustomDAC>::triggerEvent(Event::BREATH, dac_sm); }

void CustomDAC::sine() { SM<CustomDAC>::triggerEvent(Event::SINE, dac_sm); }

void CustomDAC::generateSineWave(double amplitude, double frequency, double samplingRate) {
    this->amplitude = amplitude;
    this->frequency = frequency;
    this->samplingRate = samplingRate;
    for (int i = 0; i < 1000; i++) {
        double time = i / samplingRate;
        sineWaveArray[i] = amplitude * std::sin(2 * M_PI * frequency * time);
    }
}

void CustomDAC::schedule() {
    if (dac_sm.currentState == State::BREATH) {
        level = abs(sin(HAL_GetTick() % breath_period)) * 3.3;
    }
    applyLevel();
}

// Private Functions

void CustomDAC::applyLevel() {
    uint32_t count_value = static_cast<uint32_t>(level * 4096 / vref);
    HAL_DAC_SetValue(port, channel, alignment, count_value);
}

void CustomDAC::zeroLevel() { HAL_DAC_SetValue(port, channel, alignment, 0); }

Action CustomDAC::actionOn() {
    return [this]() { applyLevel(); };
}

Action CustomDAC::actionOff() {
    return [this]() { zeroLevel(); };
}

Action CustomDAC::actionBreath() {
    return [this]() { zeroLevel(); };
}

Action CustomDAC::actionSine() {
    return [this]() { /* Implement sine wave action if needed */ };
}

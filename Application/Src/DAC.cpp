#include "DAC.hpp"

#include "Instances.hpp"

MotorDAC::MotorDAC() {}

MotorDAC::~MotorDAC() {}

void MotorDAC::setPort(DAC_HandleTypeDef *port, uint32_t channel) {
    m_port = port;
    m_channel = channel;
}

void MotorDAC::setVoltRef(double value) { m_vref = value; }

void MotorDAC::setLevel(double value) {
    m_level = value;
}

void MotorDAC::addLevel(double value) {
    m_level += value;
    if (m_level > m_vref) m_level = m_vref;
    if (m_level < 0) m_level = 0;
}

double MotorDAC::getLevel() { return m_vref; }

void MotorDAC::on() { setState(MotorDAC::State::s_on); }

void MotorDAC::off() { setState(MotorDAC::State::s_off); }

void MotorDAC::setState(uint8_t cmd) { setState(static_cast<MotorDAC::State>(cmd)); }

uint8_t MotorDAC::getState() { return static_cast<uint8_t>(m_state); }

// Private Functions

void MotorDAC::applyLevel() {
    HAL_DAC_SetValue(m_port, m_channel, m_alignment, static_cast<uint32_t>(m_level));
}

void MotorDAC::zeroLevel() { HAL_DAC_SetValue(m_port, m_channel, m_alignment, 0); }

void MotorDAC::setState(MotorDAC::State cmd) {
    switch (cmd) {
        case MotorDAC::State::s_off:
            zeroLevel();
            break;
        case MotorDAC::State::s_on:
            applyLevel();
            break;
    }
    m_state = cmd;
}

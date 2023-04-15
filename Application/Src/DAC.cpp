#include "DAC.hpp"

#include "Instances.hpp"

CustomDAC::CustomDAC() {}

CustomDAC::~CustomDAC() {}

void CustomDAC::setPort(DAC_HandleTypeDef *port, uint32_t channel) {
    m_port = port;
    m_channel = channel;
}

void CustomDAC::setVoltRef(double value) { m_vref = value; }

void CustomDAC::setLevel(double value) {
    m_level = value;
    on();
}

void CustomDAC::addLevel(double value) {
    m_level += value;
    if (m_level > m_vref) m_level = m_vref;
    if (m_level < 0) m_level = 0;
	on();
}

double CustomDAC::getLevel() { return m_vref; }

void CustomDAC::on() { setState(CustomDAC::State::s_on); }

void CustomDAC::off() { setState(CustomDAC::State::s_off); }

void CustomDAC::setState(uint8_t cmd) { setState(static_cast<CustomDAC::State>(cmd)); }

uint8_t CustomDAC::getState() { return static_cast<uint8_t>(m_state); }

// Private Functions

void CustomDAC::applyLevel() {
    uint32_t count_value = static_cast<uint32_t>(m_level * 4096 / m_vref);
    HAL_DAC_SetValue(m_port, m_channel, m_alignment, count_value);
}

void CustomDAC::zeroLevel() { HAL_DAC_SetValue(m_port, m_channel, m_alignment, 0); }

void CustomDAC::setState(CustomDAC::State cmd) {
    switch (cmd) {
        case CustomDAC::State::s_off:
            zeroLevel();
            break;
        case CustomDAC::State::s_on:
            HAL_DAC_Start(m_port, m_channel);
            applyLevel();
            break;
    }
    m_state = cmd;
}

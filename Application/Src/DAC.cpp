#include "DAC.hpp"

MotorDAC::MotorDAC() {}

MotorDAC::~MotorDAC() {}

void MotorDAC::setPort(DAC_HandleTypeDef *port, uint32_t channel) {
    m_port = port;
    m_channel = channel;
}

void MotorDAC::setVoltRef(double value) { m_vref = value; }

void MotorDAC::setLevel(double value) { m_level = value; }

void MotorDAC::addLevel(double value) {
    m_level += value;
    if (m_level > m_vref) m_level = m_vref;
    if (m_level < 0) m_level = 0;
}

void MotorDAC::applyLevel() {
    HAL_DAC_SetValue(m_port, m_channel, m_alignment, static_cast<uint32_t>(m_level));
}


#include "ADC.hpp"

SensorADC::SensorADC() {}

SensorADC::~SensorADC() {}

void SensorADC::setPort(ADC_HandleTypeDef *port) { m_port = port; }

uint32_t SensorADC::sample() {
    m_value = m_buffer;
    return m_value;
}

uint32_t SensorADC::getValue() { return m_value; }


#include "ADC.hpp"

CustomADC::CustomADC() {}

CustomADC::~CustomADC() {}

void CustomADC::setPort(ADC_HandleTypeDef *port) { m_port = port; }

uint32_t CustomADC::sample() {
    m_value = m_buffer;
    return m_value;
}

uint32_t CustomADC::getValue() { return m_value; }

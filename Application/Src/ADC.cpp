
#include "ADC.hpp"

SensorADC::SensorADC() {}

SensorADC::~SensorADC() {}

void SensorADC::setPort(ADC_HandleTypeDef *port) { m_port = port; }

uint32_t SensorADC::sample() {
	HAL_ADC_Start(m_port);
	HAL_ADC_PollForConversion(m_port, 100);
    m_value = HAL_ADC_GetValue(m_port);
	HAL_ADC_Stop(m_port);
    return m_value;
}

uint32_t SensorADC::getValue() { return m_value; }

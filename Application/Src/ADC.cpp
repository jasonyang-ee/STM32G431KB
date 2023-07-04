
#include "ADC.hpp"

CustomADC::CustomADC() {}

CustomADC::~CustomADC() {}

void CustomADC::saveSample(uint8_t port) {
	switch (port) {
		// hadc1
		case 1:
			break;
		// hadc2
		case 2:
			volt_from_dac = m_buffer.at(0);
			break;
		default:
			break;
	}
}


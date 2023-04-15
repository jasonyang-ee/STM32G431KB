#ifndef APPLICATION_INC_ADC
#define APPLICATION_INC_ADC

#include "main.h"

class CustomADC {
   public:
	CustomADC();
	virtual ~CustomADC();
	void setPort(ADC_HandleTypeDef *);

	uint32_t sample();
	uint32_t getValue();

	uint32_t m_buffer;

   private:
	uint32_t m_value{};
	ADC_HandleTypeDef *m_port;
};

#endif    /* APPLICATION_INC_ADC */

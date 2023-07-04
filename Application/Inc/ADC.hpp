#ifndef APPLICATION_INC_ADC
#define APPLICATION_INC_ADC

#include "array"
#include "cmath"
#include "main.h"

#ifndef ADC_BUFFER
#define ADC_BUFFER 10
#endif

class CustomADC {
   public:
    CustomADC();
    virtual ~CustomADC();

    void saveSample(uint8_t);

    std::array <uint32_t, 1> m_buffer;

    // Public Data
    uint32_t volt_from_dac{};
};

#endif /* APPLICATION_INC_ADC */

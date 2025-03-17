

#include "main.h"

class CustomCRC {
   public:
    CustomCRC();
    virtual ~CustomCRC();

    void setPort(CRC_HandleTypeDef *port);

    void setPolynomial(uint32_t, uint32_t);
	uint32_t accumulate(uint8_t *data, uint32_t size);
    uint32_t calculate(uint8_t *data, uint32_t size);
    uint32_t calculate(uint16_t *data, uint32_t size);
    uint32_t calculate(uint32_t *data, uint32_t size);

   private:
    CRC_HandleTypeDef *port;
};
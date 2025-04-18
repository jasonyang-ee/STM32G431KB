#include "CRC.hpp"

CustomCRC::CustomCRC() {}

CustomCRC::~CustomCRC() {}

void CustomCRC::setPort(CRC_HandleTypeDef *Port) {
    port = Port;
    HAL_CRCEx_Polynomial_Set(&hcrc, 0x9B, 8);
}

void CustomCRC::setPolynomial(uint32_t Pol, uint32_t PolyLength) { HAL_CRCEx_Polynomial_Set(&hcrc, Pol, PolyLength); }

void CustomCRC::setInitValue(uint32_t Value) { port->Init.InitValue = Value; }

uint32_t CustomCRC::accumulate(uint8_t *data, uint32_t size) { HAL_CRC_Accumulate(&hcrc, (uint32_t*)data, size); }

uint32_t CustomCRC::calculate(uint8_t *data, uint32_t size) { HAL_CRC_Calculate(&hcrc, (uint32_t*)data, size); }
uint32_t CustomCRC::calculate(uint16_t *data, uint32_t size) { HAL_CRC_Calculate(&hcrc, (uint32_t*)data, size); }
uint32_t CustomCRC::calculate(uint32_t *data, uint32_t size) { HAL_CRC_Calculate(&hcrc, (uint32_t*)data, size); }

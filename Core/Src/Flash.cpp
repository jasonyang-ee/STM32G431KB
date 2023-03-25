#include "Flash.hpp"

#include "algorithm"
#include "iterator"

Flash::Flash() {}

Flash::~Flash() {}

// Writing 2KB (2048) = 32 x uint64_t of data at page 250 (0x0807d000)
void Flash::Write(const uint64_t *data, uint32_t size) {
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

    uint32_t PAGEError{0};
    FLASH_EraseInitTypeDef EraseInitStruct{};
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.NbPages = 1;
    EraseInitStruct.Page = 250;
    EraseInitStruct.Banks = FLASH_BANK_1;

    uint32_t data_addr_ptr{0};
    uint32_t address{m_address};

    HAL_FLASH_Unlock();

    HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError);

    while (data_addr_ptr < size) {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, data[data_addr_ptr]);
        data_addr_ptr++;
        address += 8;
    }

    HAL_FLASH_Lock();
}

// Reading 2KB (2048) = 32 x uint64_t of data at page 127 (0x0803F800)
uint64_t* Flash::Read() {
    uint32_t address{m_address};
    uint16_t data_ptr{0};
    for (int i = 0; i < 32; i++) {
        *(m_flash_data + data_ptr) = *(__IO uint32_t *)address;
        address += 8;
        data_ptr++;
    }
	return m_flash_data;
}
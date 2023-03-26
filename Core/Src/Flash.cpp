#include "Flash.hpp"

Flash::Flash() {}

Flash::~Flash() {}

// Writing 32 x uint64_t of data from last page last row
int32_t Flash::Save(const uint64_t *data, uint8_t size) {
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

    // Calculat Needed Pages to Erase
    const uint8_t page_needed{
        (uint8_t)((size / 32) + ((size % 32) > 0 ? 1 : 0))};
    const uint8_t page_to_clear{(uint8_t)(m_page_total - page_needed + 1)};

    // Prep Flash Erase Info
    uint32_t PAGEError{0};
    FLASH_EraseInitTypeDef EraseInitStruct{};
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.NbPages = (uint32_t)page_needed;
    EraseInitStruct.Page = (uint32_t)page_to_clear;
    EraseInitStruct.Banks = FLASH_BANK_1;

    // Start Erase Flash
    HAL_FLASH_Unlock();
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK) return 1;

    // Start Write Flash
    uint16_t data_ptr{0};
    uint32_t address{m_address_end};
    while (data_ptr < size) {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address,
                              data[data_ptr]) == HAL_OK) {
            data_ptr++;     // Next data to write
            address -= 64;  // Move target flash address a row (64 bits)
        } else
            return 1;
    }
    HAL_FLASH_Lock();
    return 0;
}

int32_t Flash::Unload() {
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

    // Calculat Needed Pages to Erase
    const uint8_t page_needed{
        (uint8_t)((m_flash_data.size() / 32) +
                  ((m_flash_data.size() % 32) > 0 ? 1 : 0))};
    const uint8_t page_to_clear{(uint8_t)(m_page_total - page_needed + 1)};

    // Prep Flash Erase Info
    uint32_t PAGEError{0};
    FLASH_EraseInitTypeDef EraseInitStruct{};
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.NbPages = (uint32_t)page_needed;
    EraseInitStruct.Page = (uint32_t)page_to_clear;
    EraseInitStruct.Banks = FLASH_BANK_1;

    // Start Erase Flash
    HAL_FLASH_Unlock();
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK) return 1;

    // Start Write Flash
    uint32_t address{m_address_end};
    while (!m_flash_data.empty()) {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address,
                              m_flash_data.front()) == HAL_OK) {
            m_flash_data.pop();
            address -= 64;  // Move target flash address a row (64 bits)
        } else
            return 1;
    }
    HAL_FLASH_Lock();
    return 0;
}

// Reading 2KB (2048) = 32 x uint64_t of data at page 127 (0x0803F800)
std::queue<uint64_t> Flash::Load() {
    // Clear previoud data
    uint32_t address{m_address_end};
    for (int i = 0; i < 32; i++) {
        m_flash_data.push(*(__IO uint32_t *)address);
        address -= 64;
    }
    return m_flash_data;
}

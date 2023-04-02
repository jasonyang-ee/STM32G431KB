#include "Flash.hpp"

#include "instances.hpp"

Flash::Flash() {}

Flash::~Flash() {}

void Flash::Save() {


    // IMPORTANT:
    // Struct has padding alignment. And one of the member has to be uint64_t
    // This will force the object be sizeof(uint64_t) aligned for flash write.
	Flash::Config config{123,led_user.getLevel(), led_user.getScale()};

	// config.led_level = led_user.getLevel();
	// config.led_scale = led_user.getScale();
	
    uint8_t size{sizeof(config)};
	Write(reinterpret_cast<const uint64_t*>(&config), size);
    // Write((const uint64_t *)&config, size);

}


// Reading 2KB (2048) = 32 x uint64_t of data at page 127 (0x0803F800)
void Flash::Load() {

	led_user.setLevel(data.front());
	data.pop();

	led_user.setScale(data.front());
	data.pop();
}
}

// Private Functions

// Writing multiple rows of (uint64_t) data from last page last row.
int32_t Flash::Write(const uint64_t *data, uint8_t size) {
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

    // 32 Rows per Page. Calculating needed Pages to write all data.
    const uint8_t page_needed{
        (uint8_t)((size / 32) + ((size % 32) > 0 ? 1 : 0))};
    const uint8_t page_to_clear{(uint8_t)(m_page_total - page_needed + 1)};

    // Prep flash erase method and range.
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
            data_ptr++;    // Next data to write
            address -= 8;  // Move target flash address a row = sizeof(64 bits)
        } else
            return 1;
    }
    HAL_FLASH_Lock();
    return 0;
}

std::queue<uint64_t> Flash::Read(uint16_t range) {
    std::queue<uint64_t> data;
    uint32_t address{m_address_end};
    while (*(__IO uint32_t *)address != 0xFFFFFFFF) {
		serialCOM.sendNumber(*(__IO uint32_t *)address);
		serialCOM.sendLn();
        data.push(*(__IO uint32_t *)address);  // 64 bits of reading width
        address -= 8;
    }
    return data;
}
#include "Flash.hpp"

#include "Instances.hpp"

Flash::Flash() {}

Flash::~Flash() {}


/**
 * @brief Saving user configuration into flash. Config structure must be defined in header.
 * 
 */
void Flash::Save() {
    Flash::Config_Arr current_config{};

	// Gether config from all instances into one object
    current_config.config.led_level = led_user.getLevel();
    current_config.config.led_scale = led_user.getScale();

	// Save config object aligned into uint64_t array into flash
    Write(current_config.config_arr, config_arr_size);
}


/**
 * @brief Loading user configuration from flash. Config structure must be defined in header.
 * 
 */
void Flash::Load() {
    Flash::Config_Arr loaded_config{};

	// Read config and save into the prepared config object
    Read(&loaded_config, config_arr_size);

	// Unpack all config back to instances
	led_user.setLevel(loaded_config.config.led_level);
    led_user.setScale(loaded_config.config.led_scale);
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

void Flash::Read(Flash::Config_Arr *loaded_config, uint8_t) {
    uint16_t data_ptr{0};
    uint32_t address{m_address_end};

	// Read 64 bits (a row) per loop and save into config object
    for(int i=0; i<config_arr_size; i++){
        loaded_config->config_arr[data_ptr] = (*(__IO uint64_t *)address);
        address -= 8;
        data_ptr++;
    }
}
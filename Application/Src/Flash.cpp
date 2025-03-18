#include "Flash.hpp"

#include "Instances.hpp"

Flash::Flash() {}

Flash::~Flash() {}

bool Flash::Save() {
    Payload payload{};
    payload.dac_level = dac.getLevel();
    payload.led_state = (uint16_t)SM<LED>::getState(led_user.led_sm);
    return Write(payload);
}

void Flash::Load() {
    std::array<uint32_t, user_pages> write_level{};
    for (uint8_t page = 0; page < user_pages; page++) {
        write_level[page] = Read(address_end - (page * page_size)).write_counts;
    }

    // find the (last save) max write count
    auto max_iter = std::max_element(write_level.rbegin(), write_level.rend());
    auto max_index = std::distance(write_level.begin(), max_iter.base()) - 1;

    Payload payload = Read(address_end - (max_index * page_size));

    serial.sendString("\nDAC Level: ");
    serial.sendNumber(payload.dac_level);
    serial.sendString("\nLED State: ");
    serial.sendNumber(payload.led_state);
    serial.sendString("\nFlash Write Counts: ");
    serial.sendNumber(payload.write_counts);

    // restore the state
    dac.setLevel(payload.dac_level);
    SM<LED>::setState(static_cast<LED::State>(payload.led_state), led_user.led_sm);
}

bool Flash::Write(Payload payload) {
    // read the write_level of each user page
    std::array<uint32_t, user_pages> write_level{};
    for (uint8_t page = 0; page < user_pages; page++) {
        write_level[page] = Read(address_end - (page * page_size)).write_counts;
    }

    // find the min & max write count and the element location
    auto min_iter = std::min_element(write_level.begin(), write_level.end());
    auto min_index = std::distance(write_level.begin(), min_iter);

    // increment the write count
    payload.write_counts = write_level[min_index] + 1;

    // erase the page with min write count
    FLASH_EraseInitTypeDef EraseInitStruct{0};
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.NbPages = 1;
    EraseInitStruct.Page = page_total - min_index;
    EraseInitStruct.Banks = FLASH_BANK_1;
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_SR_ERRORS);
    HAL_FLASH_Unlock();
    uint32_t PAGEError = 0;
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK) {
        HAL_FLASH_Lock();
        return false;
    }

    // write the data to the page
    uint32_t address = address_end - (min_index * page_size);
    uint32_t size{sizeof(Payload) / sizeof(uint64_t)};
    uint64_t *data_ptr = reinterpret_cast<uint64_t *>(&payload);
    for (size_t i = 0; i < size; i++) {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, data_ptr[i]) != HAL_OK) {
            HAL_FLASH_Lock();
            return false;
        }
        address += 8;  // 8 bytes of memory for double word = 64 bits
    }
    HAL_FLASH_Lock();
    return true;
}

Flash::Payload Flash::Read(uint32_t address) {
    Payload payload{};
    std::copy_n(reinterpret_cast<const uint8_t *>(address), sizeof(Payload), reinterpret_cast<uint8_t *>(&payload));
    return payload;
}

bool Flash::Purge() {
    // erase the page with min write count
    FLASH_EraseInitTypeDef EraseInitStruct{0};
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.NbPages = user_pages;
    EraseInitStruct.Page = page_total - user_pages + 1;
    EraseInitStruct.Banks = FLASH_BANK_1;
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_SR_ERRORS);
    HAL_FLASH_Unlock();
    uint32_t PAGEError = 0;
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK) {
        HAL_FLASH_Lock();
        return false;
    }
    HAL_FLASH_Lock();
    return true;
}

bool Flash::Init() {
    // erase all user page
    FLASH_EraseInitTypeDef EraseInitStruct{0};
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.NbPages = user_pages;
    EraseInitStruct.Page = page_total - user_pages + 1;
    EraseInitStruct.Banks = FLASH_BANK_1;
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_SR_ERRORS);
    HAL_FLASH_Unlock();
    uint32_t PAGEError = 0;
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK) {
        HAL_FLASH_Lock();
        return false;
    }

    // write the blank data to all user page
    Payload payload{};
    uint32_t size{sizeof(Payload) / sizeof(uint64_t)};
    uint64_t *data_ptr = reinterpret_cast<uint64_t *>(&payload);
    for (uint8_t page = 0; page < user_pages; page++) {
        uint32_t address = address_end - (page * page_size);
        for (size_t i = 0; i < size; i++) {
            if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, data_ptr[i]) != HAL_OK) {
                HAL_FLASH_Lock();
                return false;
            }
            address += 8;  // 8 bytes of memory for double word = 64 bits
        }
    }
    HAL_FLASH_Lock();
    return true;
}
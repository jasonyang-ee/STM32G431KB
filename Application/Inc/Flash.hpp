#ifndef APPLICATION_INC_FLASH_STM32G431KB
#define APPLICATION_INC_FLASH_STM32G431KB

#include <algorithm>
#include <array>
#include <cstdint>
#include "main.h"

// Header-only Flash interface with injectable Payload type
// Usage:
// struct MyPayload { double dac_level; uint16_t led_state; uint32_t write_counts; };
// Flash<MyPayload> flash;
// flash.Init(MyPayload{0,0,0});

template <typename PayloadType>
class Flash {
public:
    using Payload = PayloadType;

    Flash() = default;
    ~Flash() = default;

    /// @brief Inject custom memory parameters
    void setMemory(uint8_t last_page, uint32_t page_size, uint32_t user_pages) {
        this->last_page = last_page;
        this->page_size = page_size;
        this->user_pages = user_pages;
        last_page_address = 0x08000000 + (last_page * page_size);
    }

    // Save settings to flash
    bool Save(const Payload& payload) {
        return Write(payload);
    }

    // Load settings from flash
    Payload Load() {
        return Read(SelectLatestAddress());
    }

    // Erase all user pages
    bool Purge() {
        return ErasePages(user_pages, page_total - user_pages + 1);
    }

    // Initialize pages with a default payload
    bool Init(const Payload& defaultPayload) {
        if (!ErasePages(user_pages, page_total - user_pages + 1)) return false;
        for (uint32_t page = 0; page < user_pages; ++page) {
            auto addr = last_page_address - (page * page_size);
            if (!ProgramPayload(addr, defaultPayload)) return false;
        }
        return true;
    }

private:
    uint8_t last_page{}; //63
    uint32_t page_size{}; //0x800
    uint32_t user_pages{}; //3
    uint32_t last_page_address{};

    bool Write(const Payload& payload) {
        auto [addr, nextCount] = FindNextPage(payload);
        Payload p = payload;
        p.write_counts = nextCount;
        if (!ErasePages(1, page_total - GetPageIndex(addr))) return false;
        return ProgramPayload(addr, p);
    }

    Payload Read(uint32_t addr) const {
        Payload p;
        std::copy_n(reinterpret_cast<const uint8_t*>(addr), sizeof(Payload), reinterpret_cast<uint8_t*>(&p));
        return p;
    }

    // Helpers (implementation inline)
    std::pair<uint32_t, uint32_t> FindNextPage(const Payload& payload) {
        std::array<uint32_t, user_pages> counts{};
        for (uint8_t i = 0; i < user_pages; ++i) {
            counts[i] = Read(last_page_address - (i * page_size)).write_counts;
        }
        auto it = std::min_element(counts.begin(), counts.end());
        size_t idx = std::distance(counts.begin(), it);
        return {last_page_address - (idx * page_size), counts[idx] + 1};
    }

    uint32_t SelectLatestAddress() const {
        std::array<uint32_t, user_pages> counts{};
        for (uint8_t i = 0; i < user_pages; ++i) {
            counts[i] = Read(last_page_address - (i * page_size)).write_counts;
        }
        auto it = std::max_element(counts.begin(), counts.end());
        size_t idx = std::distance(counts.begin(), it);
        return last_page_address - (idx * page_size);
    }

    bool ErasePages(uint32_t num, uint32_t startPage) {
        FLASH_EraseInitTypeDef ei{FLASH_TYPEERASE_PAGES, startPage, num, FLASH_BANK_1};
        __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_SR_ERRORS);
        HAL_FLASH_Unlock();
        uint32_t err = 0;
        bool ok = (HAL_FLASHEx_Erase(&ei, &err) == HAL_OK);
        HAL_FLASH_Lock();
        return ok;
    }

    bool ProgramPayload(uint32_t addr, const Payload& p) {
        auto data = reinterpret_cast<const uint64_t*>(&p);
        for (size_t i = 0; i < sizeof(Payload)/sizeof(uint64_t); ++i) {
            if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr, data[i]) != HAL_OK) {
                HAL_FLASH_Lock();
                return false;
            }
            addr += 8;
        }
        HAL_FLASH_Lock();
        return true;
    }

    size_t GetPageIndex(uint32_t addr) const {
        return (last_page_address - addr) / page_size;
    }
};

#endif // APPLICATION_INC_FLASH_STM32G431KB

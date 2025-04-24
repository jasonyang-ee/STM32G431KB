#ifndef APPLICATION_INC_FLASH_STM32G431KB
#define APPLICATION_INC_FLASH_STM32G431KB

#include <algorithm>
#include <array>
#include <iterator>

#include "StateMachine.hpp"
#include "main.h"

class Flash {
   public:
    Flash();
    virtual ~Flash();
    struct Payload {
        double dac_level{0};
        uint16_t led_state{0};
        uint32_t write_counts{0};
    };

    bool Save();
    void Load();
    bool Purge();
    bool Init();

   private:
    static constexpr uint8_t last_page{63};
    static constexpr uint32_t page_size{0x800};
    static constexpr uint32_t user_pages{3};
    static constexpr uint32_t last_page_address{0x08000000 + (last_page * page_size)};

    bool Write(Payload payload);
    Payload Read(uint32_t address);
};

#endif /* APPLICATION_INC_FLASH_STM32G431KB */

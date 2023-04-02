#ifndef CORE_INC_FLASH
#define CORE_INC_FLASH

#include "algorithm"
#include "iterator"
#include "main.h"
#include "queue"

class Flash {
   public:
    Flash();
    virtual ~Flash();

    void Save();
    void Load();

   private:
    int32_t Write(const uint64_t*, uint8_t);
    std::queue<uint64_t> Read();

    // Begin write on last page last row.
    // 0x08020000 is the next page of last page.
    // Same as (0x0801FFFF - 0x8 + 1); if using datasheet provided value
    const uint32_t m_address_end{0x08020000 - 0x8};
    const uint8_t m_page_total{63};

    struct Config {
        uint64_t pad;  // Force Alignment
        uint32_t led_level;
        uint32_t led_scale;
    };

};

#endif /* CORE_INC_FLASH */

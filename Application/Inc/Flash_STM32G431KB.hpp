#ifndef APPLICATION_INC_FLASH_STM32G431KB
#define APPLICATION_INC_FLASH_STM32G431KB

#include "algorithm"
#include "array"
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
    // Update config_arr_size according to the sizeof(config)/sizeof(uint64_t)
	// ------------------- MUST EDIT -------------------
	#define config_arr_size 2
    union Config_Arr {
        struct Config {
            uint32_t led_level;
            uint32_t led_scale;
			uint8_t led_state;
        } config;

        uint64_t config_arr[config_arr_size];
    };

	// Begin write on last page last row.
    // 0x08020000 is the next page of last page.
    // Same as (0x0801FFFF + 1 - 0x8); if using datasheet provided value
	// ------------------- MUST EDIT -------------------
    const uint32_t m_address_end{0x08020000 - 0x8};
    const uint8_t m_page_total{63};


	int32_t Write(const uint64_t*, uint8_t);
    void Read(Flash::Config_Arr*, uint8_t);
};

#endif    /* APPLICATION_INC_FLASH_STM32G431KB */

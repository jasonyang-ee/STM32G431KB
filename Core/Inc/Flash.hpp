#ifndef CORE_INC_FLASH
#define CORE_INC_FLASH

#include "main.h"


#include "algorithm"
#include "iterator"
#include "queue"

class Flash {
   public:
    Flash();
    virtual ~Flash();
    int32_t Save(const uint64_t*, uint8_t);
	std::queue<uint64_t> Load();
    int32_t Unload();

	std::queue<uint64_t> m_flash_data{};

   private:
	const uint32_t m_address_begin{0x08000000};
	const uint32_t m_address_end{0x08020000 - 0x8};
	const uint8_t m_page_total{63};

};


#endif    /* CORE_INC_FLASH */

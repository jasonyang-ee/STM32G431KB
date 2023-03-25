#ifndef CORE_INC_FLASH
#define CORE_INC_FLASH

#include "main.h"

class Flash {
   public:
    Flash();
    virtual ~Flash();
    void Write(const uint64_t*, uint32_t);
	uint64_t* Read();

   private:
	const uint32_t m_address{0x0803F800};		// Page 250 to Page 255; 16KB Total
	uint64_t m_flash_data[32];
};


#endif    /* CORE_INC_FLASH */

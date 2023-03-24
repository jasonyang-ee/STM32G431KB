#ifndef CORE_INC_CLI
#define CORE_INC_CLI

#include "lwshell.h"
#include "string.h"

class CLI {
   public:
    CLI();
    virtual ~CLI();
    void init();
	bool parse();
    void setSize(uint16_t);
    static void output(const char*, lwshell*);

    static int32_t led(int32_t, char**);
    static int32_t help(int32_t, char**);

	private:
	uint16_t m_cmd_size;
};

#endif    /* CORE_INC_CLI */

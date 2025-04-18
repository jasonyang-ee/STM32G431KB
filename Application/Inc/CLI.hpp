#ifndef APPLICATION_INC_ACLI
#define APPLICATION_INC_ACLI

#include <cstdint>
#include <cstring>  // for strncpy, strlen

#include "StateMachine.hpp"

class CLI {
   private:
    void func_help(int32_t argc, char** argv);
    void func_led(int32_t argc, char** argv);
    void func_flash(int32_t argc, char** argv);
    void func_info(int32_t argc, char** argv);
    void func_rot(int32_t argc, char** argv);
    void func_crc(int32_t argc, char** argv);
    void func_dac(int32_t argc, char** argv);
    void func_reset(int32_t argc, char** argv);

   private:
    static constexpr size_t MAX_CACHE_SIZE = 128;
    static constexpr size_t MAX_ARGS = 10;
    static constexpr size_t NUM_COMMANDS = 10;
    struct CommandEntry { const char* name; void (CLI::*func)(int32_t, char**); };
    CommandEntry commands[NUM_COMMANDS];
    char rx_cache[MAX_CACHE_SIZE];
    size_t rx_size{};

   public:
    CLI();
    virtual ~CLI();

    void saveCache(const char* rx) { strncpy(rx_cache, rx, MAX_CACHE_SIZE); rx_cache[MAX_CACHE_SIZE-1] = '\0'; rx_size = strlen(rx_cache); }

    bool parse() {
        // tokenizing rx_cache by space
        char* tokens[MAX_ARGS];
        size_t token_count = 0;
        char* token = strtok(rx_cache, " \n\r");
        while (token != nullptr && token_count < MAX_ARGS) {
            tokens[token_count++] = token;
            token = strtok(nullptr, " \n\r");
        }

        // parse command and arguments
        if (token_count > 0) {
            const char* command = tokens[0];
            for (size_t i = 0; i < NUM_COMMANDS; ++i) {
                if (strcmp(commands[i].name, command) == 0) {
                    // Invoke command functions by first token
                    (this->*commands[i].func)(token_count, tokens);
                    return true;
                }
            }
            return false;
        }
        return true;
    }
};

#endif /* APPLICATION_INC_ACLI */

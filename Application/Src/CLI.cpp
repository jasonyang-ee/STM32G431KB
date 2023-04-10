#include "CLI.hpp"

#include "Instances.hpp"

CLI::CLI() {}

CLI::~CLI() {}

void CLI::init() {
    lwshell_init();
    lwshell_set_output_fn(&CLI::output);

    lwshell_register_cmd("help", &CLI::help, NULL);
    lwshell_register_cmd("led", &CLI::led, NULL);
    lwshell_register_cmd("flash", &CLI::flash, NULL);
    lwshell_register_cmd("motor", &CLI::motor, NULL);
    lwshell_register_cmd("show", &CLI::show, NULL);
}

/**
 * @brief Parse c++ wrapper function to call lwshell_input
 *
 * @return bool: repeat lwshell parsing result
 */
bool CLI::parse() {
    if (lwshell_input(serialCOM.m_rx_data, m_cmd_size) == lwshellOK) return true;
    return false;
}

/**
 * @brief Buffer received command size
 *
 * This is needed to correctly process right amount of char
 *
 * @param size UART rx interrupt size
 */
void CLI::setSize(uint16_t size) { m_cmd_size = size; }

/**
 * @brief Output c++ wrapper function to echo cmd, parse result, and help menu
 *
 * @param str
 * @param lwobj
 */
void CLI::output(const char* str, lwshell* lwobj) { serialCOM.sendString(str); }



// Start of user defined command list
// Nested command must be determined using if
// Make your unknow command return when not found

int32_t CLI::led(int32_t argc, char** argv) {
    // Detailed Menu
    const char* help_text =
        "\nLED Functions:\n"
        "  on\t\tTurns ON LED\n"
        "  off\t\tTurns OFF LED\n"
        "  breath\t\tLED in breath effect mode\n"
        "  blink\t\tLED in slow blink mode\n"
        "  rapid\t\tLED in fast blink mode\n"
        "  scale #value\tSet LED dimmer scale\n"
        "  level #value\tSet LED light level\n"
        "  add #value\tIncrease or Decrease LED light level\n\n";

    // Sub Command
    if (argc == 2) {
        if (!strcmp(argv[1], "help"))
            serialCOM.sendString(help_text);
        else if (!strcmp(argv[1], "on"))
            led_user.on();
        else if (!strcmp(argv[1], "off"))
            led_user.off();
        else if (!strcmp(argv[1], "breath"))
            led_user.breath();
        else if (!strcmp(argv[1], "blink"))
            led_user.blink();
        else if (!strcmp(argv[1], "rapid"))
            led_user.rapid();
        else
            serialCOM.sendString("Unknown Command\n");
    }
    // Sub Command with values
    if (argc == 3) {
        if (!isdigit(*argv[2]) && *argv[2] != '-')
            return 1;  // Check if number on input
        else if (!strcmp(argv[1], "level"))
            led_user.setLevel(atof(argv[2]));  // Apply input number
        else if (!strcmp(argv[1], "scale"))
            led_user.setScale(atof(argv[2]));  // Apply input number
        else if (!strcmp(argv[1], "add"))
            led_user.addLevel(atof(argv[2]));  // Apply input number
        else
            serialCOM.sendString("Unknown Command\n");
    }
    return 0;
}



int32_t CLI::flash(int32_t argc, char** argv) {
    // Detailed Menu
    const char* help_text =
        "\nFlash Functions:\n"
        "  load\tUse setting from flash\n"
        "  unload\tSave setting to flash\n\n";

    // Sub Command
    if (argc == 2) {
        if (!strcmp(argv[1], "help"))
            serialCOM.sendString(help_text);
        else if (!strcmp(argv[1], "save"))
            xTaskResumeFromISR(thread.app_save_config_handle);
        else if (!strcmp(argv[1], "load"))
            xTaskResumeFromISR(thread.app_load_config_handle);
        else
            serialCOM.sendString("Unknown Command\n");
    }

    return 0;
}


int32_t CLI::motor(int32_t argc, char** argv) {
    // Detailed Menu
    const char* help_text =
        "\nMotor Functions:\n"
        "  level #value\tSet LED light level\n"
        "  add #value\tIncrease or Decrease LED light level\n\n";

    // Sub Command
    if (argc == 3) {
        if (!strcmp(argv[1], "help"))
            serialCOM.sendString(help_text);
        else if (!strcmp(argv[1], "level"))
            motor_dac.setLevel(atof(argv[2]));
        else if (!strcmp(argv[1], "add"))
            motor_dac.addLevel(atof(argv[2]));
        else
            serialCOM.sendString("Unknown Command\n");
    }

    return 0;
}


int32_t CLI::show(int32_t argc, char** argv) {
    xTaskResumeFromISR(thread.app_telemetry_handle);
    return 0;
}

// Default lwshell has only -h option
// If using git style where help is better syntax,
// Then, you need to do this help menu manually
int32_t CLI::help(int32_t argc, char** argv) {
    const char* help_menu =
        "\nUsage:  led\t[help] [on] [off]\n"
        "\t\t[breath] [blink] [rapid]\n"
        "\t\t[scale #] [level #] [add #]\n"
        "\n"
        "\tflash\t[save] [load]\n"
        "\n"
        "\tshow\n";
    serialCOM.sendString(help_menu);
    return 0;
}

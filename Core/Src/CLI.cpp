#include "CLI.hpp"

#include "instances.hpp"

CLI::CLI() {}

CLI::~CLI() {}

void CLI::init() {
    lwshell_init();
    lwshell_set_output_fn(&CLI::output);

    lwshell_register_cmd("help", &CLI::help, NULL);
    lwshell_register_cmd("led", &CLI::led, NULL);
}

bool CLI::parse() {
    if (lwshell_input(serialCOM.m_rx_data, m_cmd_size) == lwshellOK) return true;
    return false;
}

void CLI::setSize(uint16_t size) {m_cmd_size = size;}

void CLI::output(const char* str, lwshell* lwobj) { serialCOM.sendString(str); }

int32_t CLI::led(int32_t argc, char** argv) {
    const char* led_help =
        "\nLED Functions:\n"
        "  on\tTurns ON LED\n"
        "  off\tTurns OFF LED\n"
        "  breath\tLED in breath effect mode\n"
        "  blink\tLED in slow blink mode\n"
        "  rapid\tLED in fast blink mode\n\n";

    if (!strcmp(argv[1], "help")) serialCOM.sendString(led_help);
    if (!strcmp(argv[1], "on")) led_user.on();
    if (!strcmp(argv[1], "off")) led_user.off();
    if (!strcmp(argv[1], "breath")) led_user.breath();
    if (!strcmp(argv[1], "blink")) led_user.blink();
    if (!strcmp(argv[1], "rapid")) led_user.rapid();
    return 0;
}

int32_t CLI::help(int32_t argc, char** argv) {
    const char* help_menu =
        "\nUsage:  led\t[help] [on] [off]\n"
        "\t\t[breath] [blink] [rapid]\n";
    serialCOM.sendString(help_menu);
    return 0;
}

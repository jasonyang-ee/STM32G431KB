#include "CLI.hpp"

#include "Instances.hpp"
#include <cstring> // for strcmp, strtok

CLI::CLI() {
    commands[0] = {"?", &CLI::func_help};
    commands[1] = {"-h", &CLI::func_help};
    commands[2] = {"help", &CLI::func_help};
    commands[3] = {"led", &CLI::func_led};
    commands[4] = {"flash", &CLI::func_flash};
    commands[5] = {"info", &CLI::func_info};
    commands[6] = {"rot", &CLI::func_rot};
    commands[7] = {"crc", &CLI::func_crc};
    commands[8] = {"dac", &CLI::func_dac};
    commands[9] = {"reset", &CLI::func_reset};
}

CLI::~CLI() {}

// Register commands and their respective functions
void CLI::setCommands() {
    cmd_map["?"] = [this](int32_t argc, char** argv) { func_help(argc, argv); };
    cmd_map["-h"] = [this](int32_t argc, char** argv) { func_help(argc, argv); };
    cmd_map["help"] = [this](int32_t argc, char** argv) { func_help(argc, argv); };
    cmd_map["led"] = [this](int32_t argc, char** argv) { func_led(argc, argv); };
    cmd_map["flash"] = [this](int32_t argc, char** argv) { func_flash(argc, argv); };
    cmd_map["info"] = [this](int32_t argc, char** argv) { func_info(argc, argv); };
    cmd_map["rot"] = [this](int32_t argc, char** argv) { func_rot(argc, argv); };
    // cmd_map["crc"] = [this](int32_t argc, char** argv) { func_crc(argc, argv); };
    cmd_map["dac"] = [this](int32_t argc, char** argv) { func_dac(argc, argv); };
    cmd_map["reset"] = [this](int32_t argc, char** argv) { func_reset(argc, argv); };
}

void CLI::func_help(int32_t argc, char** argv) {
    const char* help_text =
        "\nUsage:  led\t[help] [on] [off]\n"
        "\t\t[breath] [blink] [rapid]\n"
        "\t\t[scale #] [level #] [add #]\n"
        "\n"
        "\tflash\t[save] [load]\n"
        "\n"
        "\tdac\t[help] [level *] [add *]\n"
        "\n"
        "\tinfo\t[help] [on] [off]\n";
    serial.sendString(help_text);
}

void CLI::func_led(int32_t argc, char** argv) {
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

    if (argc == 1) {
        led_user.toggle();
    }
    if (argc > 1) {
        const char* arg = argv[1];
        if (strcmp(arg, "help") == 0 || strcmp(arg, "?") == 0 || strcmp(arg, "-h") == 0) {
            serial.sendString(help_text);
        } else if (strcmp(arg, "on") == 0) {
            led_user.on();
        } else if (strcmp(arg, "off") == 0) {
            led_user.off();
        } else if (strcmp(arg, "blink") == 0) {
            led_user.blink();
        } else if (strcmp(arg, "rapid") == 0) {
            led_user.rapid();
        } else if (strcmp(arg, "breath") == 0) {
            led_user.breath();
        } else if (strcmp(arg, "three") == 0) {
            led_user.three();
        } else {
            serial.sendString("Command not found\n");
        }
    }
}

void CLI::func_flash(int32_t argc, char** argv) {
    const char* help_text =
        "\nFlash Functions:\n"
        "  load\t\tse setting from flash\n"
        "  unload\t\tSave setting to flash\n\n";

    if (argc > 1) {
        const char* arg = argv[1];
        if (strcmp(arg, "help") == 0 || strcmp(arg, "?") == 0 || strcmp(arg, "-h") == 0) {
            serial.sendString(help_text);
        } else if (strcmp(arg, "save") == 0) {
            xTaskCreate(Thread::task<&Thread::flashSave>, "Flash Save", 200, NULL, 1, &thread.flashSave_handle);
        } else if (strcmp(arg, "load") == 0) {
            xTaskCreate(Thread::task<&Thread::flashLoad>, "Flash Load", 200, NULL, 1, &thread.flashLoad_handle);
        } else if (strcmp(arg, "purge") == 0) {
            if (flash.Purge()) {
                serial.sendString("Flash Purged\n");
            }
        } else if (strcmp(arg, "init") == 0) {
            if (flash.Init()) {
                serial.sendString("Flash Initialized\n");
            }
        } else {
            serial.sendString("Command not found\n");
        }
    }
}

void CLI::func_info(int32_t argc, char** argv) {
    const char* help_text =
        "\nPrint System Telemetry:\n"
        "  on\tStream Telemetry\n"
        "  off\tStop Telemetry\n\n";

    if (argc == 1) {
        SM<Thread>::triggerEvent(Thread::Event::SINGLE, thread.telemetry_sm);
    }
    if (argc > 1) {
        const char* arg = argv[1];
        if (strcmp(arg, "help") == 0 || strcmp(arg, "?") == 0 || strcmp(arg, "-h") == 0) {
            serial.sendString(help_text);
        } else if (strcmp(arg, "on") == 0) {
            SM<Thread>::triggerEvent(Thread::Event::START, thread.telemetry_sm);
        } else if (strcmp(arg, "off") == 0) {
            SM<Thread>::triggerEvent(Thread::Event::STOP, thread.telemetry_sm);
        } else {
            serial.sendString("Command not found\n");
        }
    }
}

void CLI::func_rot(int32_t argc, char** argv) {
    const char* help_text =
        "\nRotation Functions:\n"
        "  start\t\tStart Rotation\n"
        "  stop\t\tStop Rotation\n\n";

    if (argc > 1) {
        const char* arg = argv[1];
        if (strcmp(arg, "help") == 0 || strcmp(arg, "?") == 0 || strcmp(arg, "-h") == 0) {
            serial.sendString(help_text);
        } else if (strcmp(arg, "on") == 0) {
            SM<Thread>::triggerEvent(Thread::Event::START, thread.runner_sm);
        } else if (strcmp(arg, "off") == 0) {
            SM<Thread>::setState(Thread::State::OFF, thread.runner_sm);
        } else if (strcmp(arg, "set") == 0 && argc == 3) {
            SM<Thread>::setState(Thread::State::INIT, thread.runner_sm, int{std::stoi(argv[2])});
        } else {
            serial.sendString("Command not found\n");
        }
    }
}

void CLI::func_crc(int32_t argc, char** argv) {
    const char* help_text =
        "\nCRC Functions:\n"
        "  acc [#]\tAccumulate CRC\n"
        "  cal [#]\tCalculate CRC\n\n";

    if (argc > 1) {
        const char* arg = argv[1];
        if (strcmp(arg, "help") == 0 || strcmp(arg, "?") == 0 || strcmp(arg, "-h") == 0) {
            serial.sendString(help_text);
        } else if (strcmp(arg, "cal") == 0) {
            if (argc == 3) {
                uint8_t input = (uint8_t)atoi(argv[2]);
                uint8_t data[1] = {input};
                SM<Thread>::setState(Thread::State::CRC_CAL, thread.runner_sm, data, 1);
            }
        } else {
            serial.sendString("Command not found\n");
        }
    }
}

void CLI::func_dac(int32_t argc, char** argv) {
    const char* help_text =
        "\nDAC Functions:\n"
        "  level [value]\tSet DAC level\n"
        "  add [value]\tAdd to DAC level\n"
        "  sine [amplitude] [frequency] [samplingRate]\tGenerate sine wave\n\n";

    if (argc == 1) {
        serial.sendString(help_text);
    }
    if (argc > 1) {
        const char* arg = argv[1];
        if (strcmp(arg, "help") == 0 || strcmp(arg, "?") == 0 || strcmp(arg, "-h") == 0) {
            serial.sendString(help_text);
        } else if (strcmp(arg, "level") == 0 && argc == 3) {
            double value = std::stod(argv[2]);
            dac.setLevel(value);
        } else if (strcmp(arg, "add") == 0 && argc == 3) {
            double value = std::stod(argv[2]);
            dac.addLevel(value);
        } else if (strcmp(arg, "sine") == 0 && argc == 5) {
            double amplitude = std::stod(argv[2]);
            double frequency = std::stod(argv[3]);
            double samplingRate = std::stod(argv[4]);
            dac.generateSineWave(amplitude, frequency, samplingRate);
        } else {
            serial.sendString("Command not found\n");
        }
    }
}

void CLI::func_reset(int32_t argc, char** argv) { NVIC_SystemReset(); }
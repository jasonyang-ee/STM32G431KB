#include "CLI.hpp"

#include "Instances.hpp"

CLI::CLI() { setCommands(); }

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
    std::string help_text =
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

    // No Sub Command
    if (argc == 1) {
        led_user.toggle();
    }

    // Sub Command
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "help" || arg == "?" || arg == "-h") {
            serial.sendString(help_text);
        } else if (arg == "on") {
            led_user.on();
        } else if (arg == "off") {
            led_user.off();
        } else if (arg == "blink") {
            led_user.blink();
        } else if (arg == "rapid") {
            led_user.rapid();
        } else if (arg == "breath") {
            led_user.breath();
        } else if (arg == "three") {
            led_user.three();
        } else {
            serial.sendString("Command not found\n");
        }
    }
}

void CLI::func_flash(int32_t argc, char** argv) {
    // Detailed Menu
    const char* help_text =
        "\nFlash Functions:\n"
        "  load\t\tse setting from flash\n"
        "  unload\t\tSave setting to flash\n\n";

    // Sub Command
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "help" || arg == "?" || arg == "-h") {
            serial.sendString(help_text);
        } else if (arg == "save") {
            if (flash.Save()) {
                serial.sendString("Flash Saved\n");
            }
        } else if (arg == "load") {
            flash.Load();
        } else if (arg == "purge") {
            if (flash.Purge()) {
                serial.sendString("Flash Purged\n");
            }
        } else if (arg == "init") {
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

    // No Sub Command
    if (argc == 1) {
        thread.telemetry_sm.triggerEvent(Thread::Event::SINGLE);
    }

    // Sub Command
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "help" || arg == "?" || arg == "-h") {
            serial.sendString(help_text);
        } else if (arg == "on") {
            thread.telemetry_sm.triggerEvent(Thread::Event::START);
        } else if (arg == "off") {
            thread.telemetry_sm.triggerEvent(Thread::Event::STOP);
        } else {
            serial.sendString("Command not found\n");
        }
    }
}

void CLI::func_rot(int32_t argc, char** argv) {
    // Detailed Menu
    const char* help_text =
        "\nRotation Functions:\n"
        "  start\t\tStart Rotation\n"
        "  stop\t\tStop Rotation\n\n";

    // Sub Command
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "help" || arg == "?" || arg == "-h") {
            serial.sendString(help_text);
        } else if (arg == "on") {
            thread.runner_sm.triggerEvent(Thread::Event::START);
        } else if (arg == "off") {
            thread.runner_sm.setState(Thread::State::OFF);
        } else if (arg == "set" && argc == 3) {
            thread.runner_sm.setState(Thread::State::INIT, int{std::stoi(argv[2])});
        } else {
            serial.sendString("Command not found\n");
        }
    }
}

// void CLI::func_crc(int32_t argc, char** argv) {
//     // Detailed Menu
//     const char* help_text =
//         "\nCRC Functions:\n"
//         "  acc [#]\tAccumulate CRC\n"
//         "  cal [#]\tCalculate CRC\n\n";

//     // Sub Command
//     if (argc > 1) {
//         std::string arg = argv[1];
//         if (arg == "help" || arg == "?" || arg == "-h") {
//             serial.sendString(help_text);
//         } else if (arg == "cal") {
//             if (argc == 3) {
//                 uint8_t input = std::stoi(argv[2]);
//                 SM<Thread>::setState(Thread::State::CRC_CAL, thread.runner_sm, std::vector<uint8_t>{input});
//             }
//         } else {
//             serial.sendString("Command not found\n");
//         }
//     }
// }

void CLI::func_dac(int32_t argc, char** argv) {
    // Detailed Menu
    const char* help_text =
        "\nDAC Functions:\n"
        "  level [value]\tSet DAC level\n"
        "  add [value]\tAdd to DAC level\n"
        "  sine [amplitude] [frequency] [samplingRate]\tGenerate sine wave\n\n";

    // No Sub Command
    if (argc == 1) {
        serial.sendString(help_text);
    }

    // Sub Command
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "help" || arg == "?" || arg == "-h") {
            serial.sendString(help_text);
        } else if (arg == "level" && argc == 3) {
            double value = std::stod(argv[2]);
            dac.setLevel(value);
        } else if (arg == "add" && argc == 3) {
            double value = std::stod(argv[2]);
            dac.addLevel(value);
        } else if (arg == "sine" && argc == 5) {
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
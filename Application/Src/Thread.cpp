#include "Thread.hpp"

#include "Instances.hpp"

Thread::Thread() {
    // Main State Machine
    // clang-format off
	thread_sm = {
		State::INIT,
		{ // Entry definition: (TargetState, GuardFunction, ActionFunction, Injection)
			{ State::INIT,	{},	actionSystemInit(), {} },
		},
		{ // Transition definition: (CurrentState, Event, NextState, GuardFunction, ActionFunction, Injection)
			{ State::INIT,			Event::INIT_DONE,		State::CREATE_TASKS,	{},	actionSystemRun(), {} },
			{ State::CREATE_TASKS,	Event::CREATE_DONE,		State::IDLE,			{},	{}, {} },
		}
	};
    // clang-format on

    // Telemetry State Machine
    // clang-format off
	telemetry_sm = {
		State::OFF,
		{ // Entry definition: (TargetState, GuardFunction, ActionFunction, Injection)
			{ State::ON,		{},	actionTelStart(),	{} },
			{ State::OFF,		{},	actionTelStop(),	{} },
			{ State::SINGLE,	{},	actionTelStart(),	{} },
		},
		{ // Transition definition: (CurrentState, Event, NextState, GuardFunction, ActionFunction, Injection)
			{ State::OFF,		Event::START,		State::ON,		{},	actionTelStart(),	{} },
			{ State::OFF,		Event::SINGLE,		State::SINGLE,	{},	actionTelStart(),	{} },
			{ State::ON,		Event::SINGLE,		State::OFF,		{},	actionTelStop(),	{} },
			{ State::ON,		Event::STOP,		State::OFF,		{},	actionTelStop(),	{} },
			{ State::ON,		Event::TASK_DONE,	State::ON,		{},	actionTelDelay(),	{} },
			{ State::SINGLE,	Event::TASK_DONE,	State::OFF,		{},	actionTelStop(),	{} },
		}
	};
    // clang-format on

    // clang-format off
	runner_sm = {
		State::OFF,
		{ // Entry definition: (TargetState, GuardFunction, ActionFunction, Injection)
			{ State::OFF,		{},	actionRotationStop(),	{} },
            { State::INIT,		{},	actionRotationStart(),	{} },
			{ State::CRC_CAL,	{},	actionCRCStart(),		{} },
		},
		{ // Transition definition: (CurrentState, Event, NextState, GuardFunction, ActionFunction, Injection)
			{ State::OFF,		Event::START,		State::R1,		{},	actionRotationStart(),	{} },
			{ State::INIT,		Event::TASK_DONE,	State::ON,		{},	actionRotate(),			{} },
			{ State::ON,		Event::TASK_DONE,	State::OFF,		{},	actionRotationStop(),	{} },
			{ State::R1,		Event::TASK_DONE,	State::R2,		{},	actionRotate(),			{1} },
			{ State::R2,		Event::TASK_DONE,	State::R3,		{},	actionRotate(),			{2} },
			{ State::R3,		Event::TASK_DONE,	State::R4,		{},	actionRotate(),			{3} },
			{ State::R4,		Event::TASK_DONE,	State::R1,		{},	actionRotate(),			{4} },
			{ State::R1,		Event::STOP,		State::OFF,		{},	actionRotationStop(),	{} },
			{ State::CRC_CAL,	Event::TASK_DONE,	State::OFF,		{},	actionCRCStop(),		{} },
		}
	};
    // clang-format on

    // Set initial state
    SM<Thread>::setState(State::INIT, thread_sm);
}

Thread::~Thread() {}

// Initialize System
void Thread::init() {
    while (1) {
		dac.init();
        flash.Load();
        SM<Thread>::triggerEvent(Event::INIT_DONE, thread_sm);
        vTaskDelete(NULL);
    }
}
Action Thread::actionSystemInit() {
	return [this]() {
        xTaskCreate(task<&Thread::init>, "system init", 800, this, 6, &init_handle);
    };
}
Action Thread::actionSystemRun() {
    return [this]() {
        xTaskCreate(task<&Thread::serialTX>, "serial send tx", 100, this, 1, &serial_handle);
		xTaskCreate(task<&Thread::parse>, "cli parsing", 200, this, 2, &parse_handle);
		xTaskCreate(task<&Thread::schedule_20Hz>, "schedule 20Hz", 64, this, 5, &schedule_20Hz_handle);
		xTaskCreate(task<&Thread::telemetry>, "telemetry", 400, this, 3, &telemetry_handle);
        vTaskSuspend(telemetry_handle);
		xTaskCreate(task<&Thread::runner>, "task simulation", 200, this, 3, &runner_handle);
        vTaskSuspend(runner_handle);
		xTaskCreate(task<&Thread::calculator>, "calculator", 1000, this, 5, &calculator_handle);
        vTaskSuspend(calculator_handle);
		xTaskCreate(task<&Thread::dacUpdate>, "dacUpdate", 64, this, 4, &dacUpdate_handle);
        vTaskSuspend(dacUpdate_handle);
		serial.sendString("\n\nSystem Boot OK\n");
		SM<Thread>::triggerEvent(Event::CREATE_DONE, thread_sm);
    };
}

// Telemetry
void Thread::telemetry() {
    while (1) {
        serial.sendString("\n-------- System Telemetry --------");
        serial.sendString("\nDAC Target Value:\t");
        serial.sendNumber(dac.getLevel());
        serial.sendString("\nADC Sensing Value:\t");
        serial.sendNumber(adc.volt_from_dac);
        serial.sendString("\nDate - Time:\t");
        serial.sendNumber(rtc.getDate());
        serial.sendString("  -  ");
        serial.sendNumber(rtc.getTime());
        serial.sendString("\nCurrent Free Heap: ");
        serial.sendNumber(xPortGetFreeHeapSize());
        serial.sendString("\nMinimum Free Heap: ");
        serial.sendNumber(xPortGetMinimumEverFreeHeapSize());
        serial.sendLn();

        SM<Thread>::triggerEvent(Event::TASK_DONE, telemetry_sm);
    }
}
Action Thread::actionTelStart() {
    return [this]() {
        BaseType_t xYieldRequired = pdTRUE;
        xTaskResumeFromISR(telemetry_handle);
        portYIELD_FROM_ISR(xYieldRequired);
    };
}
Action Thread::actionTelStop() {
    return [this]() { vTaskSuspend(telemetry_handle); };
}
Action Thread::actionTelDelay() {
    return [this]() { vTaskDelay(thread.telemetry_delay); };
}

// DAC Update
void Thread::dacUpdate() {
    while (1) {
        serial.sendString("dac updating\n");
        dac.schedule();
        vTaskDelay(1000);
    }
}

void Thread::schedule_20Hz() {
    while (1) {
        led_user.scheduler();
        HAL_IWDG_Refresh(&hiwdg);
        vTaskDelay(50);
    }
}
void Thread::serialTX() {
    while (1) {
        ulTaskNotifyTake(pdTRUE, 300);
        serial.commit();
    }
}

void Thread::parse() {
    while (1) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (!cli.parse()) {
            serial.sendString("Command not found\n");
        }
    }
}

void Thread::runner() {
    while (1) {
        serial.sendString("Running\n");
        SM<Thread>::triggerEvent(Event::TASK_DONE, runner_sm);
    }
}
Action Thread::actionRotationStart() {
    return [this]() {
        BaseType_t xYieldRequired = pdTRUE;
        xTaskResumeFromISR(runner_handle);
        portYIELD_FROM_ISR(xYieldRequired);
    };
}
Action Thread::actionRotationStop() {
    return [this]() {
        led_user.off();
        vTaskSuspend(runner_handle);
    };
}
Action Thread::actionRotate() {
    return [this]() {
        int order{0};
        if (runner_sm.injections.size() > 0) {
            order = std::any_cast<int>(runner_sm.injections[0]);
        }
        switch (order) {
            case 1:
                led_user.rapid();
                break;
            case 2:
                led_user.off();
                break;
            case 3:
                led_user.three();
                break;
            case 4:
                led_user.breath();
                break;
            default:
                led_user.off();
                break;
        }
        vTaskDelay(1000);
    };
}

void Thread::calculator() {
    while (1) {
        // get std::vector<uint8> input from injection
        std::vector<uint8_t> input = std::any_cast<std::vector<uint8_t>>(runner_sm.injections[0]);
        crc.setPolynomial(0x9B, 8);
        crc.setInitValue(0x00);
        auto result = crc.calculate(input.data(), 2);
        serial.sendString("CRC Result: ");
        serial.sendNumber(result);
        serial.sendLn();
        SM<Thread>::triggerEvent(Event::TASK_DONE, runner_sm);
    }
}
Action Thread::actionCRCStart() {
    return [this]() {
        BaseType_t xYieldRequired = pdTRUE;
        xTaskResumeFromISR(calculator_handle);
        portYIELD_FROM_ISR(xYieldRequired);
    };
}
Action Thread::actionCRCStop() {
    return [this]() { vTaskSuspend(calculator_handle); };
}
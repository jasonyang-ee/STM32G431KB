

#include "Thread.hpp"

#include "Instances.hpp"

Thread::Thread() {
    // Template for creating task
    // auto _task = [](void *arg) { static_cast<Thread *>(arg)->(); };
    // xTaskCreate(_task, "", 512, this, 2, &_Handle);

    auto t1 = [](void *arg) { static_cast<Thread *>(arg)->parse(); };
    xTaskCreate(t1, "cli parsing", 512, this, 2, &parse_handle);

    auto t2 = [](void *arg) { static_cast<Thread *>(arg)->init(); };
    xTaskCreate(t2, "system init", 512, this, 6, &init_handle);

    auto t3 = [](void *arg) { static_cast<Thread *>(arg)->utilities(); };
    xTaskCreate(t3, "utilities", 128, this, 1, &utilities_handle);

    auto a1 = [](void *arg) { static_cast<Thread *>(arg)->app_telemetry(); };
    xTaskCreate(a1, "showing", 128, this, 1, &app_telemetry_handle);

    auto a2 = [](void *arg) { static_cast<Thread *>(arg)->app_dac(); };
    xTaskCreate(a2, "dac + adc", 128, this, 3, &app_dac_handle);

    auto s1 = [](void *arg) { static_cast<Thread *>(arg)->schedule_20Hz(); };
    xTaskCreate(s1, "schedule 20Hz", 128, this, 2, &schedule_20Hz_handle);

    auto s2 = [](void *arg) { static_cast<Thread *>(arg)->serial_send(); };
    xTaskCreate(s2, "serial send out", 128, this, 3, &serial_send_handle);
}

Thread::~Thread() {}

void Thread::parse() {
    while (1) {
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
        cli.parse();
    }
}

void Thread::init() {
    while (1) {
        cli.init();
        motor_dac.init();
        flash.Load();
        motor_dac.setLevel(0);
        vTaskSuspend(NULL);
    }
}


void Thread::utilities() {
    while (1) {
        vTaskSuspend(NULL);
    }
}

void Thread::app_telemetry() {
    while (1) {
        // Show
        vTaskSuspend(NULL);
    }
}

void Thread::app_dac() {
    while (1) {
        // ADC + DAC
        if (sensor_adc.getValue() > 3000)
            motor_dac.setLevel(0);
        else
            motor_dac.addLevel(0.1);

        serialCOM.sendString("DAC value: ");
        serialCOM.sendNumber(motor_dac.getLevel());
        serialCOM.sendString("\nADC value: ");
        serialCOM.sendNumber(sensor_adc.getVolt());
        serialCOM.sendLn();

        // vTaskSuspend(NULL);
        vTaskDelay(10000);
    }
}

void Thread::schedule_20Hz() {
    while (1) {
        led_user.scheduler();
        // vTaskResume(serial_send_handle);
        vTaskDelay(50);
    }
}
void Thread::serial_send() {
    while (1) {
        // vTaskSuspend(NULL);
		ulTaskNotifyTake(pdFALSE, 100);	// 20Hz Check Timmer
        serialCOM.sendOut();
    }
}
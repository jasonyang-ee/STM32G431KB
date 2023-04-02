

#include "Thread.hpp"

#include "Instances.hpp"

Thread::Thread() {
    // Template for creating task
    // auto _task = [](void *arg) { static_cast<Thread *>(arg)->(); };
    // xTaskCreate(_task, "", 512, this, 2, &_Handle);

    auto t1 = [](void *arg) { static_cast<Thread *>(arg)->parse(); };
    xTaskCreate(t1, "cli parsing", 256, this, -2, &parse_Handle);

    auto t2 = [](void *arg) { static_cast<Thread *>(arg)->app_1(); };
    xTaskCreate(t2, "app 1", 128, this, 0, &app_1_Handle);

    auto t3 = [](void *arg) { static_cast<Thread *>(arg)->app_2(); };
    xTaskCreate(t3, "app 2", 128, this, 0, &app_2_Handle);

    auto t6 = [](void *arg) { static_cast<Thread *>(arg)->app_3(); };
    xTaskCreate(t6, "app 3", 128, this, 0, &app_3_Handle);

    auto t4 = [](void *arg) { static_cast<Thread *>(arg)->schedule_20Hz(); };
    xTaskCreate(t4, "schedule 20Hz", 128, this, -2, &schedule_20Hz_Handle);

    auto t5 = [](void *arg) { static_cast<Thread *>(arg)->serial_send(); };
    xTaskCreate(t5, "serial send out", 128, this, 0, &serial_send_Handle);
}

Thread::~Thread() {}

void Thread::parse() {
    while (1) {
		vTaskSuspend(NULL);
        cli.parse();
    }
}

void Thread::app_1() {
    while (1) {
		vTaskSuspend(NULL);
		flash.Save();
    }
}

void Thread::app_2() {
    while (1) {
		vTaskSuspend(NULL);
		flash.Load();
    }
}

void Thread::app_3() {
    while (1) {
		vTaskSuspend(NULL);
		serialCOM.sendNumber(led_user.getLevel());
		serialCOM.sendLn();
		serialCOM.sendNumber(led_user.getScale());
		serialCOM.sendLn();
    }
}

void Thread::schedule_20Hz() {
    while (1) {
        led_user.scheduler();
        vTaskResume(serial_send_Handle);
        vTaskDelay(50);
    }
}
void Thread::serial_send() {
    while (1) {
        vTaskSuspend(NULL);
        serialCOM.sendOut();
    }
}
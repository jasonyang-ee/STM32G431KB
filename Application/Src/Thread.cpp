

#include "Thread.hpp"

#include "Instances.hpp"

Thread::Thread() {
    // Template for creating task
    // auto _task = [](void *arg) { static_cast<Thread *>(arg)->(); };
    // xTaskCreate(_task, "", 512, this, 2, &_Handle);

    auto t1 = [](void *arg) { static_cast<Thread *>(arg)->parse(); };
    xTaskCreate(t1, "cli parsing", 256, this, -2, &parse_Handle);

    auto a1 = [](void *arg) { static_cast<Thread *>(arg)->app_1(); };
    xTaskCreate(a1, "app 1", 128, this, 0, &app_1_Handle);

    auto a2 = [](void *arg) { static_cast<Thread *>(arg)->app_2(); };
    xTaskCreate(a2, "app 2", 128, this, 0, &app_2_Handle);

    auto a3 = [](void *arg) { static_cast<Thread *>(arg)->app_3(); };
    xTaskCreate(a3, "app 3", 128, this, 0, &app_3_Handle);
	
    auto a4 = [](void *arg) { static_cast<Thread *>(arg)->app_4(); };
    xTaskCreate(a4, "app 4", 128, this, 0, &app_4_Handle);

    auto a5 = [](void *arg) { static_cast<Thread *>(arg)->app_5(); };
    xTaskCreate(a5, "app 4", 128, this, 0, &app_5_Handle);

    auto s1 = [](void *arg) { static_cast<Thread *>(arg)->schedule_20Hz(); };
    xTaskCreate(s1, "schedule 20Hz", 128, this, -2, &schedule_20Hz_Handle);

    auto s2 = [](void *arg) { static_cast<Thread *>(arg)->serial_send(); };
    xTaskCreate(s2, "serial send out", 128, this, 0, &serial_send_Handle);
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
		//save
		vTaskSuspend(NULL);
		flash.Save();
    }
}

void Thread::app_2() {
    while (1) {
		//load
		flash.Load();
		vTaskSuspend(NULL);
    }
}

void Thread::app_3() {
    while (1) {
		//Show
		vTaskSuspend(NULL);
		serialCOM.sendNumber(led_user.getLevel());
		serialCOM.sendLn();
		serialCOM.sendNumber(led_user.getScale());
		serialCOM.sendLn();
    }
}

void Thread::app_4() {
    while (1) {
		//motor
		motor_dac.setLevel(2.5);
		vTaskSuspend(NULL);
    }
}

void Thread::app_5() {
    while (1) {
		//motor
		motor_dac.addLevel(0.1);
		vTaskSuspend(NULL);
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
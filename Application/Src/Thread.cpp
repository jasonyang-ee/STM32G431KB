

#include "Thread.hpp"

#include "Instances.hpp"

Thread::Thread() {
    // Template for creating task
    // auto _task = [](void *arg) { static_cast<Thread *>(arg)->(); };
    // xTaskCreate(_task, "", 512, this, 2, &_Handle);

    auto t1 = [](void *arg) { static_cast<Thread *>(arg)->parse(); };
    xTaskCreate(t1, "cli parsing", 256, this, -2, &parse_handle);

    auto a1 = [](void *arg) { static_cast<Thread *>(arg)->app_save_config(); };
    xTaskCreate(a1, "saving config", 128, this, 0, &app_save_config_handle);

    auto a2 = [](void *arg) { static_cast<Thread *>(arg)->app_load_config(); };
    xTaskCreate(a2, "load config", 128, this, 0, &app_load_config_handle);

    auto a3 = [](void *arg) { static_cast<Thread *>(arg)->app_telemetry(); };
    xTaskCreate(a3, "showing", 512, this, 0, &app_telemetry_handle);
	
    auto a4 = [](void *arg) { static_cast<Thread *>(arg)->app_dac(); };
    xTaskCreate(a4, "dac_adc", 128, this, 0, &app_dac_handle);

    auto a5 = [](void *arg) { static_cast<Thread *>(arg)->app_5(); };
    xTaskCreate(a5, "app 5", 128, this, 0, &app_5_handle);

    auto s1 = [](void *arg) { static_cast<Thread *>(arg)->schedule_20Hz(); };
    xTaskCreate(s1, "schedule 20Hz", 128, this, -2, &schedule_20Hz_handle);

    auto s2 = [](void *arg) { static_cast<Thread *>(arg)->serial_send(); };
    xTaskCreate(s2, "serial send out", 128, this, 1, &serial_send_handle);
}

Thread::~Thread() {}

void Thread::parse() {
    while (1) {
		vTaskSuspend(NULL);
        cli.parse();
    }
}

void Thread::app_save_config() {
    while (1) {
		//save
		vTaskSuspend(NULL);
		flash.Save();
    }
}

void Thread::app_load_config() {
    while (1) {
		//load
		flash.Load();
		motor_dac.setLevel(2.0);
		vTaskSuspend(NULL);
    }
}

void Thread::app_telemetry() {
    while (1) {
		//Show
		vTaskSuspend(NULL);
		serialCOM.sendString("LED level: ");
		serialCOM.sendNumber(led_user.getLevel());
		serialCOM.sendString("\nLED scale: ");
		serialCOM.sendNumber(led_user.getScale());
		serialCOM.sendString("\nDAC value: ");
		serialCOM.sendNumber(motor_dac.getLevel());
		serialCOM.sendString("\nADC value: ");
		serialCOM.sendNumber(sensor_adc.getValue());
		serialCOM.sendLn();
    }
}

void Thread::app_dac() {
    while (1) {
		// ADC + DAC

		if (sensor_adc.getValue()>3000) motor_dac.setLevel(0);
		else motor_dac.addLevel(0.1);

		serialCOM.sendString("DAC value: ");
		serialCOM.sendNumber(motor_dac.getLevel());
		serialCOM.sendString("\nADC value: ");
		serialCOM.sendNumber(sensor_adc.getValue());
		serialCOM.sendLn();
		
		// vTaskSuspend(NULL);
		vTaskDelay(1000);
    }
}

void Thread::app_5() {
    while (1) {
		//motor
		vTaskSuspend(NULL);
    }
}


void Thread::schedule_20Hz() {
    while (1) {
        led_user.scheduler();
        vTaskResume(serial_send_handle);
        vTaskDelay(50);
    }
}
void Thread::serial_send() {
    while (1) {
        vTaskSuspend(NULL);
        serialCOM.sendOut();
    }
}
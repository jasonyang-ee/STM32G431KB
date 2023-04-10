#ifndef APPLICATION_INC_THREAD
#define APPLICATION_INC_THREAD

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

class Thread {
   public:
    Thread();
    virtual ~Thread();

    TaskHandle_t parse_handle;
    void parse();

    TaskHandle_t app_save_config_handle;
    void app_save_config();

    TaskHandle_t app_load_config_handle;
    void app_load_config();

    TaskHandle_t app_telemetry_handle;
    void app_telemetry();

    TaskHandle_t app_dac_handle;
    void app_dac();

    TaskHandle_t app_5_handle;
    void app_5();

    TaskHandle_t schedule_20Hz_handle;
    void schedule_20Hz();

    TaskHandle_t serial_send_handle;
    void serial_send();
};

#endif    /* APPLICATION_INC_THREAD */

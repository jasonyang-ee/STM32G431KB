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

	TaskHandle_t telemetry_human_handle;
	void telemetry_human();

    TaskHandle_t init_handle;
    void init();

    TaskHandle_t utilities_handle;
    void utilities();

    TaskHandle_t app_dac_handle;
    void app_dac();

    TaskHandle_t schedule_20Hz_handle;
    void schedule_20Hz();

    TaskHandle_t serial_send_handle;
    void serial_send();


	// Settings
	uint16_t m_stream_freq{500};
};

#endif    /* APPLICATION_INC_THREAD */

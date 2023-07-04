#ifndef APPLICATION_INC_THREAD
#define APPLICATION_INC_THREAD

#include "FreeRTOS.h"
#include "main.h"
#include "task.h"

class Thread {
   public:
    Thread();
    virtual ~Thread();

    TaskHandle_t telemetry_human_handle;
    void telemetry_human();

    /*---------------------------------------------------------------------------

            Applications

    ---------------------------------------------------------------------------*/

    TaskHandle_t idle_handle;
    void idle();

    TaskHandle_t dac_handle;
    void dacUpdate();

    /*---------------------------------------------------------------------------

            System initialization and periodic update

    ---------------------------------------------------------------------------*/

    TaskHandle_t init_handle;
    void init();

    TaskHandle_t schedule_20Hz_handle;
    void schedule_20Hz();

    TaskHandle_t serial_send_handle;
    void serial_send();

    TaskHandle_t parse_handle;
    void parse();

    /*---------------------------------------------------------------------------

            Thread Settings

    ---------------------------------------------------------------------------*/

	void idling_start();
	void dac_start();
	void dac_shutdown();

    uint16_t m_stream_freq{2000};
};

#endif /* APPLICATION_INC_THREAD */

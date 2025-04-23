#ifndef APPLICATION_INC_THREAD
#define APPLICATION_INC_THREAD

#include "FreeRTOS.h"
#include "StateMachine.hpp"  // now provides both SM<> and StateMachine<>
#include "main.h"
#include "task.h"

class Thread {
   public:
    Thread();
    virtual ~Thread();

    // your task‑wrapper
    template <void (Thread::*F)()>
    static void task(void* arg) {
        (static_cast<Thread*>(arg)->*F)();
    }

    // all your methods...
    void init();
    TaskHandle_t init_handle;
    void sendTX();
    TaskHandle_t sendTX_handle;
    void parseRX();
    TaskHandle_t parseRX_handle;
    void telemetry();
    TaskHandle_t telemetry_handle;
    void runner();
    TaskHandle_t runner_handle;
    void flashSave();
    TaskHandle_t flashSave_handle;
    void flashLoad();
    TaskHandle_t flashLoad_handle;
    void calculator();
    TaskHandle_t calculator_handle;
    void dacUpdate();
    TaskHandle_t dacUpdate_handle;
    void schedule_20Hz();
    TaskHandle_t schedule_20Hz_handle;
    void watchdog();
    TaskHandle_t watchdog_handle;

	
    enum class State { ON, OFF, SINGLE, INIT, CREATE_TASKS, IDLE, R0, R1, R2, R3, R4, CRC_CAL };
    enum class Event { START, STOP, TOGGLE, SINGLE, TASK_DONE, INIT_DONE, CREATE_DONE };

    SM<Thread>::StateMachine thread_sm;
    SM<Thread>::StateMachine telemetry_sm;
    SM<Thread>::StateMachine runner_sm;
    SM<Thread>::StateMachine dac_sm;

   private:
    // guards & actions…
    Action actionSystemInit();
    Action actionSystemRun();
    Action actionTelStart();
    Action actionTelStop();
    Action actionTelDelay();
    Action actionRotationStart();
    Action actionRotationStop();
    Action actionRotate();
    Action actionCRCStart();
    Action actionCRCStop();

    // State Machine Data
    uint16_t telemetry_delay{2000};
};

#endif /* APPLICATION_INC_THREAD */

#ifndef APPLICATION_INC_THREAD
#define APPLICATION_INC_THREAD

#include "FreeRTOS.h"
#include "StateMachine.hpp"
#include "main.h"
#include "task.h"

class Thread {
   public:
    Thread();
    virtual ~Thread();

    // Task Task Handle
    template <void (Thread::*TaskFunction)()>
    static void task(void *arg) {
        (static_cast<Thread *>(arg)->*TaskFunction)();
    }
    void init();
    TaskHandle_t init_handle;
    void serialTX();
    TaskHandle_t serial_handle;
    void parse();
    TaskHandle_t parse_handle;
    void telemetry();
    TaskHandle_t telemetry_handle;
    void runner();
    TaskHandle_t runner_handle;
	void flashSave();
	TaskHandle_t flashSave_handle;
	void flashLoad();
	TaskHandle_t flashLoad_handle;
    // void calculator();
    // TaskHandle_t calculator_handle;
    void dacUpdate();
    TaskHandle_t dacUpdate_handle;
    void schedule_20Hz();
    TaskHandle_t schedule_20Hz_handle;
    void watchdog();
    TaskHandle_t watchdog_handle;

    /*---------------------------------------------------------------------------

            State Machine for Thread Control

    ---------------------------------------------------------------------------*/
   public:
    // State and Event List
    enum class State { ON, OFF, SINGLE, INIT, CREATE_TASKS, IDLE, R0, R1, R2, R3, R4, CRC_CAL };
    enum class Event { START, STOP, TOGGLE, SINGLE, TASK_DONE, INIT_DONE, CREATE_DONE };

    // State Machine Mechanism
    template <typename Parent>
    friend class SM;
    using Entry = std::tuple<State, Guard, Action, Injection>;
    using Transition = std::tuple<State, Event, State, Guard, Action, Injection>;
    using Anonymous = std::tuple<State, State, Guard, Action, Injection>;
    struct StateMachine {
        State currentState;
        std::vector<Entry> entries;
        std::vector<Transition> transitions;
        std::vector<Anonymous> anonymous;
        Injection injections;
    };

    // State Machine Instances
    StateMachine thread_sm;
    StateMachine telemetry_sm;
    StateMachine runner_sm;
    StateMachine dac_sm;

   private:
    // Action and Guard Functions
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

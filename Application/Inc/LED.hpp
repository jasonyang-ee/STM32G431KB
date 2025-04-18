#ifndef APPLICATION_INC_LED
#define APPLICATION_INC_LED

#include "StateMachine.hpp"
#include "main.h"

/**
 * @brief Construct a new LED::led object
 * @param period Set PWM reload period.
 * @param frequency Set scheduler running frequency in Hz.
 * @warning setPort(&htimX->CCRX) is required to run.
 */

class LED {
   public:
    LED(uint16_t period, uint16_t frequency);
    virtual ~LED();

    // Public Methods
    void setPort(__IO uint32_t*);
    void on();
    void off();
    void toggle();
    void breath();
    void blink();
    void rapid();
    void three();
    void scheduler();
    void setDimmer(uint16_t dimmer);

   private:
    // Hardware Data
    __IO uint32_t* port;    // Ex: htim3.Instance->CCR2 for Timer3 Channel2
    uint16_t ratio{1};      // Max CCR value ratio to 100%
    uint16_t frequency{0};  // thread scheduler frequency
    uint16_t dimmer{1};     // User custom dimmer value 0 - 10

    // Controlling Data
    bool breath_direction{true};
    uint16_t on_percent{100};
    uint16_t breath_percent{0};
    uint16_t blink_timer{0};
    uint16_t rapid_timer{0};
    uint16_t three_timer_on{0};
    uint16_t three_timer_off{0};
    uint16_t three_counter{0};

    /*---------------------------------------------------------------------------

            State Machine for Thread Control

    ---------------------------------------------------------------------------*/
   public:
    // State and Event List
    enum class State { ON, OFF, BREATH, BLINK, RAPID, THREE };
    enum class Event { ON, OFF, TOGGLE, BREATH, BLINK, RAPID, SCHEDULE };

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
    StateMachine led_sm;

   private:
    // Action and Guard Functions
    Action actionOn();
    Action actionOff();
    Action actionToggle();
    Action actionBreath();
    Action actionBlink();
    Action actionRapid();
    Guard guardBlink();
    Guard guardRapid();
    Guard guardThree();
};

#endif /* APPLICATION_INC_LED */

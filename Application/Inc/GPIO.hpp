#ifndef APPLICATION_INC_GPIO
#define APPLICATION_INC_GPIO

#include "StateMachine.hpp"
#include "main.h"

/**
 * @brief Construct a new Switch:: Switch object
 *
 * @param freq Provide scheduler running frequency. 100Hz is good
 * @warning setPort(GPIOX, GPIO_PIN_X) is required to run.
 */
class CustomGPIO {
   public:
    CustomGPIO();
    CustomGPIO(uint16_t PollPeriod);
    virtual ~CustomGPIO();
    void setPort(GPIO_TypeDef *Port, uint16_t Pin);

    // Public Methods
    void on();
    void off();
    void toggle();
    bool getStatus();
    void scheduler();

    // Hardware Data
   private:
    GPIO_TypeDef *port;
    uint16_t pin;

    // State Machine
   public:
    enum class State { ON, OFF, LOW_INACTIVE, LOW_SETTLING, LOW_ACTIVE, HIGH_INACTIVE, HIGH_SETTLING, HIGH_ACTIVE };
    enum class Event { ON, OFF, TOGGLE, POLL };
    SM<CustomGPIO>::StateMachine gpio_sm;

    // Action and Guard
   private:
    Action actionOn();
    Action actionOff();
    Action actionToggle();

    Guard guardLowSense();
    Guard guardHighSense();
    Guard guardStable();

    // State Machine Data
    uint16_t roll_period{100};
    uint16_t roll_timer{0};
};

#endif /* APPLICATION_INC_GPIO */

#ifndef APPLICATION_INC_GPIO
#define APPLICATION_INC_GPIO

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
    CustomGPIO(uint16_t);
    virtual ~CustomGPIO();
    void setPort(GPIO_TypeDef *, uint16_t);

	void on();
	void off();
	void toggle();

    bool getStatus();

    void schedulerLowActive();
    void schedulerHighActive();

   private:
    GPIO_TypeDef *m_port;
    uint16_t m_pin;

    uint16_t m_roll_period{100};
    uint16_t m_roll_timer{0};
    bool m_stable{true};
    bool m_pressed{false};
};

#endif /* APPLICATION_INC_GPIO */

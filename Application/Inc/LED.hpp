#ifndef CORE_INC_LED
#define CORE_INC_LED

#include "main.h"

/**
 * @brief Construct a new LED::led object
 * @param int32_t(opt) Set PWM reload period.
 * @param int32_t(opt) Set scheduler running frequency in Hz.
 * @warning setPort(&htimX->CCRX) is required to run.
 *
 */
class LED {
   public:
    LED();
    LED(int32_t);
    LED(int32_t, int32_t);
    virtual ~LED();
    void setPort(__IO uint32_t *);

    // Unscoped to allow config saving to flash
    enum State {
        s_off,
        s_on,
        s_breath,
        s_blink,
        s_rapid,
    } state;

    // Value Configuration
    void setLevel(int32_t);
    void addLevel(int32_t);
    void setScale(int32_t);
    int32_t getLevel();
    int32_t getScale();
    void setState(uint8_t);
    uint8_t getState();

    void on();
    void off();
    void toggle();
    void breath();
    void blink();
    void rapid();
    void scheduler();

   private:
    void applyCCR();
    void zeroCCR();
    void setState(LED::State);

    // Setting
    __IO uint32_t *m_CCR;  // Ex: htim3.Instance->CCR2 for Timer3 Channel2
	int32_t m_CCR_ratio{1};
    int32_t m_level{100};  // light level
    int32_t m_scale{1};    // light scale
    uint16_t m_ext_freq{0};
    uint16_t m_schedule{0};

    uint16_t m_blink_timer{0};
    uint16_t m_rapid_timer{0};

    uint8_t m_breath_itr{0};
    uint16_t m_breath[25] = {0,  5,  10, 17, 29, 43, 60, 69, 77, 83, 91, 97, 99,
                             99, 84, 70, 55, 45, 37, 29, 23, 19, 16, 10, 0};
};

#endif /* CORE_INC_LED */

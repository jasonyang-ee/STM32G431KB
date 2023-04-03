#include "LED.hpp"

LED::LED() {}

LED::LED(int32_t pwm_period) { m_CCR_ratio = pwm_period / 100; }

LED::LED(int32_t pwm_period, int32_t freq) {
    m_CCR_ratio = pwm_period / 100;
    m_ext_freq = freq;
}

LED::~LED() { zeroCCR(); }

/**
 * @brief Initialize with Timer PWM CCR register address reference.
 * @param port Timer PWM CCR register.
 * @note Must use after HAL_TIM_PWM_Start().
 * @note Timer_1 Channel_3: setPort(&htim1.Instance->CCR3)
 */
void LED::setPort(__IO uint32_t *CCR) { m_CCR = CCR; }

/**
 * @brief Turn on LED
 *
 */
void LED::on() { setState(LED::State::s_on); }

/**
 * @brief Turn off LED
 *
 */
void LED::off() { setState(LED::State::s_off); }

/**
 * @brief Toggle LED on/off state
 *
 */
void LED::toggle() {
    if (*m_CCR > 0)
        zeroCCR();
    else
        applyCCR();
}

/**
 * @brief Activate breathing effect mode
 *
 */
void LED::breath() {setState(LED::State::s_breath);
}

/**
 * @brief Activate blinking effect mode
 *
 */
void LED::blink() {setState(LED::State::s_blink);
}

/**
 * @brief Activate rapid blinking effect mode
 *
 */
void LED::rapid() {setState(LED::State::s_rapid);
}

/**
 * @brief Set LED state for flash config loading
 *
 * @param uint8_t - 0=off, 1=on, 2=breath, 3=blink, 4=rapid
 */
void LED::setState(uint8_t cmd) { setState(static_cast<LED::State>(cmd)); }

/**
 * @brief Return current LED state for flash config saving
 *
 * @return uint8_t - 0=off, 1=on, 2=breath, 3=blink, 4=rapid
 */
uint8_t LED::getState() { return static_cast<uint8_t>(state); }

/**
 * @brief Set brightness dimming scale.
 *
 * @param int32_t dimming division value. CANNOT be 0.
 */
void LED::setScale(int32_t value) {
    if (value == 0) value = 1;  // Prevent dividing 0
    m_scale = value;
    applyCCR();
}

/**
 * @brief Set brightness level.
 *
 * @param int32_t - value from 0 to 100
 */
void LED::setLevel(int32_t value) {
    m_level = value;
    applyCCR();
}

int32_t LED::getScale() { return m_scale; }

int32_t LED::getLevel() { return m_level; }

/**
 * @brief Increase or Decrease current level
 *
 * @param value supports both positive and negative int32_t
 */
void LED::addLevel(int32_t value) {
    m_level += value;
    if (m_level > 1000) m_level = 1000;
    if (m_level < 0) m_level = 0;
    applyCCR();
}

/**
 * @brief Use in timer interrupt to allow active mode, breath, blink, etc..
 *
 * @note Timer interrupt frequency is set during object initializaiton.
 */
void LED::scheduler() {
    // Supporting both thread or timer interrupt active mode schedule for
    // consistant 20Hz
    if (m_ext_freq != 0) ++m_schedule %= (m_ext_freq / 20);

    // Active mode schedule logic
    if (m_schedule == 0) {
        if (state == LED::State::s_breath) {
            if (++m_breath_itr < 25)
                *m_CCR = m_breath[m_breath_itr] / m_scale * m_CCR_ratio;
            else
                m_breath_itr = 0;
        }

        // Slow Blinking LED Logic
        else if (state == LED::State::s_blink) {
            if (m_blink_timer > 5) {
                toggle();
                m_blink_timer = 0;
            } else
                m_blink_timer++;
        }

        // Fast Blinking LED Logic
        else if (state == LED::State::s_rapid) {
            if (m_rapid_timer > 1) {
                toggle();
                m_rapid_timer = 0;
            } else
                m_rapid_timer++;
        }
    }
}

// Private Functions

void LED::applyCCR() {
    *m_CCR = static_cast<uint32_t>(m_level / m_scale * m_CCR_ratio);
}

void LED::zeroCCR() { *m_CCR = 0; }

void LED::setState(LED::State cmd) {
    switch (cmd) {
        case LED::State::s_off:
            zeroCCR();
            break;
        case LED::State::s_on:
            applyCCR();
            break;
        case LED::State::s_breath:
            zeroCCR();
            break;
        case LED::State::s_blink:
            zeroCCR();
            break;
        case LED::State::s_rapid:
            zeroCCR();
            break;
    }
    state = cmd;
}

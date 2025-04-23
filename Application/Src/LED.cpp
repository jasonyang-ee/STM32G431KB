#include "LED.hpp"

/// @brief Construct LED with dimming ratio and update frequency.
/// @param period Divider base for duty cycle (percent basis).
/// @param freq Update event frequency in ticks.
LED::LED(uint16_t period, uint16_t freq) : ratio(period / 100), frequency(freq) {
    // clang-format off

	led_sm = {
		State::OFF,
		{ // Entry definition: (TargetState, GuardFunction, ActionFunction, Injection)
			{ State::ON,		{},	actionOn(), {} },
			{ State::OFF,		{},	actionOff(), {} },
			{ State::BREATH,	{},	actionBreath(), {} },
			{ State::BLINK,		{},	{}, {} },
			{ State::RAPID,		{},	{}, {} },
			{ State::THREE,		{},	actionOff(), {} },
		},
		{ // Transition definition: (CurrentState, Event, NextState, GuardFunction, ActionFunction, Injection)
			{ State::OFF,		Event::TOGGLE,		State::ON,		{},	actionOn(), {} },
			{ State::ON,		Event::TOGGLE,		State::OFF,		{},	actionOff(), {} },
			{ State::BREATH,	Event::SCHEDULE,	State::BREATH,	{},	actionBreath(), {} },
			{ State::BLINK,		Event::SCHEDULE,	State::BLINK,	guardBlink(),	actionToggle(), {} },
			{ State::RAPID,		Event::SCHEDULE,	State::RAPID,	guardRapid(),	actionToggle(), {} },
			{ State::THREE,		Event::SCHEDULE,	State::OFF,		guardThree(),	actionOff(), {} },
			{ State::BREATH,	Event::TOGGLE,		State::OFF,		{},	actionOff(), {} },
			{ State::BLINK,		Event::TOGGLE,		State::OFF,		{},	actionOff(), {} },
			{ State::RAPID,		Event::TOGGLE,		State::OFF,		{},	actionOff(), {} },
		}
	};
    // clang-format on

    // Set initial state
    led_sm.setState(State::BREATH);
}

/// @brief Destructor for LED instance, cleans up state machine.
LED::~LED() {}

// Public Methods
/// @brief Assign PWM compare register port for LED control.
/// @param CCR Pointer to timer CCR register.
void LED::setPort(__IO uint32_t *CCR) { port = CCR; }

/// @brief Transition LED to ON state.
void LED::on() { led_sm.setState(State::ON); }

/// @brief Transition LED to OFF state.
void LED::off() { led_sm.setState(State::OFF); }

/// @brief Transition LED to BREATH state.
void LED::breath() { led_sm.setState(State::BREATH); }

/// @brief Transition LED to BLINK state.
void LED::blink() { led_sm.setState(State::BLINK); }

/// @brief Transition LED to RAPID blink state.
void LED::rapid() { led_sm.setState(State::RAPID); }

/// @brief Execute three-blink sequence state.
void LED::three() { led_sm.setState(State::THREE); }

/// @brief Trigger state machine toggle event.
void LED::toggle() { led_sm.triggerEvent(Event::TOGGLE); }

/// @brief Trigger state machine schedule event.
void LED::scheduler() { led_sm.triggerEvent(Event::SCHEDULE); }

/// @brief Set brightness scaling factor.
/// @param value Divider to adjust duty cycle calculations.
void LED::setDimmer(uint16_t value) { dimmer = value; }

// Define Actions
/// @brief Action for turning LED fully on.
/// Sets CCR register value based on dimmer and ratio.
Action LED::actionOn() {
    return [this]() { *port = on_percent / dimmer * ratio; };
}

/// @brief Action for turning LED fully off.
Action LED::actionOff() {
    return [this]() { *port = 0; };
}

/// @brief Action for toggling LED output.
Action LED::actionToggle() {
    return [this]() {
        if (*port == 0)
            *port = on_percent / dimmer * ratio;
        else
            *port = 0;
    };
}

/// @brief Action for breath effect, alters duty cycle up/down.
Action LED::actionBreath() {
    return [this]() {
        if (breath_percent <= 100 && breath_direction) {
            breath_percent += 4;
        }
        if (breath_percent == 100 && breath_direction) {
            breath_direction = false;
        }
        if (breath_percent > 0 && !breath_direction) {
            breath_percent -= 10;
        }
        if (breath_percent == 0 && !breath_direction) {
            breath_direction = true;
        }
        *port = breath_percent / dimmer * ratio;
    };
}

// Define Guards
/// @brief Guard for blink timing interval.
/// @return true every N ticks to toggle output.
Guard LED::guardBlink() {
    return [this]() {
        if (blink_timer++ > 5) {
            blink_timer = 0;
            return true;
        }
        return false;
    };
}

/// @brief Guard for rapid blink timing interval.
/// @return true every M ticks to toggle output.
Guard LED::guardRapid() {
    return [this]() {
        if (rapid_timer++ > 1) {
            rapid_timer = 0;
            return true;
        }
        return false;
    };
}

/// @brief Guard for three-flash sequence completion.
/// @return true when three cycles complete, else false.
Guard LED::guardThree() {
    // blink 3 times
    return [this]() {
        if (three_counter == 3) {
            three_counter = 0;
            return true;
        }
        if (three_timer_off++ == 2) {
            (*actionToggle())();  // run actionToggle
        }
        if (three_timer_on++ == 5) {
            three_timer_on = 0;
            three_timer_off = 0;
            three_counter++;
            (*actionToggle())();  // run actionToggle
        }
        return false;
    };
}

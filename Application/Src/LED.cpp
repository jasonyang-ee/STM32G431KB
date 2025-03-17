#include "LED.hpp"

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
    SM<LED>::setState(State::BREATH, led_sm);
}

LED::~LED() {}

// Public Methods
void LED::setPort(__IO uint32_t *CCR) { port = CCR; }
void LED::on() { SM<LED>::setState(State::ON, led_sm); }
void LED::off() { SM<LED>::setState(State::OFF, led_sm); }
void LED::breath() { SM<LED>::setState(State::BREATH, led_sm); }
void LED::blink() { SM<LED>::setState(State::BLINK, led_sm); }
void LED::rapid() { SM<LED>::setState(State::RAPID, led_sm); }
void LED::three() { SM<LED>::setState(State::THREE, led_sm); }
void LED::toggle() { SM<LED>::triggerEvent(Event::TOGGLE, led_sm); }
void LED::scheduler() { SM<LED>::triggerEvent(Event::SCHEDULE, led_sm); }
void LED::setDimmer(uint16_t value) { dimmer = value; }

// Define Actions
Action LED::actionOn() {
    return [this]() { *port = on_percent / dimmer * ratio; };
}

Action LED::actionOff() {
    return [this]() { *port = 0; };
}
Action LED::actionToggle() {
    return [this]() {
        if (*port == 0)
            *port = on_percent / dimmer * ratio;
        else
            *port = 0;
    };
}
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
Guard LED::guardBlink() {
    return [this]() {
        if (blink_timer++ > 5) {
            blink_timer = 0;
            return true;
        }
        return false;
    };
}
Guard LED::guardRapid() {
    return [this]() {
        if (rapid_timer++ > 1) {
            rapid_timer = 0;
            return true;
        }
        return false;
    };
}
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

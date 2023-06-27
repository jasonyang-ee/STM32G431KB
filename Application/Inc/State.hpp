#ifndef APPLICATION_INC_STATE
#define APPLICATION_INC_STATE

#include "sml.hpp"

#include "DAC.hpp"

namespace sml = boost::sml;

// State
struct on;
struct off;
struct breathing;
    
// Events
struct start {};
struct stop {};
struct breath {};

struct MotorState {
	auto operator()() {
		using namespace sml;
	
		auto act_start = [](CustomDAC *motor_dac, Thread *thread) { };

		return make_transition_table(
			*state<off> + event<start> / act_start = state<on>,
			state<off> + event<breath> = state<breathing>,
			state<on> + event<stop> = state<off>,
			state<breathing> + event<stop> = state<off>
		);
	}
};

#endif    /* APPLICATION_INC_STATE */

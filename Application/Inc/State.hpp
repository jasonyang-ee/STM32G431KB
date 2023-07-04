#ifndef APPLICATION_INC_STATE
#define APPLICATION_INC_STATE

#include "DAC.hpp"
#include "sml.hpp"


namespace sml = boost::sml;

// State
struct begin {};
struct on;
struct off;
struct single_on {};
struct shutting_down {};

// Events
struct start {};
struct stop {};
struct toggle {};
struct oneshot {};
struct next {};
struct finish {};
struct shutdown {};
struct function_ended {};

// Application Events
struct dac_update {};

struct Idle {
    auto operator()() {
        using namespace sml;

        // State Machine Actions
        auto act_delay = []() { vTaskDelay(3000); };
        auto act_start = [](Thread *thread, SerialCOM *serial) {
            thread->idling_start();
            vTaskResume(thread->idle_handle);
			serial->sendString("ideling act_start completed\n");
        };
        auto act_shutdown = [](Thread *thread, SerialCOM *serial) {
            vTaskSuspend(thread->idle_handle);
			serial->sendString("ideling act_shutdown completed\n");
        };

        // State Machine Logics
        // clang-format off
		return make_transition_table(
			*state<begin> + event<start> / act_start = state<on>,
			state<on> + event<finish> / act_delay = state<on>,
			state<on> + event<shutdown> / act_shutdown = state<shutting_down>,
			state<shutting_down> / process(function_ended{}) = X
		);
        // clang-format on
    }
};

struct DACState {
    auto operator()() {
        using namespace sml;

        auto act_delay = []() { vTaskDelay(3000); };
        auto act_start = [](Thread *thread, SerialCOM *serial) {
            thread->dac_start();
            vTaskResume(thread->dac_handle);
			serial->sendString("dac act_start completed\n");
        };
        auto act_shutdown = [](Thread *thread, SerialCOM *serial) {
            thread->dac_shutdown();
            vTaskSuspend(thread->dac_handle);
			serial->sendString("dac act_shutdown completed\n");
        };

        // State Machine Logics
        // clang-format off
		return make_transition_table(
			*state<begin> / act_start = state<on>,
			state<on> + event<finish> / act_delay = state<on>,
			state<on> + event<shutdown> / act_shutdown = state<shutting_down>,
			state<shutting_down> / process(function_ended{}) = X,
			state<on> + event<toggle> / act_shutdown = state<off>,
			state<off> + event<toggle> = state<begin>
		);
        // clang-format on
    }
};

struct MainState {
    auto operator()() {
        using namespace sml;

        // State Machine Logics
        // clang-format off
		return make_transition_table(
			*state<begin> + event<start> = state<Idle>,
			state<Idle> + event<dac_update> = state<DACState>,
			state<DACState> + event<function_ended> = state<Idle>
		);
        // clang-format on
    }
};

struct StreamState {
    auto operator()() {
        using namespace sml;

        // State Machine Actions
        auto act_on = [](Thread *thread, SerialCOM *serial) {
            xTaskResumeFromISR(thread->telemetry_human_handle);
        };
        auto act_off = [](Thread *thread, SerialCOM *serial) {
            vTaskSuspend(thread->telemetry_human_handle);
        };
        auto act_delay = [](Thread *thread, SerialCOM *serial) { vTaskDelay(thread->m_stream_freq); };

        // State Machine Logics
        // clang-format off
		return make_transition_table(
			*state<off> + event<start> / act_on = state<on>,
			state<off> + event<toggle> / act_on = state<on>,
			state<off> + event<oneshot> / act_on = state<single_on>,
			state<single_on> + event<finish> / act_off = state<off>,
			state<on> + event<finish> / act_delay = state<on>,
			state<on> + event<stop> / act_off = state<off>,
			state<on> + event<toggle> / act_off = state<off>
		);
        // clang-format on
    }
};

#endif /* APPLICATION_INC_STATE */

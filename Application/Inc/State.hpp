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
struct oneshot {};
struct next {};
struct finish {};
struct shutdown {};
struct toggle {};
struct sub_endded {};

struct Idle {
    auto operator()() {
        using namespace sml;

        // State Machine Actions
        auto act_delay = []() { vTaskDelay(1000); };
        auto act_start = [](Thread *thread) {
            thread->idling_start();
            vTaskResume(thread->idling_handle);
        };
        auto act_shutdown = [](Thread *thread) {
            vTaskSuspend(thread->idling_handle);
        };

        // State Machine Logics
        // clang-format off
		return make_transition_table(
			*state<begin> / act_start = state<on>,
			state<on> + event<finish> / act_delay = state<on>,
			state<on> + event<shutdown> / act_shutdown = state<shutting_down>,
			state<shutting_down> / process(sub_endded{}) = X
		);
        // clang-format on
    }
};

struct DACState {
    auto operator()() {
        using namespace sml;

        auto act_delay = []() { vTaskDelay(1000); };
        auto act_start = [](Thread *thread) {
            thread->constant_run_start();
            vTaskResume(thread->constant_run_handle);
        };
        auto act_shutdown = [](Thread *thread) {
            thread->constant_run_shutdown();
            vTaskSuspend(thread->constant_run_handle);
        };

        // State Machine Logics
        // clang-format off
		return make_transition_table(
			*state<begin> / act_start = state<on>,
			state<on> + event<finish> / act_delay = state<on>,
			state<on> + event<shutdown> / act_shutdown = state<shutting_down>,
			state<shutting_down> / process(sub_endded{}) = X,
			state<on> + event<toggle> / act_shutdown = state<off>,
			state<off> + event<toggle> = state<begin>
		);
        // clang-format on
    }
};

struct MainState {
    auto operator()() {
        using namespace sml;

        auto act_start = [](Thread thread) {};
        auto act_shutdown = [](Thread thread) {};

        // State Machine Logics
        // clang-format off
		return make_transition_table(
			*state<begin> + event<start> / act_idle = state<Idle>,
			state<Idle> + event<next> / process(shutdown{}),
			state<Idle> + event<sub_endded> / act_constant = state<Constant>,
			state<Constant> + event<next> / process(shutdown{}),
			state<Constant> + event<sub_endded> / act_breathing = state<Breathing>,
			state<Breathing> + event<next> / process(shutdown{}),
			state<Breathing> + event<sub_endded> / act_idle = state<Idle>
		);
        // clang-format on
    }
};

struct StreamState {
    auto operator()() {
        using namespace sml;

        // State Machine Actions
        auto act_on = [](Thread *thread) {
            xTaskResumeFromISR(thread->telemetry_human_handle);
        };
        auto act_off = [](Thread *thread) {
            vTaskSuspend(thread->telemetry_human_handle);
        };
        auto act_delay = [](Thread *thread) { vTaskDelay(thread->m_stream_freq); };

        // State Machine Logics
        // clang-format off
		return make_transition_table(
			*state<off> + event<start> / act_on = state<on>,
			state<off> + event<oneshot> / act_on = state<single_on>,
			state<single_on> + event<finish> / act_off = state<off>,
			state<on> + event<finish> / act_delay = state<on>,
			state<on> + event<stop> / act_off = state<off>
		);
        // clang-format on
    }
};

#endif /* APPLICATION_INC_STATE */

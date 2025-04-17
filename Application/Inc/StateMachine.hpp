#ifndef APPLICATION_INC_STATEMACHINE
#define APPLICATION_INC_STATEMACHINE

#include <any>
#include <functional>
#include <optional>
#include <tuple>
#include <unordered_map>

using Injection = std::vector<std::any>;
using Action = std::optional<std::function<void()>>;
using Guard = std::optional<std::function<bool()>>;

template <typename Parent>
class SM {
   public:
    using State = typename Parent::State;
    using Event = typename Parent::Event;
    using StateMachine = typename Parent::StateMachine;

    template <typename... ExternalInjection>
    static void triggerEvent(Parent::Event event, Parent::StateMachine &sm, ExternalInjection... args) {
        auto it = std::ranges::find_if(sm.transitions.begin(), sm.transitions.end(), [&](const auto &t) {
            return std::get<0>(t) == sm.currentState && std::get<1>(t) == event;
        });

        if (it != sm.transitions.end()) {
            auto [curState, evt, nextState, guard, action, injection] = *it;
            updateInjection(sm, injection, std::forward<ExternalInjection>(args)...);
            handleStateChange(sm, nextState, guard, action);
        }
        runAnonymous(sm, std::forward<ExternalInjection>(args)...);
    }

    template <typename... ExternalInjection>
    static void setState(Parent::State state, Parent::StateMachine &sm, ExternalInjection... args) {
        auto it = std::ranges::find_if(sm.entries.begin(), sm.entries.end(),
                                       [&](const auto &entry) { return std::get<0>(entry) == state; });

        if (it != sm.entries.end()) {
            auto [targetState, guard, action, injection] = *it;
            updateInjection(sm, injection, std::forward<ExternalInjection>(args)...);
            handleStateChange(sm, state, guard, action);
        }
        runAnonymous(sm, std::forward<ExternalInjection>(args)...);
    }

    template <typename... ExternalInjection>
    static void runAnonymous(StateMachine &sm, ExternalInjection... args) {
        auto it = std::ranges::find_if(sm.anonymous.begin(), sm.anonymous.end(),
                                       [&](const auto &entry) { return std::get<0>(entry) == sm.currentState; });

        if (it != sm.anonymous.end()) {
            auto [fromState, nextState, guard, action, injection] = *it;
            updateInjection(sm, injection, std::forward<ExternalInjection>(args)...);
            handleStateChange(sm, nextState, guard, action);
        }
    }

    static Parent::State getState(Parent::StateMachine &sm) { return sm.currentState; }

   private:
    static void handleStateChange(Parent::StateMachine &sm, Parent::State nextState, Guard guard, Action action) {
        if (guard && !(*guard)()) return;
        sm.currentState = nextState;
        if (action) (*action)();
    }

    template <typename... ExternalInjection>
    static void updateInjection(Parent::StateMachine &sm, Injection injection, ExternalInjection &&...args) {
        if (!injection.empty()) {
            sm.injections = injection;
        } else if (sizeof...(args) > 0) {
            sm.injections = {std::any(std::forward<ExternalInjection>(args))...};
        }
    }
};

#endif /* APPLICATION_INC_STATEMACHINE */

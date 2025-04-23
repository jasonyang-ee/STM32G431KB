#ifndef APPLICATION_INC_STATEMACHINE
#define APPLICATION_INC_STATEMACHINE

#include <any>
#include <functional>
#include <optional>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <ranges>

using Injection = std::vector<std::any>;
using Action    = std::optional<std::function<void()>>;
using Guard     = std::optional<std::function<bool()>>;

template <typename Parent>
class SM;

template<typename Parent>
struct StateMachineContainer {
    using State      = typename Parent::State;
    using Event      = typename Parent::Event;
    using Entry      = std::tuple< State, Guard, Action, Injection >;
    using Transition = std::tuple< State, Event, State, Guard, Action, Injection >;
    using Anonymous  = std::tuple< State, State, Guard, Action, Injection >;

    State                  currentState;
    std::vector<Entry>      entries;
    std::vector<Transition> transitions;
    std::vector<Anonymous>  anonymous;
    Injection               injections;

    // Member convenience methods
    template<typename... ExternalInjection>
    void triggerEvent(typename Parent::Event event, ExternalInjection&&... args) {
        SM<Parent>::triggerEvent(event, *this, std::forward<ExternalInjection>(args)...);
    }
    template<typename... ExternalInjection>
    void setState(typename Parent::State state, ExternalInjection&&... args) {
        SM<Parent>::setState(state, *this, std::forward<ExternalInjection>(args)...);
    }
    template<typename... ExternalInjection>
    bool runAnonymous(ExternalInjection&&... args) {
        return SM<Parent>::runAnonymous(*this, std::forward<ExternalInjection>(args)...);
    }
};

template<typename Parent>
using StateMachine = StateMachineContainer<Parent>;

template <typename Parent>
class SM {
   public:
    using State = typename Parent::State;
    using Event = typename Parent::Event;
    using StateMachine = StateMachineContainer<Parent>;

    template <typename... ExternalInjection>
    static void triggerEvent(Parent::Event event, StateMachine &sm, ExternalInjection... args) {
        auto it = std::ranges::find_if(sm.transitions.begin(), sm.transitions.end(), [&](const auto &t) {
            return std::get<0>(t) == sm.currentState && std::get<1>(t) == event;
        });

        if (it != sm.transitions.end()) {
            auto [curState, evt, nextState, guard, action, injection] = *it;
            updateInjection(sm, injection, std::forward<ExternalInjection>(args)...);
            handleStateChange(sm, nextState, guard, action);
        }
        while(runAnonymous(sm, std::forward<ExternalInjection>(args)...)) {}
    }

    template <typename... ExternalInjection>
    static void setState(Parent::State state, StateMachine &sm, ExternalInjection... args) {
        auto it = std::ranges::find_if(sm.entries.begin(), sm.entries.end(),
                                       [&](const auto &entry) { return std::get<0>(entry) == state; });

        if (it != sm.entries.end()) {
            auto [targetState, guard, action, injection] = *it;
            updateInjection(sm, injection, std::forward<ExternalInjection>(args)...);
            handleStateChange(sm, state, guard, action);
        }
        while(runAnonymous(sm, std::forward<ExternalInjection>(args)...)) {}
    }

    template <typename... ExternalInjection>
    static bool runAnonymous(StateMachine &sm, ExternalInjection... args) {
        auto it = std::ranges::find_if(sm.anonymous.begin(), sm.anonymous.end(),
                                       [&](const auto &entry) { return std::get<0>(entry) == sm.currentState; });

        if (it != sm.anonymous.end()) {
            auto [fromState, nextState, guard, action, injection] = *it;
            updateInjection(sm, injection, std::forward<ExternalInjection>(args)...);
            handleStateChange(sm, nextState, guard, action);
			return true;
        }
		return false;
    }

    static Parent::State getState(StateMachine &sm) { return sm.currentState; }

   private:
    static void handleStateChange(StateMachine &sm, Parent::State nextState, Guard guard, Action action) {
        if (guard && !(*guard)()) return;
        sm.currentState = nextState;
        if (action) (*action)();
    }

    template <typename... ExternalInjection>
    static void updateInjection(StateMachine &sm, Injection injection, ExternalInjection &&...args) {
        if (!injection.empty()) {
            sm.injections = injection;
        } else if (sizeof...(args) > 0) {
            sm.injections = {std::any(std::forward<ExternalInjection>(args))...};
        }
    }
};

#endif /* APPLICATION_INC_STATEMACHINE */

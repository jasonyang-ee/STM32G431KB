#ifndef APPLICATION_INC_STATEMACHINE
#define APPLICATION_INC_STATEMACHINE

#include <any>
#include <functional>
#include <optional>
#include <ranges>
#include <tuple>
#include <unordered_map>
#include <vector>


/// @brief Values injected into actions and guards.
using Injection = std::vector<std::any>;

/// @brief Optional transition action callback.
/// @see std::function<void()>
using Action = std::optional<std::function<void()>>;

/// @brief Optional guard predicate for transitions.
/// @return true if transition is allowed.
using Guard = std::optional<std::function<bool()>>;

/// @brief Forward declaration of the state machine engine.
template <typename Parent>
class SM;

/// @brief Container for states, transitions, and injections of the state machine.
/// @tparam Parent Type defining State and Event.
template <typename Parent>
struct StateMachineContainer {
    using State = typename Parent::State;
    using Event = typename Parent::Event;
    using Entry = std::tuple<State, Guard, Action, Injection>;
    using Transition = std::tuple<State, Event, State, Guard, Action, Injection>;
    using Anonymous = std::tuple<State, State, Guard, Action, Injection>;

    /// @brief Current active state of the state machine.
    State currentState;

    /// @brief Registered state entries with guards and entry actions.
    std::vector<Entry> entries;

    /// @brief Event-driven transitions.
    std::vector<Transition> transitions;

    /// @brief Automatic transitions executed without an explicit event.
    std::vector<Anonymous> anonymous;

    /// @brief Values injected into callbacks during state changes.
    Injection injections;

    // Member convenience methods
    /// @brief Trigger a state machine event.
    /// @tparam ExternalInjection Types of additional data to inject.
    /// @param event Event to trigger.
    /// @param args Optional injection values.
    template <typename... ExternalInjection>
    void triggerEvent(typename Parent::Event event, ExternalInjection &&...args) {
        SM<Parent>::triggerEvent(event, *this, std::forward<ExternalInjection>(args)...);
    }

    /// @brief Set a new state directly, bypassing events.
    /// @tparam ExternalInjection Types of additional data to inject.
    /// @param state Target state to set.
    /// @param args Optional injection values.
    template <typename... ExternalInjection>
    void setState(typename Parent::State state, ExternalInjection &&...args) {
        SM<Parent>::setState(state, *this, std::forward<ExternalInjection>(args)...);
    }
};

template <typename Parent>
using StateMachine = StateMachineContainer<Parent>;

/// @brief State machine engine implementing transitions and state changes.
/// @tparam Parent Type defining State and Event.
template <typename Parent>
class SM {
   public:
    using State = typename Parent::State;
    using Event = typename Parent::Event;
    using StateMachine = StateMachineContainer<Parent>;

    /// @brief Process an event and transition state if matching.
    /// @tparam ExternalInjection Types of optional injection values.
    /// @param event Event to handle.
    /// @param sm StateMachine instance to operate on.
    /// @param args Additional values for injection.
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
        while (runAnonymous(sm, std::forward<ExternalInjection>(args)...)) {
        }
    }

    /// @brief Force set the state with injection values.
    /// @tparam ExternalInjection Types of optional injection values.
    /// @param state New state to set.
    /// @param sm StateMachine instance to operate on.
    /// @param args Additional values for injection.
    template <typename... ExternalInjection>
    static void setState(Parent::State state, StateMachine &sm, ExternalInjection... args) {
        auto it = std::ranges::find_if(sm.entries.begin(), sm.entries.end(),
                                       [&](const auto &entry) { return std::get<0>(entry) == state; });

        if (it != sm.entries.end()) {
            auto [targetState, guard, action, injection] = *it;
            updateInjection(sm, injection, std::forward<ExternalInjection>(args)...);
            handleStateChange(sm, state, guard, action);
        }
        while (runAnonymous(sm, std::forward<ExternalInjection>(args)...)) {
        }
    }

    /// @brief Retrieve the current state from the state machine.
    /// @param sm StateMachine instance.
    /// @return Current active state.
    static Parent::State getState(StateMachine &sm) { return sm.currentState; }

   private:
    /// @brief Execute an anonymous transition if the current state matches.
    /// @tparam ExternalInjection Types of optional injection values.
    /// @param sm StateMachine instance to operate on.
    /// @param args Additional values for injection.
    /// @return true if a transition occurred.
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

    /// @brief Evaluate guard and perform state change and action.
    /// @param sm Instance to update.
    /// @param nextState State to transition to.
    /// @param guard Optional guard predicate.
    /// @param action Optional action to execute.
    static void handleStateChange(StateMachine &sm, Parent::State nextState, Guard guard, Action action) {
        if (guard && !(*guard)()) return;
        sm.currentState = nextState;
        if (action) (*action)();
    }

    /// @brief Populate the injection vector for upcoming callbacks.
    /// @param sm Instance whose injections will be updated.
    /// @param injection Predefined injection values.
    /// @param args Additional values to inject.
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

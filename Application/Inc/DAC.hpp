#ifndef APPLICATION_INC_DAC
#define APPLICATION_INC_DAC

#include <array>
#include <cmath>

#include "StateMachine.hpp"
#include "main.h"

class CustomDAC {
   public:
    CustomDAC();
    virtual ~CustomDAC();
    void setPort(DAC_HandleTypeDef *, uint32_t);
    void setVoltRef(double);
    void init();

    void setLevel(double);
    void addLevel(double);
    double getLevel();

    void on();
    void off();
    void breath();
    void sine();
    void generateSineWave(double, double, double);

    void schedule();

    std::array<double, 1000> sineWaveArray;

   private:
    void applyLevel();
    void zeroLevel();

    double level{0.0};
    double vref{3.3};

    double amplitude{1.0};        // Amplitude of the sine wave
    double frequency{1.0};        // Frequency of the sine wave
    double samplingRate{1000.0};  // Sampling rate of the sine wave

    uint32_t breath_period{325};

    DAC_HandleTypeDef *port;
    uint32_t channel{DAC_CHANNEL_1};
    uint32_t alignment{DAC_ALIGN_12B_R};

    /*---------------------------------------------------------------------------

            State Machine for Thread Control

    ---------------------------------------------------------------------------*/
   public:
    // State and Event List
    enum class State { OFF, ON, BREATH, SINE };
    enum class Event { START, STOP, BREATH, SINE };

    // State Machine Mechanism
    template <typename Parent>
    friend class SM;
    using Entry = std::tuple<State, Guard, Action, Injection>;
    using Transition = std::tuple<State, Event, State, Guard, Action, Injection>;
    using Anonymous = std::tuple<State, State, Guard, Action, Injection>;
    struct StateMachine {
        State currentState;
        std::vector<Entry> entries;
        std::vector<Transition> transitions;
        std::vector<Anonymous> anonymous;
        Injection injections;
    };

    // State Machine Instance
    StateMachine dac_sm;

   private:
    // Action and Guard Functions
    Action actionOn();
    Action actionOff();
    Action actionBreath();
    Action actionSine();
};

#endif /* APPLICATION_INC_DAC */

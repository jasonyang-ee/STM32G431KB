#ifndef APPLICATION_INC_DAC
#define APPLICATION_INC_DAC

#include "main.h"

class MotorDAC {
   public:
    MotorDAC();
    virtual ~MotorDAC();
    void setPort(DAC_HandleTypeDef *, uint32_t);
    void setVoltRef(double);

	// Unscoped to allow config saving to flash
    enum State {
        s_off,
        s_on,
    } m_state;

    void setLevel(double);
    void addLevel(double);
    double getLevel();

	void setState(uint8_t);
    uint8_t getState();

    void on();
    void off();

   private:
    void applyLevel();
	void zeroLevel();
	void setState(MotorDAC::State);

    double m_level{0};
    double m_vref{3.3};

	/**
	 * @brief Using HAL predefined bit mask for DAC output options
	 * Options: DAC_CHANNEL_1, DAC_CHANNEL_2
	 * Options: DAC_ALIGN_12B_R, DAC_ALIGN_12B_L, DAC_ALIGN_8B_R
	 */
    DAC_HandleTypeDef *m_port{};
    uint32_t m_channel{DAC_CHANNEL_1};
    uint32_t m_alignment{DAC_ALIGN_12B_R};
};

#endif /* APPLICATION_INC_DAC */

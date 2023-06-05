#ifndef APPLICATION_INC_DAC
#define APPLICATION_INC_DAC

#include "main.h"

class CustomDAC {
   public:
    CustomDAC();
    virtual ~CustomDAC();
    void setPort(DAC_HandleTypeDef *, uint32_t);
    void setVoltRef(double);
	void init();

	// Unscoped to allow config saving to flash
    enum State {
        s_off,
        s_on,
		s_breath,
    } m_state;

    void setLevel(double);
    void addLevel(double);
    double getLevel();

	void setState(uint8_t);
    uint8_t getState();

    void on();
    void off();
	void breath();

	void schedule();

   private:
    void applyLevel();
	void zeroLevel();
	void setState(CustomDAC::State);

    double m_level{0.0};
    double m_vref{3.3};

	uint32_t m_breeath_period{325};

	/**
	 * @brief Using HAL predefined bit mask for DAC output options
	 * Options: DAC_CHANNEL_1, DAC_CHANNEL_2
	 * Options: DAC_ALIGN_12B_R, DAC_ALIGN_12B_L, DAC_ALIGN_8B_R
	 */
    DAC_HandleTypeDef *m_port;
    uint32_t m_channel{DAC_CHANNEL_1};
    uint32_t m_alignment{DAC_ALIGN_12B_R};
};

#endif /* APPLICATION_INC_DAC */

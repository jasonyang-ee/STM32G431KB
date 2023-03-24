#ifndef CORE_INC_SERIALCOM
#define CORE_INC_SERIALCOM

#include "algorithm"
#include "main.h"
#include "string"

#ifndef UART_BUFFER
#define UART_BUFFER 64
#endif

/**
 * @brief Construct a new Uart serial COM port
 *
 * @warning setPort(&huartX) is required to run.
 *
 * @note define UART_BUFFER to overwrite the default 64 byte
 */
class SerialCOM {
   public:
    SerialCOM();
    virtual ~SerialCOM();
    void setPort(UART_HandleTypeDef *);

    void sendLn();
    void sendString(std::string);
    void sendNumber(int);
    void sendNumber(float);
    void sendNumber(double);
    void sendNumber(uint8_t);
    void sendNumber(uint16_t);
    void sendNumber(uint32_t);
    void sendNumber(int8_t);
    void sendNumber(int16_t);
    void sendNumber(int32_t);

	
    bool sendOut();

    // Tx Rx Memory for DMA to fetch and push
    uint8_t m_tx_data[UART_BUFFER];  // UART_BUFFER may be re-defined in main.h
    uint8_t m_rx_data[UART_BUFFER];

   private:
    // Port
    UART_HandleTypeDef *m_port;

    // Data
    uint16_t m_tx_data_size{};
    std::string m_buffer_msg{};
};

#endif /* CORE_INC_SERIALCOM */

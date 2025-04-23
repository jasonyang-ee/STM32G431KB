#ifndef CORE_INC_SERIAL
#define CORE_INC_SERIAL

#include <algorithm>
#include <array>
#include <string>
#include <vector>

#include "main.h"

class Serial {
   public:
    Serial(){};
    virtual ~Serial(){};

    // Must be greater than protobuf payload size
    static constexpr size_t UART_BUFFER_SIZE{64};

    // Tx Rx Memory for DMA
    std::array<uint8_t, UART_BUFFER_SIZE * 4> tx;
    std::array<uint8_t, UART_BUFFER_SIZE> rx;

    /// @brief Initialize Serial
    /// @param Ports Vector of UART_HandleTypeDef pointer
    void setPort(std::vector<UART_HandleTypeDef *> Ports) { ports = std::move(Ports); }

	/// @brief Get high water mark
	/// @return High water mark
	/// @note High water mark is the maximum size of the tx_cache
	uint16_t getHighWaterMark() { return high_water_mark; }

    /// @brief Send a new line character
    void sendLn() { tx_cache.append("\n"); };

    /// @brief Buffer a string to be sent
    /// @param msg string to be sent
    void sendString(std::string msg) { tx_cache.append(msg); }

    /// @brief Sending number and bool
    /// @tparam T Data type
    /// @param value number to be buffered
    template <class T>
    void sendNumber(T value) {
        if (std::is_same<T, bool>::value) {
            value ? tx_cache.append("ON") : tx_cache.append("OFF");
        } else {
            tx_cache.append(std::to_string(value));
        }
    }

    /// @brief Sending 1D array
    /// @tparam T Data type
    /// @tparam N 1D array size
    /// @param elements Array to be buffered
    template <class T, size_t N>
    void sendNumber(std::array<T, N> elements) {
        for (auto &i : elements) {
            tx_cache.append(std::to_string(i));
            if (&i != &elements.back()) tx_cache.append("  ");
        }
    }

    /// @brief Sending 2D array
    /// @tparam T Data type
    /// @tparam N 2D array size
    /// @tparam M 1D array size
    /// @param elements Array to be buffered
    template <class T, size_t N, size_t M>
    void sendNumber(std::array<std::array<T, M>, N> elements) {
        for (auto &column : elements) {
            for (auto &element : column) {
                tx_cache.append(std::to_string(element));
                if (&element != &column.back()) tx_cache.append("  ");
            }
            if (&column != &elements.back()) tx_cache.append("\n");
        }
    }

    /// @brief Sending number with message
    /// @tparam T Data type
    /// @param msg Message to be sent
    /// @param value number to be buffered
    template <class T>
    void sendCombined(std::string msg, T value) {
        sendString(msg);
        sendString(":\t\t\t");
        sendNumber(value);
        sendLn();
    }

    /// @brief Sending 1D array with message
    /// @tparam T Data type
    /// @tparam N 1D array size
    /// @param msg Message to be sent
    /// @param elements Array to be buffered
    /// @param value number to be buffered
    template <class T, size_t N>
    void sendCombined(std::string msg, std::array<T, N> elements) {
        sendString(msg);
        sendString(":\t\t\t");
        sendNumber(elements);
        sendLn();
    }

    /// @brief Sending 2D array with message
    /// @tparam T Data type
    /// @tparam N 2D array size
    /// @tparam M 1D array size
    /// @param msg Message to be sent
    /// @param elements Array to be buffered
    /// @param value number to be buffered
    template <class T, size_t N, size_t M>
    void sendCombined(std::string msg, std::array<std::array<T, M>, N> elements) {
        sendString(msg);
        sendString(":\t\t\t");
        sendNumber(elements);
        sendLn();
    }

    /// @brief Buffer a protobuf payload to be sent
    /// @param msg protobuf payload
    /// @param size size of the payload
    void sendPB(uint8_t *msg, uint32_t size) {
        // copy msg into std::array of pb_cache
        std::copy(msg, msg + size, pb_cache.data());
        pb_size = size;
    }

    /// @brief Send out data with iterrupt mode
    /// @return True if data is sent
    bool commit() {
        if (!tx_cache.empty()) {
			if (tx_cache.size() > high_water_mark) high_water_mark = tx_cache.size();
            if (tx_cache.size() < (UART_BUFFER_SIZE)) {
                std::copy(tx_cache.begin(), tx_cache.end(), tx.data());
                tx_size = tx_cache.size();
                tx_cache.clear();
            } else {
                std::copy(tx_cache.begin(), tx_cache.begin() + UART_BUFFER_SIZE, tx.data());
                tx_size = UART_BUFFER_SIZE;
                tx_cache.erase(0, UART_BUFFER_SIZE);
            }
            for (auto port : ports) {
                HAL_UART_Transmit_IT(port, tx.data(), tx_size);
            }
            return true;
        }

        if (!pb_cache.empty()) {
            std::copy(pb_cache.begin(), pb_cache.end(), tx.data());
            for (auto port : ports) {
                HAL_UART_Transmit_IT(port, tx.data(), pb_size);
            }
            pb_cache = {};
            return true;
        }
        return false;
    }

   private:
    std::vector<UART_HandleTypeDef *> ports;

    // Sending buffer Data
    std::string tx_cache{};
    std::string pb_cache{};

    // Sending buffer size
    uint16_t tx_size{};
    uint16_t pb_size{};

	// High water mark
	uint16_t high_water_mark{};
};

#endif /* CORE_INC_SERIAL */

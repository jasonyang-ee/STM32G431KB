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

    /// @brief Assign UART interface handles for serial communication.
    /// @param Ports Vector of UART_HandleTypeDef pointers to be used for data transmission.
    void setPort(std::vector<UART_HandleTypeDef *> Ports) { ports = std::move(Ports); }

	/// @brief Retrieve the peak transmit buffer occupancy.
	/// @return Maximum bytes ever queued in the transmit cache.
	/// @note High water mark represents the largest pending data size.
	uint16_t getHighWaterMark() { return high_water_mark; }

    /// @brief Append a newline character to the transmit buffer.
    void sendLn() { tx_cache.append("\n"); };

    /// @brief Append a string to the transmit buffer.
    /// @param msg Null-terminated string to enqueue for transmission.
    void sendString(std::string msg) { tx_cache.append(msg); }

    /// @brief Append a numeric or boolean value to the transmit buffer.
    /// @tparam T Type of the value; bool is formatted as "ON"/"OFF".
    /// @param value Value to be converted and buffered.
    template <class T>
    void sendNumber(T value) {
        if (std::is_same<T, bool>::value) {
            value ? tx_cache.append("ON") : tx_cache.append("OFF");
        } else {
            tx_cache.append(std::to_string(value));
        }
    }

    /// @brief Append elements of a one-dimensional array to the transmit buffer.
    /// @tparam T Type of array elements.
    /// @tparam N Number of elements in the array.
    /// @param elements Array whose contents will be formatted and buffered; values separated by spaces.
    template <class T, size_t N>
    void sendNumber(std::array<T, N> elements) {
        for (auto &i : elements) {
            tx_cache.append(std::to_string(i));
            if (&i != &elements.back()) tx_cache.append("  ");
        }
    }

    /// @brief Append elements of a two-dimensional array to the transmit buffer.
    /// @tparam T Type of array elements.
    /// @tparam N Number of rows in the array.
    /// @tparam M Number of columns in the array.
    /// @param elements 2D array to format; rows separated by newlines and columns by spaces.
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

    /// @brief Append a labeled numeric or boolean value followed by a newline.
    /// @tparam T Type of the value; bool is formatted as "ON"/"OFF".
    /// @param msg Descriptive label to prepend.
    /// @param value Value to be converted and buffered.
    template <class T>
    void sendCombined(std::string msg, T value) {
        sendString(msg);
        sendString(":\t\t\t");
        sendNumber(value);
        sendLn();
    }

    /// @brief Append a labeled one-dimensional array followed by a newline.
    /// @tparam T Type of array elements.
    /// @tparam N Number of elements.
    /// @param msg Label to prepend.
    /// @param elements Array whose contents will be formatted and buffered.
    template <class T, size_t N>
    void sendCombined(std::string msg, std::array<T, N> elements) {
        sendString(msg);
        sendString(":\t\t\t");
        sendNumber(elements);
        sendLn();
    }

    /// @brief Append a labeled two-dimensional array followed by a newline.
    /// @tparam T Type of array elements.
    /// @tparam N Number of rows.
    /// @tparam M Number of columns.
    /// @param msg Label to prepend.
    /// @param elements 2D array whose contents will be formatted and buffered.
    template <class T, size_t N, size_t M>
    void sendCombined(std::string msg, std::array<std::array<T, M>, N> elements) {
        sendString(msg);
        sendString(":\t\t\t");
        sendNumber(elements);
        sendLn();
    }

    /// @brief Buffer a raw protobuf payload for transmission.
    /// @param msg Pointer to the protobuf byte array.
    /// @param size Number of bytes to copy into the internal cache.
    void sendPB(uint8_t *msg, uint32_t size) {
        // copy msg into std::array of pb_cache
        std::copy(msg, msg + size, pb_cache.data());
        pb_size = size;
    }

    /// @brief Transmit pending data via interrupt-driven UART.
    /// @return true if any data was sent; false if no data was pending.
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

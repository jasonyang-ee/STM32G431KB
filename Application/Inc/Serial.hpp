#ifndef CORE_INC_SERIAL
#define CORE_INC_SERIAL

#include <algorithm>
#include <array>
#include <cstdint>

#include "main.h"

class Serial {
   public:
    static constexpr size_t MAX_PORTS{1};
    Serial(){};
    virtual ~Serial(){};

    // Must be greater than protobuf payload size
    static constexpr size_t UART_BUFFER_SIZE{256};

    // Tx Rx Memory for DMA
    std::array<uint8_t, UART_BUFFER_SIZE> tx;
    std::array<uint8_t, UART_BUFFER_SIZE> rx;

    /// @brief Initialize Serial
    /// @param Ports Vector of UART_HandleTypeDef pointer
    void setPort(UART_HandleTypeDef* ports_arr[], size_t count) {
        port_count = (count <= MAX_PORTS ? count : MAX_PORTS);
        for (size_t i = 0; i < port_count; ++i) ports[i] = ports_arr[i];
    }

    /// @brief Send a new line character
    void sendLn() { 
        if (tx_cache_size + 1 < UART_BUFFER_SIZE) {
            tx_cache[tx_cache_size++] = '\n';
        }
    }

    /// @brief Buffer a string to be sent
    /// @param msg string to be sent
    void sendString(const char* msg) {
        while (*msg && tx_cache_size < UART_BUFFER_SIZE) {
            tx_cache[tx_cache_size++] = *msg++;
        }
    }

    /// @brief Sending number and bool
    /// @tparam T Data type
    /// @param value number to be buffered
    template <class T>
    void sendNumber(T value) {
        if constexpr (std::is_same_v<T, bool>) {
            sendString(value ? "ON" : "OFF");
        } else if constexpr (std::is_floating_point_v<T>) {
            sendNumber(static_cast<long>(value));
        } else {
            char buf[32];
            size_t idx = 0;
            if (value == 0) {
                buf[idx++] = '0';
            } else {
                if constexpr (std::is_signed_v<T>) {
                    if (value < 0) {
                        buf[idx++] = '-';
                        value = -value;
                    }
                }
                char rev[32]; size_t ridx = 0;
                auto v = value;
                while (v > 0) {
                    rev[ridx++] = '0' + (v % 10);
                    v /= 10;
                }
                while (ridx > 0) {
                    buf[idx++] = rev[--ridx];
                }
            }
            buf[idx] = '\0';
            sendString(buf);
        }
    }

    /// @brief Sending 1D array
    /// @tparam T Data type
    /// @tparam N 1D array size
    /// @param elements Array to be buffered
    template <class T, size_t N>
    void sendNumber(std::array<T, N> elements) {
        for (auto &i : elements) {
            sendNumber(i);
            if (&i != &elements.back()) sendString("  ");
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
                sendNumber(element);
                if (&element != &column.back()) sendString("  ");
            }
            if (&column != &elements.back()) sendLn();
        }
    }

    /// @brief Buffer a protobuf payload to be sent
    /// @param msg protobuf payload
    /// @param size size of the payload
    void sendPB(uint8_t *msg, uint32_t size) {
        // copy msg into std::array of pb_cache
        std::copy(msg, msg + size, pb_cache.data());
        pb_cache_size = size;
    }

    /// @brief Send out data with iterrupt mode
    /// @return True if data is sent
    bool commit() {
        if (tx_cache_size > 0) {
            if (tx_cache_size < UART_BUFFER_SIZE) {
                std::copy(tx_cache.begin(), tx_cache.begin() + tx_cache_size, tx.data());
            } else {
                std::copy(tx_cache.begin(), tx_cache.begin() + UART_BUFFER_SIZE, tx.data());
                tx_cache_size = UART_BUFFER_SIZE;
            }
            for (size_t i = 0; i < port_count; ++i) {
                HAL_UART_Transmit_IT(ports[i], tx.data(), tx_cache_size);
            }
            tx_cache_size = 0;
            return true;
        }

        if (pb_cache_size > 0) {
            std::copy(pb_cache.begin(), pb_cache.begin() + pb_cache_size, tx.data());
            for (size_t i = 0; i < port_count; ++i) {
                HAL_UART_Transmit_IT(ports[i], tx.data(), pb_cache_size);
            }
            pb_cache_size = 0;
            return true;
        }
        return false;
    }

   private:
    std::array<UART_HandleTypeDef*, MAX_PORTS> ports;
    size_t port_count{};

    std::array<char, UART_BUFFER_SIZE> tx_cache;
    size_t tx_cache_size{};
    std::array<uint8_t, UART_BUFFER_SIZE> pb_cache;
    size_t pb_cache_size{};

    // no dynamic buffers remain
};

#endif /* CORE_INC_SERIAL */

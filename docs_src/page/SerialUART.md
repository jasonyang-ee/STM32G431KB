# Serial UART

Serial COM is the base of all debugging and user interface for this project. It is used in interrupt mode only. DMA is not necessary because of my string buffer.

## Machnism

A STL string buffer is used to cache all pending to send data. This is gambling on that all data will be send on a fast scheduler to minimize the risk of stack overflow.

This is really convineant that one do not have to worry about data size to exceed TX buffer size.

The receiving buffer size is the only thing must be pre-determined which is easy. That is just your command max length.

In short, both RX and TX buffer size are share defined as below.

SerialCOM.hpp

```c++
#ifndef UART_BUFFER
#define UART_BUFFER 64
#endif
```

A better practice maybe using [ETL library](https://www.etlcpp.com/) where string max size can be defined. But, whatever.


## Use of UART Object

- `sendString(std::string)` and `sendNumber(any_number)`to add data to buffer.
- `sendLn()` for fast formatting.
- `sendOut()` to initiate transmission. **Recall this funtion in `HAL_UART_TxCpltCallback` to send all buffer.**
- Start of the Instance:


Before `main()`:

```c++
SerialCOM serialCOM{};
```

Inside `main()`:

```c++
MX_USART2_UART_Init();
serialCOM.setPort(&huart2);
HAL_UARTEx_ReceiveToIdle_IT(&huart2, serialCOM.m_rx_data, UART_BUFFER);
```



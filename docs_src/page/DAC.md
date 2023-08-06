# DAC



DAC using interrupt mode as of now.

Two function is required to run with HAL:
- HAL_DAC_Start(m_port, m_channel);
- HAL_DAC_SetValue(m_port, m_channel, m_alignment, count_value);

`HAL_DAC_Start()` will call `HAL_Delay()`, and if used before `vTaskStartScheduler()`, then it will mess up with FreeRTOS causing thread to stuck in loop.

Solutin is to move `HAL_DAC_Start()` into `setState()`. But, this likey will cause minor delay in runtime since every change of DAC value will call `HAL_DAC_Start()` again.

Later we should implement a DMA or RTOS init() thread to be run once on system boot.

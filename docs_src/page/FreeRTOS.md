# FreeRTOS

## C++ Wrapper

I made a c++ wrapper class to use FreeRTOS. That's the main idea here.

To expend your task:
1. Define the task handler and function
	```c++
	TaskHandle_t parse_Handle;
	void parse();
	```
2. Use lamda to create the task
	```c++
	auto t1 = [](void *arg) { static_cast<Thread *>(arg)->parse(); };
	xTaskCreate(t1, "cli parsing", 256, this, -2, &parse_Handle);
	```
3. Trap the task into its own tiny while() like the one in your main()
	```c++
	void Thread::parse() {
		while (1) {
			vTaskSuspend(NULL);
			cli.parse();
		}
	}
	```
4. Use task control (i.e. queue, notify, suspend, resume) to run your app and feature.
	```c++
	xTaskResumeFromISR(thread.parse_Handle);
	```


## Use of FreeRTOS Wrapper Class

The task are created in the object initialization using Lamda due to C++ wrapping.

- In object initializer:
	```c++
	auto t1 = [](void *arg) { static_cast<Thread *>(arg)->parse(); };
	xTaskCreate(t1, "cli parsing", 256, this, -2, &parse_Handle);
	```

Each of the corresponding task are decleared with its handle typle.

- In header:
	```c++
	TaskHandle_t parse_Handle;
	void parse();
	```


## FreeRTOS Debugging

- Install VS Code Extension: RTOS Views (`mcu-debug.rtos-views`)
- Use a hardware timer with 20KHz setting. In this projcet timer 7 is used.
- Include the follwing code to enable full debugging feature.

In `FreeRTOSConfig.h`:

```c++
#include "tim.h"					// Using 20kHz hardware timer
#define configUSE_TRACE_FACILITY 1			// Show RTOS task ID stats
#define configUSE_STATS_FORMATTING_FUNCTIONS 1		// Enable using vTaskList()
#define configRECORD_STACK_HIGH_ADDRESS 1		// Show RTOS stack stats
#define configGENERATE_RUN_TIME_STATS 1
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() MX_TIM7_Init()
#define portGET_RUN_TIME_COUNTER_VALUE() htim7.Instance->CNT
```

In `MX_TIM7_Init()`:

```c++
/* USER CODE BEGIN TIM7_Init 2 */
__TIM7_CLK_ENABLE();
HAL_TIM_Base_Init(&htim7);
HAL_TIM_Base_Start(&htim7);
```

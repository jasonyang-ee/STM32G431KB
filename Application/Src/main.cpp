
#include "main.h"

#include "Instances.hpp"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "gpio.h"
#include "tim.h"
#include "usart.h"

// Instances Objects
CLI cli{};
Thread thread{};
LED led_user{1000};
SerialCOM serialCOM{};
Flash flash{};
CustomDAC motor_dac{};
CustomADC sensor_adc{};

sml::sm<StreamState> stream_sm{&thread};
sml::sm<MainState> main_sm{&thread};

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
    HAL_Init();

    SystemClock_Config();

    MX_GPIO_Init();
    MX_DMA_Init();
    MX_TIM8_Init();
    MX_USART2_UART_Init();
    MX_TIM7_Init();
    MX_ADC2_Init();
    MX_DAC1_Init();
    MX_TIM2_Init();
    MX_TIM4_Init();

    HAL_TIM_PWM_Start_IT(&htim8, TIM_CHANNEL_2);
    HAL_UARTEx_ReceiveToIdle_IT(&huart2, serialCOM.m_rx_data, UART_BUFFER);
    HAL_ADC_Start_DMA(&hadc2, &sensor_adc.m_buffer, 1);

    // Instances Dependency Injection
    serialCOM.setPort(&huart2);
    led_user.setPort(&htim8.Instance->CCR2);
    motor_dac.setPort(&hdac1, DAC_CHANNEL_2);
    sensor_adc.setPort(&hadc2);

    // FreeRTOS Start
    vTaskStartScheduler();

    while (1) {
    }
}

/* ------------------------- Call Back Functions * ---------------------------------*/

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    vTaskNotifyGiveFromISR(thread.serial_send_handle, NULL);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    if (huart->Instance == USART2) {
        // Start the DMA again before jumping thread
        HAL_UARTEx_ReceiveToIdle_IT(&huart2, serialCOM.m_rx_data, UART_BUFFER);

        // Parse Command
        cli.setSize(Size);
        vTaskNotifyGiveFromISR(thread.parse_handle, NULL);
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) { sensor_adc.sample(); }

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM17) {
        HAL_IncTick();
    }
}

/* ------------------------- System Start Up Functions * ---------------------------*/

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
     */
    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
    RCC_OscInitStruct.PLL.PLLN = 85;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV20;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
        Error_Handler();
    }
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
    __disable_irq();
    while (1) {
    }
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
    /* User can add his own implementation to report the file name and line
       number, ex: printf("Wrong parameters value: file %s on line %d\r\n",
       file, line) */
}
#endif

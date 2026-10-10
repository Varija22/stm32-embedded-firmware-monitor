#include "main.h"
#include <stdio.h>
#include <stdint.h>

ADC_HandleTypeDef hadc1;
UART_HandleTypeDef huart2;

#define ADC_THRESHOLD       2000U
#define ADC_MAX_VALUE       4095U
#define ADC_REFERENCE_MV    3300U
#define ADC_TIMEOUT_MS      100U
#define REPORT_INTERVAL_MS  500U

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_USART2_UART_Init(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_ADC1_Init();
    MX_USART2_UART_Init();

    uint32_t adc_value = 0U;
    uint32_t voltage_mV = 0U;
    char uart_message[100];
    int message_length;
    const char *status;

    const char *startup_message =
        "\r\nSTM32 Embedded Firmware Monitor\r\n"
        "ADC threshold status monitoring started.\r\n"
        "Threshold: 2000 ADC counts\r\n\r\n";

    if (HAL_UART_Transmit(
            &huart2,
            (uint8_t *)startup_message,
            (uint16_t)sizeof(
                "\r\nSTM32 Embedded Firmware Monitor\r\n"
                "ADC threshold status monitoring started.\r\n"
                "Threshold: 2000 ADC counts\r\n\r\n") - 1U,
            HAL_MAX_DELAY) != HAL_OK)
    {
        Error_Handler();
    }

    while (1)
    {
        if (HAL_ADC_Start(&hadc1) == HAL_OK)
        {
            if (HAL_ADC_PollForConversion(
                    &hadc1, ADC_TIMEOUT_MS) == HAL_OK)
            {
                adc_value = HAL_ADC_GetValue(&hadc1);

                /* Convert ADC counts to millivolts. */
                voltage_mV =
                    (adc_value * ADC_REFERENCE_MV)
                    / ADC_MAX_VALUE;

                /*
                 * Preserve existing threshold behavior:
                 * ADC >= 2000: WARNING, LED ON
                 * ADC <  2000: NORMAL,  LED OFF
                 */
                if (adc_value >= ADC_THRESHOLD)
                {
                    status = "WARNING";

                    HAL_GPIO_WritePin(
                        GPIOA,
                        GPIO_PIN_5,
                        GPIO_PIN_SET);
                }
                else
                {
                    status = "NORMAL";

                    HAL_GPIO_WritePin(
                        GPIOA,
                        GPIO_PIN_5,
                        GPIO_PIN_RESET);
                }

                /*
                 * Report ADC value, voltage and status
                 * together in a single UART message.
                 */
                message_length = snprintf(
                    uart_message,
                    sizeof(uart_message),
                    "ADC: %lu | Voltage: %lu.%03lu V"
                    " | Status: %s\r\n",
                    (unsigned long)adc_value,
                    (unsigned long)(voltage_mV / 1000U),
                    (unsigned long)(voltage_mV % 1000U),
                    status);

                if (message_length > 0 &&
                    message_length < (int)sizeof(uart_message))
                {
                    if (HAL_UART_Transmit(
                            &huart2,
                            (uint8_t *)uart_message,
                            (uint16_t)message_length,
                            HAL_MAX_DELAY) != HAL_OK)
                    {
                        Error_Handler();
                    }
                }
                else
                {
                    const char *format_error =
                        "ERROR: UART message formatting failed\r\n";

                    (void)HAL_UART_Transmit(
                        &huart2,
                        (uint8_t *)format_error,
                        (uint16_t)(
                            sizeof(
                                "ERROR: UART message formatting failed\r\n")
                            - 1U),
                        HAL_MAX_DELAY);
                }
            }
            else
            {
                const char *error_message =
                    "ERROR: ADC conversion timeout\r\n";

                (void)HAL_UART_Transmit(
                    &huart2,
                    (uint8_t *)error_message,
                    (uint16_t)(
                        sizeof("ERROR: ADC conversion timeout\r\n")
                        - 1U),
                    HAL_MAX_DELAY);
            }

            (void)HAL_ADC_Stop(&hadc1);
        }
        else
        {
            const char *error_message =
                "ERROR: ADC could not start\r\n";

            (void)HAL_UART_Transmit(
                &huart2,
                (uint8_t *)error_message,
                (uint16_t)(
                    sizeof("ERROR: ADC could not start\r\n") - 1U),
                HAL_MAX_DELAY);
        }

        HAL_Delay(REPORT_INTERVAL_MS);
    }
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();

    __HAL_PWR_VOLTAGESCALING_CONFIG(
        PWR_REGULATOR_VOLTAGE_SCALE2);

    RCC_OscInitStruct.OscillatorType =
        RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue =
        RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_HCLK |
        RCC_CLOCKTYPE_SYSCLK |
        RCC_CLOCKTYPE_PCLK1 |
        RCC_CLOCKTYPE_PCLK2;

    RCC_ClkInitStruct.SYSCLKSource =
        RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider =
        RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider =
        RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider =
        RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(
            &RCC_ClkInitStruct,
            FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }
}

static void MX_ADC1_Init(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler =
        ADC_CLOCK_SYNC_PCLK_DIV2;
    hadc1.Init.Resolution =
        ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge =
        ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ExternalTrigConv =
        ADC_SOFTWARE_START;
    hadc1.Init.DataAlign =
        ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.EOCSelection =
        ADC_EOC_SINGLE_CONV;

    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
        Error_Handler();
    }

    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = 1;
    sConfig.SamplingTime =
        ADC_SAMPLETIME_480CYCLES;

    if (HAL_ADC_ConfigChannel(
            &hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

static void MX_USART2_UART_Init(void)
{
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength =
        UART_WORDLENGTH_8B;
    huart2.Init.StopBits =
        UART_STOPBITS_1;
    huart2.Init.Parity =
        UART_PARITY_NONE;
    huart2.Init.Mode =
        UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl =
        UART_HWCONTROL_NONE;
    huart2.Init.OverSampling =
        UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
}

static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    HAL_GPIO_WritePin(
        GPIOA,
        GPIO_PIN_5,
        GPIO_PIN_RESET);

    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode =
        GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull =
        GPIO_NOPULL;
    GPIO_InitStruct.Speed =
        GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void Error_Handler(void)
{
    __disable_irq();

    while (1)
    {
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
    (void)file;
    (void)line;
}
#endif
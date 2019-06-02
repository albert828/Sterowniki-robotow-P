#include "main.h"
#include "usart.h"
#include "gpio.h"
#include "stdbool.h"

#define BUFSIZE 3

volatile uint8_t Received[BUFSIZE], nrAxis = 0, nrSign = 1,
				 nrValue = 2, value, bsync = 0;
volatile char axis, sign;

//synchronize buffer with variables because at begining we recive data randomly
void sync()
{
	for(uint8_t i = 0; i < BUFSIZE; ++i)
	 {
		 uint8_t temp = Received[i];
		 if( ( temp == 'x' ) || ( temp == 'y' ) )
			 nrAxis = i;
		 else if (( temp == '-' ) || ( temp == '+' ))
			 nrSign = i;
		 else
			 nrValue = i;
	 }
	++bsync;
}
uint8_t yvalue = 0, xvalue = 0;
// handling callback when reciving buffer is full
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	 axis = (char)(Received[nrAxis]);
	 sign = (char)(Received[nrSign]);
	 value = (uint8_t)(Received[nrValue]);
	 if(bsync < 10)
		 sync();
	 if(axis == 'y')
		 yvalue = value;
	 if(axis == 'x')
		 xvalue = value;

	 HAL_UART_Receive_DMA(&huart2, Received, BUFSIZE);
}

void SystemClock_Config(void);

int main(void)
{

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USART2_UART_Init();

  HAL_UART_Receive_DMA(&huart2, Received, 1);

  while (1)
  {

  }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 80;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the main internal regulator output voltage 
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
}

void Error_Handler(void)
{

}

#ifdef  USE_FULL_ASSERT
void assert_failed(char *file, uint32_t line)
{

}
#endif /* USE_FULL_ASSERT */

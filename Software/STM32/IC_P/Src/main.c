#include "main.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "string.h"
#include "stdio.h"
#include "stdbool.h"
#include "pid.h"

#define BUFSIZE 3

volatile int flag1, flag2;
volatile float period1;
int pwm_dutyL, pwm_dutyP;
volatile uint8_t Received[BUFSIZE], nrAxis = 0, nrSign = 1,
				 nrValue = 2, value, bsync = 0;
volatile char axis, sign;

volatile uint16_t set_value = 230;
volatile int8_t pid_output = 0;
volatile uint8_t xvalue, yvalue, yprevious, xprevious;
volatile char xsign;

float kp = 1, ki = 1, kd = 0.0001, sampleTimeSeconds = 0;
uint16_t minOutput = 0, maxOutput = 255;

PIDControl pid;
PIDMode mode = AUTOMATIC;
PIDDirection control = DIRECT;


void PIDSetpointSet(PIDControl *pid, float setpoint) { pid->setpoint = setpoint; }
void PIDInputSet(PIDControl *pid, float input) { pid->input = input; }
float PIDOutputGet(PIDControl *pid) { return pid->output; }

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void SystemClock_Config(void);

int _write(int file, char *s, int len)
{
	HAL_UART_Transmit_DMA(&huart2, (uint8_t*)s, len);
	return len;
}

int speed;
void HAL_TIM_IC_CaptureCallback( TIM_HandleTypeDef *htim )
{
	if( htim == &htim5 )
	{
		if( htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			period1 = __HAL_TIM_GET_COMPARE(&htim5, TIM_CHANNEL_1);
			__HAL_TIM_SET_COUNTER(&htim5, 0);
			flag1 = 1;
			speed = (period1 <= 50 ) ? (255 - map(period1, 20, 50, 0, 255)) : 0;
		}
		sampleTimeSeconds = period1/1000.0;
		PIDInit(&pid, kp, ki, kd, sampleTimeSeconds, minOutput, maxOutput, mode, control);
	}
}


// handling callback when reciving buffer is full

 //int flag1 = 1;
 void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
 	 axis = (char)(Received[nrAxis]);
 	 sign = (char)(Received[nrSign]);
 	 value = (uint8_t)(Received[nrValue]);

 	 if(axis == 'x')
 	 {
 		 xsign = sign;
 		 xprevious = xvalue;
 		 xvalue = value;
 	 }

 	 if(axis == 'y')
 	 {
 		 yprevious = yvalue;
 		 yvalue = value;
 		 if(abs(value - xprevious) < 2)
 			 yvalue = yprevious;
 		 else if(value > 255)
 			 yvalue = yprevious;
 		 else if(value < 0)
 			 yvalue = yprevious;
 	 }
 	 set_value = xvalue;
 			PIDSetpointSet(&pid, set_value);
 		    PIDInputSet(&pid, speed);
 		    PIDCompute(&pid);
 		    //PIDOutputGet(&pid);
 		    pid_output = PIDOutputGet(&pid);
 	 HAL_UART_Receive_DMA(&huart2, &Received, BUFSIZE);
 }

uint8_t i = 180;

void Jazda()
 {
		  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 255 - pid.output);
 }

 void Skrecanie()
 {
	  if(axis == 'y')
	  {
		  //htim5.Instance->CCR1 = value;
		  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, yvalue);
	  }
}


int main(void)
{

  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_USART2_UART_Init();

  flag1 = flag2 = 0;
  pwm_dutyL = pwm_dutyP = 0;
 HAL_TIM_IC_Start_IT(&htim5 , TIM_CHANNEL_1);
 HAL_TIM_IC_Start_IT(&htim5 , TIM_CHANNEL_2);
 // __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_1, 75);
  //__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, pwm_dutyP);
 // __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, pwm_dutyL);
  //__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, pwm_dutyP);

  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);

  HAL_UART_Receive_DMA(&huart2, &Received, BUFSIZE);
  PIDInit(&pid, kp, ki, kd, sampleTimeSeconds, minOutput, maxOutput, mode, control);
  PIDSetpointSet(&pid, set_value);
  //PIDInputSet(&pid, speed);

  while (1)
  {
	  Jazda();
	Skrecanie();
  }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV4;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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

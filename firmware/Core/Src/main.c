/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "buzzer.h" 
#include "mpu6050.h" 
#include "math.h"   
#include "i2c.h"
#include "oled_ssd1315.h"
#include "stdio.h"  
#include "usart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ANGLE_MAX  45.0f

enum {
    FRAME_HEAD = 0,
    TEMP = 1,
    HUMIDITY = 2,
    ANGLE = 3,
    LIGHT = 4,
    KETING_LIGHT = 5,
    WOSHI_LIGHT = 6,
    KONGTIAO_STA = 7,
    ALARM_STA = 8,
    FRAME_TAIL = 9
};
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
float pitch, roll;
uint8_t report_dat[10] = {0};
static uint8_t recv_buffer[10];
static uint8_t recv_index = 0;
static uint8_t recv_state = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void uart_recv_byte(uint8_t dat)
{
    switch(recv_state)
    {
        case 0:
            if(dat == 0xff)
            {
                recv_state = 1;
                recv_index = 0;
                recv_buffer[recv_index++] = dat;
            }
            break;
        case 1:
            recv_buffer[recv_index++] = dat;
            if(recv_index >= 10)
            {
                if(recv_buffer[9] == 0xaa)
                {
                    uint8_t cmd = recv_buffer[1];
                    switch(cmd)
                    {
                        case 0x02:
                            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
                            report_dat[KETING_LIGHT] = 0;
                            break;
                        case 0x03:
                            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
                            report_dat[KETING_LIGHT] = 1;
                            break;
                        case 0x04:
                            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
                            report_dat[WOSHI_LIGHT] = 0;
                            break;
                        case 0x05:
                            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
                            report_dat[WOSHI_LIGHT] = 1;
                            break;
                        case 0x06:
                            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
                            report_dat[KONGTIAO_STA] = 0;
                            break;
                        case 0x07:
                            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
                            report_dat[KONGTIAO_STA] = 1;
                            break;
                        case 0x08:
                            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
                            report_dat[ALARM_STA] = 0;
                            break;
                        case 0x09:
                            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
                            report_dat[ALARM_STA] = 1;
                            break;
                    }
                }
                recv_state = 0;
                recv_index = 0;
            }
            break;
    }
}

void send_report_frame(void)
{
    report_dat[FRAME_HEAD] = 0xff;
    report_dat[TEMP] = 0x00;
    report_dat[HUMIDITY] = 0x00;
    report_dat[LIGHT] = 0x00;
    report_dat[FRAME_TAIL] = 0xaa;
    
    HAL_UART_Transmit(&huart1, report_dat, 10, 100);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  uint8_t mpu_init_ret;
  
  Buzzer_Init(); 
  OLED_SSD1315_Init(); 
  
  OLED_SSD1315_full(0x00);
  OLED_SSD1315_ShowString(0, 0, (uint8_t*)"Init MPU6050...", 8);
  HAL_Delay(100);
  
  mpu_init_ret = MPU6050_Init();
  
  OLED_SSD1315_full(0x00);
  if(mpu_init_ret == 0)
      OLED_SSD1315_ShowString(0, 0, (uint8_t*)"MPU6050 OK!", 8);
  else
  {
      char err_buf[20];
      sprintf(err_buf, "MPU Err: %d", mpu_init_ret);
      OLED_SSD1315_ShowString(0, 0, (uint8_t*)err_buf, 8);
  }
  HAL_Delay(500);
  
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
  
  report_dat[KETING_LIGHT] = 0;
  report_dat[WOSHI_LIGHT] = 0;
  report_dat[KONGTIAO_STA] = 0;
  report_dat[ALARM_STA] = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {  
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    char buf[30];
    float ax, ay, az;
    HAL_StatusTypeDef ret;
    uint8_t raw_buf[6];
    uint8_t recv_data;
    
    ret = HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, ACCEL_XOUT_H, 1, raw_buf, 6, 100);
    
    if(ret == HAL_OK)
    {
        int16_t raw_ax = (raw_buf[0] << 8) | raw_buf[1];
        int16_t raw_ay = (raw_buf[2] << 8) | raw_buf[3];
        int16_t raw_az = (raw_buf[4] << 8) | raw_buf[5];
        ax = raw_ax / 16384.0f;
        ay = raw_ay / 16384.0f;
        az = raw_az / 16384.0f;
    }
    else
    {
        ax = ay = az = 0.0f;
    }

    pitch = atan2(ay, sqrt(ax*ax + az*az)) * 57.2958f;
    roll  = atan2(-ax, sqrt(ay*ay + az*az)) * 57.2958f;

    if(fabs(pitch) > ANGLE_MAX || fabs(roll) > ANGLE_MAX)
    {
        Buzzer_On();
        report_dat[ANGLE] = 45;
    }
    else
    {
        Buzzer_Off();
        report_dat[ANGLE] = 0;
    }

    OLED_SSD1315_full(0x00);
    
    sprintf(buf, "P:%.1f", pitch);
    OLED_SSD1315_ShowString(0, 0, (uint8_t*)buf, 8);
    
    sprintf(buf, "R:%.1f", roll);
    OLED_SSD1315_ShowString(48, 0, (uint8_t*)buf, 8);
    
    sprintf(buf, "ANG:%d", report_dat[ANGLE]);
    OLED_SSD1315_ShowString(0, 2, (uint8_t*)buf, 8);
    
    sprintf(buf, "KL%d WL%d", report_dat[KETING_LIGHT], report_dat[WOSHI_LIGHT]);
    OLED_SSD1315_ShowString(0, 4, (uint8_t*)buf, 8);
    
    sprintf(buf, "AC%d AL%d", report_dat[KONGTIAO_STA], report_dat[ALARM_STA]);
    OLED_SSD1315_ShowString(0, 5, (uint8_t*)buf, 8);

    send_report_frame();

    if(HAL_UART_Receive(&huart1, &recv_data, 1, 10) == HAL_OK)
    {
        uart_recv_byte(recv_data);
    }

    HAL_Delay(100);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

#include <stdio.h>
#include "stm32f1xx_hal.h"
#include "tim.h"

uint8_t temp, hum;

#define DHT11_PIN_GROUP    GPIOB
#define DHT11_PIN_NUM      GPIO_PIN_8

#define DHT11_SET_LOW      HAL_GPIO_WritePin(DHT11_PIN_GROUP, DHT11_PIN_NUM, GPIO_PIN_RESET)
#define DHT11_SET_HIGH     HAL_GPIO_WritePin(DHT11_PIN_GROUP, DHT11_PIN_NUM, GPIO_PIN_SET)
#define DHT11_READ         HAL_GPIO_ReadPin(DHT11_PIN_GROUP, DHT11_PIN_NUM)

extern TIM_HandleTypeDef htim2;

inline static void DHT11_SET_OUTPUT(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = DHT11_PIN_NUM;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DHT11_PIN_GROUP, &GPIO_InitStruct);
}

inline static void DHT11_SET_INPUT(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = DHT11_PIN_NUM;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DHT11_PIN_GROUP, &GPIO_InitStruct);
}

void delay_us(uint16_t us)
{
    __HAL_TIM_SET_COUNTER(&htim2, 0);
    HAL_TIM_Base_Start(&htim2);
    uint16_t cnt = 0;
    do{
        cnt = __HAL_TIM_GET_COUNTER(&htim2);
    }while(cnt < us);
    HAL_TIM_Base_Stop(&htim2);
}

void delay_ms(uint16_t ms)
{
    for(int i = 0; i < ms; i++)
    {
        delay_us(1000);
    }
}

uint8_t DHT11_Check_Ack(void)
{
    uint16_t retry_num = 0;

    DHT11_SET_OUTPUT();
    DHT11_SET_LOW;
    delay_ms(20);

    DHT11_SET_HIGH;
    DHT11_SET_INPUT();
    delay_us(13);

    retry_num = 0;
    while((DHT11_READ == GPIO_PIN_RESET) && (retry_num < 8000))
    {
        retry_num++;
    }
    if(retry_num >= 8000)
    {
        return 1;
    }

    retry_num = 0;
    while((DHT11_READ == GPIO_PIN_SET) && (retry_num < 8000))
    {
        retry_num++;
    }
    if(retry_num >= 8000)
    {
        return 2;
    }

    return 0;
}

uint8_t DHT11_ReadBit(void)
{
    uint8_t retry_num = 0;
    uint8_t level = 0;

    while((DHT11_READ == GPIO_PIN_RESET) && (retry_num < 100))
    {
        delay_us(1);
        retry_num++;
    }

    delay_us(30);

    if(DHT11_READ == GPIO_PIN_SET)
    {
        level = 1;
    }
    else
    {
        level = 0;
    }

    while((DHT11_READ == GPIO_PIN_SET) && (retry_num < 100))
    {
        delay_us(1);
        retry_num++;
    }
    return level;
}

uint8_t DHT11_ReadByte(void)
{
    uint8_t Byte = 0;
    uint8_t i;
    for(i = 0; i < 8; i++)
    {
        Byte <<= 1;
        Byte |= DHT11_ReadBit();
    }
    return Byte;
}

void DHT11_Read_Temp_Hum(void)
{
    uint8_t ret = 0;
    uint8_t recvData[5] = {0};
    uint8_t byte_num;
    uint8_t myDataCheckResult;

    ret = DHT11_Check_Ack();

    if(ret == 0)
    {
        for(byte_num = 0; byte_num < 5; byte_num++)
        {
            recvData[byte_num] = DHT11_ReadByte();
        }

        myDataCheckResult = recvData[0] + recvData[1] + recvData[2] + recvData[3];

        if(recvData[4] == myDataCheckResult)
        {
            temp = recvData[2];
            hum = recvData[0];
        }
        else
        {
            temp = 98;
            hum = 1;
        }
    }
    else
    {
        temp = 99;
        hum = ret;
    }
}
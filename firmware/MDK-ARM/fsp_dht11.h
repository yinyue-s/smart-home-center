#ifndef __FSP_DHT11_H
#define __FSP_DHT11_H
#include "stm32f1xx_hal.h"
extern uint8_t temp, hum;
void DHT11_Start(void);
void DHT11_Read_Temp_Hum(void);
#endif
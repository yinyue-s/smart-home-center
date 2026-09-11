#ifndef __OLED_SSD1315_H__
#define __OLED_SSD1315_H__

#include "stm32f1xx_hal.h"
#include "oledfont.h"
void OLED_SSD1315_ShowString(uint8_t x,uint8_t y,uint8_t *str,uint8_t size);
void OLED_SSD1315_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size);

void OLED_SSD1315_WR_CMD(uint8_t cmd);
void OLED_SSD1315_WR_DATA(uint8_t data);
void OLED_SSD1315_Init(void);
void OLED_SSD1315_full(uint8_t dat);
void OLED_SSD1315_Char(uint8_t page, uint8_t col, uint8_t chr);

#endif /* __OLED_SSD1315_H__ */
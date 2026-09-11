#ifndef __BUZZER_H
#define __BUZZER_H

#include "main.h"

// 你的蜂鸣器引脚定义（和CubeMX一致）
#define BEEP_GPIO_PORT  GPIOB
#define BEEP_PIN        GPIO_PIN_9

// 函数声明
void Buzzer_Init(void);
void Buzzer_On(void);
void Buzzer_Off(void);
void Buzzer_Beep(uint32_t ms);

#endif
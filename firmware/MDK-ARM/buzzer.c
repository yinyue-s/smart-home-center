#include "buzzer.h"

// 初始化：默认关闭蜂鸣器
void Buzzer_Init(void)
{
    Buzzer_Off();
}

// 蜂鸣器开启（注意：你的默认电平是High，假设蜂鸣器是低电平触发）
void Buzzer_On(void)
{
    HAL_GPIO_WritePin(BEEP_GPIO_PORT, BEEP_PIN, GPIO_PIN_RESET);
}

// 蜂鸣器关闭
void Buzzer_Off(void)
{
    HAL_GPIO_WritePin(BEEP_GPIO_PORT, BEEP_PIN, GPIO_PIN_SET);
}

// 蜂鸣器响指定毫秒（非阻塞用定时器，这里是简单的阻塞式）
void Buzzer_Beep(uint32_t ms)
{
    Buzzer_On();
    HAL_Delay(ms);
    Buzzer_Off();
}
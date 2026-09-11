#include "led_ops.h"

void turingic_fn32_led_ops_fun(unsigned char led_index, unsigned char status)
{
  GPIO_TypeDef* gpio_port = GPIOB;
  uint16_t gpio_pin;

  //??????
  switch(led_index)
  {
    case 0: gpio_pin = GPIO_PIN_12; break;
    case 1: gpio_pin = GPIO_PIN_13; break;
    case 2: gpio_pin = GPIO_PIN_14; break;
    case 3: gpio_pin = GPIO_PIN_15; break;
    default: return; //????0~3????
  }

  //status=1?(???), status=0?(???)
  if(status == 1)
  {
    HAL_GPIO_WritePin(gpio_port, gpio_pin, GPIO_PIN_RESET);
  }
  else
  {
    HAL_GPIO_WritePin(gpio_port, gpio_pin, GPIO_PIN_SET);
  }
}
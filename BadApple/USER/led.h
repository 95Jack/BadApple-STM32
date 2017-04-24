#ifndef _LED_H_
#define _LED_H_

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#include "GPIOLIKE51.h"

#include "SystemCore.h"

#define LED PCout(13) //1��������0������
#define MyLEDOnceToggle() (LED = !LED)

void LED_Init(void);
void MyTimerLEDToggle(void);

#endif


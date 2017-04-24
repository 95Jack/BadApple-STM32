#ifndef _SPI_H_
#define _SPI_H_

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"

#define SPI_SPEED_2   		0
#define SPI_SPEED_4   		1
#define SPI_SPEED_8   		2
#define SPI_SPEED_16  		3
#define SPI_SPEED_32 		  4
#define SPI_SPEED_64 		  5
#define SPI_SPEED_128 		6
#define SPI_SPEED_256 		7
						  	    													  
void SPI1_Init(void);			 //��ʼ��SPI��
void SPI1_SetSpeed(unsigned char SpeedSet); //����SPI�ٶ�   
unsigned char SPI1_ReadWriteByte(unsigned char TxData);//SPI���߶�дһ���ֽ�

#endif


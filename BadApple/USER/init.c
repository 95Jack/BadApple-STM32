#include "include.h"


void All_Init(void)
{
	  delay_init(72);	    		//��ʱ������ʼ��			
		VcanLcdKeyInit();
	  LED_Init();         		//�����ϵĵƳ�ʼ��	
		TIM2_Init();       			//10ms��ʱ��
}


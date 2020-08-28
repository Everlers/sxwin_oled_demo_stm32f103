#include "led.h"

led_t led;
	
void ledInit(void)
{
	GPIO_InitTypeDef GPIOInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIOInitStruct.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;
	GPIOInitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIOInitStruct);
	
	GPIOInitStruct.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_11|GPIO_Pin_12;
	GPIOInitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIOInitStruct);
	
	for(int i=0;i<LED_NUMBER;i++)
	{
		led.sw[i] = SET;
		led.duty[i] = 0;
	}
	led.cnt = 0;
	led.state = 0;
	led.index = 0;
	led.speed = 0;
}

void ledAllClose(void)
{
	for(int i=0;i<LED_NUMBER;i++)
	{
		led.sw[i] = SET;
		led.duty[i] = 0;
	}
}

//五个灯呼吸式流水
void ledStream(void)
{
	for(int i=0;i<LED_NUMBER;i++)
	{
		led.sw[i] = SET;
		led.duty[i] = 0;
	}
	led.state |= LED_STA_STREAM_MST;
	led.index = 0;
}

void ledBrightnesSet(u8 index,u8 brightness)
{
	brightness = MAX_VALUE(brightness,1);
	brightness = MIN_VALUE(brightness,LED_BRIGHTNESS_MAX);
	led.duty[index] = brightness;
}

u8 ledBrightnesGet (u8 index)
{
	return led.duty[index];
}

void ledSwitchSet(u8 index,FlagStatus sw)
{
	led.sw[index] = sw;
}

FlagStatus ledSwitchGet(u8 index)
{
	return led.sw[index];
}

static void ledIoSet(u8 index,FlagStatus sw)
{
	switch(index)
	{
		case 0:GPIOA->ODR = sw ? GPIOA->ODR & ~GPIO_Pin_11 : GPIOA->ODR | GPIO_Pin_11;break;
		case 1:GPIOA->ODR = sw ? GPIOA->ODR & ~GPIO_Pin_8 : GPIOA->ODR | GPIO_Pin_8;break;
		case 2:GPIOA->ODR = sw ? GPIOA->ODR & ~GPIO_Pin_12 : GPIOA->ODR | GPIO_Pin_12;break;
		case 3:GPIOB->ODR = sw ? GPIOB->ODR & ~GPIO_Pin_8 : GPIOB->ODR | GPIO_Pin_8;break;
		case 4:GPIOB->ODR = sw ? GPIOB->ODR & ~GPIO_Pin_9 : GPIOB->ODR | GPIO_Pin_9;break;
	}
}

void ledTask(void)
{
	for(int i=0;i<LED_NUMBER;i++)
	{
		led.cnt++;
		if(led.sw[i])
		{
			if(led.cnt >= LED_BRIGHTNESS_MAX)
				led.cnt = 0;
			if(led.cnt < led.duty[i])
				ledIoSet(i,SET);
			else
				ledIoSet(i,RESET);
		}
		else
			ledIoSet(i,RESET);
	}
	
	if(led.state & LED_STA_STREAM_MST && led.speed == 0)
	{
		if(led.duty[led.index] < 255)
		{
			if(led.index)
			{
				//led.duty[led.index-1]--;
				if(led.index == LED_NUMBER/* && led.duty[led.index-1] == 0*/)
				{
					led.state &= ~LED_STA_STREAM_MST;
				}
			}
			if(led.index < LED_NUMBER)
				led.duty[led.index]++;
		}
		else
		{
				led.index++;
		}
		led.speed = LED_STREAM_SPEED;
	}
	else
		led.speed--;
	
	
}

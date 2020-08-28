#include "hal_key.h"

static u8 key = 0;
static u8 keyLongPress = 0;
static u16 longPressTime = 0;

static u16 pollKey = HAL_KEY_SW_ALL;
static u16 waitKey = 0;
static u8 shakeTime;

void HalKeyInit(void)
{
	GPIO_InitTypeDef GPIOInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIOInitStruct.GPIO_Pin = HAL_KEY_SW_ALL;
	GPIOInitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIOInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIOInitStruct);
}

static void shortPressPoll(u16 port)
{
	if((waitKey & HAL_KEY_SW_1) && (port & HAL_KEY_SW_1)){
		key |= HAL_KEY_1;
	}
	if((waitKey & HAL_KEY_SW_2) && (port & HAL_KEY_SW_2)){
		key |= HAL_KEY_2;
	}
	if((waitKey & HAL_KEY_SW_3) && (port & HAL_KEY_SW_3)){
		key |= HAL_KEY_3;
	}
	if((waitKey & HAL_KEY_SW_4) && (port & HAL_KEY_SW_4)){
		key |= HAL_KEY_4;
	}
	if((waitKey & HAL_KEY_SW_5) && (port & HAL_KEY_SW_5)){
		key |= HAL_KEY_5;
	}
	if((waitKey & HAL_KEY_SW_6) && (port & HAL_KEY_SW_6)){
		key |= HAL_KEY_6;
	}
	if((waitKey & HAL_KEY_SW_7) && (port & HAL_KEY_SW_7)){
		key |= HAL_KEY_ROTARY;
	}
}

static void longPressPoll(void)
{
	if(waitKey & HAL_KEY_SW_1){
		keyLongPress |= HAL_KEY_1;
	}
	else
		keyLongPress &= ~HAL_KEY_1;
	
	if(waitKey & HAL_KEY_SW_2){
		keyLongPress |= HAL_KEY_2;
	}
	else
		keyLongPress &= ~HAL_KEY_2;
	
	if(waitKey & HAL_KEY_SW_3){
		keyLongPress |= HAL_KEY_3;
	}
	else
		keyLongPress &= ~HAL_KEY_3;
	
	if(waitKey & HAL_KEY_SW_4){
		keyLongPress |= HAL_KEY_4;
	}
	else
		keyLongPress &= ~HAL_KEY_4;
	
	if(waitKey & HAL_KEY_SW_5){
		keyLongPress |= HAL_KEY_5;
	}
	else
		keyLongPress &= ~HAL_KEY_5;
	
	if(waitKey & HAL_KEY_SW_6){
		keyLongPress |= HAL_KEY_6;
	}
	else
		keyLongPress &= ~HAL_KEY_6;
	
	if(waitKey & HAL_KEY_SW_7){
		keyLongPress |= HAL_KEY_ROTARY;
	}
	else
		keyLongPress &= ~HAL_KEY_ROTARY;
}

void HalKeyTask(void)
{
	if(shakeTime == 0 && pollKey != HAL_KEY_SW_ALL)//如果有按下并且抖动时间过
	{
		if(pollKey == (HAL_KEY_PORT&HAL_KEY_SW_ALL))//按下状态确认
		{
			waitKey = ~pollKey;
			waitKey &= HAL_KEY_SW_ALL;
			pollKey = HAL_KEY_SW_ALL;
		}
		else{
			pollKey = HAL_KEY_SW_ALL;
		}
	}
	
	if((HAL_KEY_PORT & HAL_KEY_SW_ALL) !=  HAL_KEY_SW_ALL && shakeTime == 0 && waitKey == 0)//如果有按钮按下
	{
		pollKey = (HAL_KEY_PORT & HAL_KEY_SW_ALL);//得到按钮状态
		shakeTime = HAL_KEY_SHAKE_TIME;//设置祛抖时间
	}
	
	if(waitKey)//如果有按键按下
	{
		u16 port = HAL_KEY_PORT & HAL_KEY_SW_ALL;
		if(longPressTime < HAL_KEY_LONG_PRESS_TIME){//如果不到长按时间
			longPressTime++;
			shortPressPoll(port);//执行短按扫描
		}
		else if(longPressTime == HAL_KEY_LONG_PRESS_TIME)//长按时间到
		{
			longPressTime++;
			longPressPoll();//执行长按扫描
		}
		waitKey ^= (port&waitKey);//清除已经松开的按钮
	}
	else{//所有按钮已经松开
		keyLongPress = 0;
		longPressTime =  0;
	}
	
	if(shakeTime)
		shakeTime--;
}

//查询是否松开
u8 HalKeyGetLoosen(u8 sw)
{
	return (HAL_KEY_PORT & sw) ? 1:0;
}

u8 HalKeyGetShortPress(u8 sw)
{
	u8 s = (sw&key) ? 1:0;
	key ^= sw&key;
	return s;
}

u8 HalKeyGetLongPress(u8 sw)
{
	return(sw&keyLongPress) ? 1:0;
}

u8 HalKeyGetShortPressNoClear(u8 sw)
{
	return sw & key;
}

u8 HalKeyGetLongPressNoClear(u8 sw)
{
	return sw & keyLongPress;
}





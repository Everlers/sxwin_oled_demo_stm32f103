#include "hal_buzzer.h"
#include "string.h"
#include "hal_time.h"

Buzzer_t Buzzer;
u8 NumberCount = RESET;
u16 dutyCycleCount = RESET;

void HalBuzzerInit(void)
{
	GPIO_InitTypeDef GPIOInitStruct;
	TIM_TimeBaseInitTypeDef TIMInitstruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2,ENABLE);
	BUZZER_CLR;
	
	GPIOInitStruct.GPIO_Pin = GPIO_PIN;
	GPIOInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIOInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_PORT,&GPIOInitStruct);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	TIMInitstruct.TIM_Prescaler = (SystemCoreClock / 1000000) - 1;//分频
	TIMInitstruct.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	TIMInitstruct.TIM_Period = 250 - 1;//重载值
	TIMInitstruct.TIM_ClockDivision = TIM_CKD_DIV1;
	
	TIM_TimeBaseInit(TIM3,&TIMInitstruct);//初始化定时器
	TIM_Cmd(TIM3,ENABLE);//使能定时器
	
	TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = 0;
	TIM_OC3Init(TIM3,&TIM_OCInitStruct);
	TIM_CtrlPWMOutputs(TIM3,ENABLE);
	TIM_ARRPreloadConfig (TIM3, ENABLE);//使能自动重装载
	
	HalBuzzerSwitch(ENABLE);
}

void HalBuzzerSwitch(FunctionalState State)
{
	Buzzer.BuzzerSw = State;
	//HalBuzzSetPeriod(150);
}

FunctionalState HalBuzzerSwitchState(void)
{
	return Buzzer.BuzzerSw;
}

//Set buzzer open time
void HalBuzzSetPeriod(u16 Period)
{
	if(Buzzer.BuzzerSw == ENABLE)
	if(Buzzer.State == DISABLE)
	{
		Buzzer.Number = 1;
		Buzzer.Period = Period;
		Buzzer.DutyCycle = Period;
		Buzzer.State = ENABLE;
	}
}
void HalBuzzerStop(void)
{
	Buzzer.BuzzerSw = DISABLE;
	Buzzer.DutyCycle = 0;
	Buzzer.Number = 0;
	Buzzer.Period = 0;
	Buzzer.State = DISABLE;
	memset((u8 *)&Buzzer,RESET,sizeof(Buzzer_t));
	BUZZER_CLR;
}
//set buzzer open time and duty cycle (Number=0xFF Without stop)
void HalBuzzerSet(u8 Number,u16 Period,u16 dutyCucle)
{
	if(Buzzer.BuzzerSw == ENABLE)
	if(Buzzer.State == DISABLE)
	{
		Buzzer.Number = Number;
		Buzzer.Period = Period;
		Buzzer.DutyCycle = dutyCucle;
		Buzzer.State = ENABLE;
	}
}
//Get buzzer state
FunctionalState HalGetBuzzerState(void)
{
	return Buzzer.State;
}

//Buzzer task,1 milliseconds enter once
void HalBuzzerTask(void)
{
	if(Buzzer.State == ENABLE)
	{
		dutyCycleCount++;
		if(dutyCycleCount >= Buzzer.DutyCycle)
			BUZZER_CLR;
		if(dutyCycleCount < Buzzer.DutyCycle)
			BUZZER_SET;
		if(dutyCycleCount >= Buzzer.Period)
		{
			NumberCount ++;
			dutyCycleCount = RESET;
		}
		if(NumberCount >= Buzzer.Number && Buzzer.Number != 0xFF)
		{
			NumberCount = RESET;
			BUZZER_CLR;
			Buzzer.State = DISABLE;
		}
	}
}


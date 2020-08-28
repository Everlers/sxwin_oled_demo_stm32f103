#include "hal_exti.h"

HalExti_t HalExti[HAL_EXTI_MAX_CB_NUM];

static void gpioConfig(HalGpioTypeDef GPIOx,u8 PINx,GPIOMode_TypeDef IOMode);

//外部中断初始化
//参数GPIOx:根据HalGpioTypeDef枚举定义 
//参数PINx:0~15
//参数IOMode:参考EXTITrigger_TypeDef类定义 
//参数Trigger:参考EXTITrigger_TypeDef (选择条边沿)
//参数CB:中断回调函数
void HalExtiInit(HalGpioTypeDef GPIOx,u8 PINx,GPIOMode_TypeDef IOMode,EXTITrigger_TypeDef Trigger,HalExtiCallBack_t CB)
{
	u8 i;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	gpioConfig(GPIOx,PINx,IOMode);
	
	GPIO_EXTILineConfig(GPIOx,PINx);
	EXTI_InitStructure.EXTI_Line = BV(PINx);
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = Trigger;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	if(PINx <= 4)
		NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn+PINx;
	else if(PINx <= 9)
		NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	else 
		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;	//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	for(i=0;i<HAL_EXTI_MAX_CB_NUM;i++){//寻找空缓冲并注册
		if(HalExti[i].cb == 0){
			HalExti[i].line = PINx;
			HalExti[i].cb = CB;
			break;
		}
	}
}

static void gpioConfig(HalGpioTypeDef GPIOx,u8 PINx,GPIOMode_TypeDef IOMode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|(RCC_APB2Periph_GPIOA<<GPIOx),ENABLE);
	GPIO_InitStructure.GPIO_Pin = BV(PINx);
	GPIO_InitStructure.GPIO_Mode = IOMode;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init((GPIO_TypeDef *)(GPIOA_BASE+(GPIOx*0x400)),&GPIO_InitStructure);
}

static void HalExtiInterrupt(u8 line)
{
	for(u8 i=0;i<HAL_EXTI_MAX_CB_NUM;i++){//寻找需要回调的函数
		if(HalExti[i].cb){//如果有注册回调
			if(HalExti[i].line == line)//如果是此IO的中断回调
				HalExti[i].cb();//调用
		}
	}
}

//外部中断0服务程序 
void EXTI0_IRQHandler(void)
{
	if(EXTI_GetFlagStatus(EXTI_Line0))
	{
		HalExtiInterrupt(0);
	}
	EXTI_ClearITPendingBit(EXTI_Line0); //清除LINE0上的中断标志位  
}

//外部中断1服务程序 
void EXTI1_IRQHandler(void)
{
	if(EXTI_GetFlagStatus(EXTI_Line1))
	{
		HalExtiInterrupt(1);
	}
	EXTI_ClearITPendingBit(EXTI_Line1); //清除LINE1上的中断标志位  
}

//外部中断2服务程序 
void EXTI2_IRQHandler(void)
{
	if(EXTI_GetFlagStatus(EXTI_Line2))
	{
		HalExtiInterrupt(2);
	}
	EXTI_ClearITPendingBit(EXTI_Line2); //清除LINE3上的中断标志位  
}

//外部中断3服务程序 
void EXTI3_IRQHandler(void)
{
	if(EXTI_GetFlagStatus(EXTI_Line3))
	{
		HalExtiInterrupt(3);
	}
	EXTI_ClearITPendingBit(EXTI_Line3); //清除LINE3上的中断标志位  
}

//外部中断4服务程序 
void EXTI4_IRQHandler(void)
{
	if(EXTI_GetFlagStatus(EXTI_Line4))
	{
		HalExtiInterrupt(4);
	}
	EXTI_ClearITPendingBit(EXTI_Line4); //清除LINE4上的中断标志位  
}

//外部中断5-9服务程序 
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetFlagStatus(EXTI_Line5))
	{
		HalExtiInterrupt(5);
		EXTI_ClearITPendingBit(EXTI_Line5); //清除
	}
	if(EXTI_GetFlagStatus(EXTI_Line6))
	{
		HalExtiInterrupt(6);
		EXTI_ClearITPendingBit(EXTI_Line6); //清除
	}
	if(EXTI_GetFlagStatus(EXTI_Line7))
	{
		HalExtiInterrupt(7);
		EXTI_ClearITPendingBit(EXTI_Line7); //清除
	}
	if(EXTI_GetFlagStatus(EXTI_Line8))
	{
		HalExtiInterrupt(8);
		EXTI_ClearITPendingBit(EXTI_Line8); //清除
	}
	if(EXTI_GetFlagStatus(EXTI_Line9))
	{
		HalExtiInterrupt(9);
		EXTI_ClearITPendingBit(EXTI_Line9); //清除
	}
}

//外部中断10-15服务程序 
void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetFlagStatus(EXTI_Line10))
	{
		HalExtiInterrupt(10);
		EXTI_ClearITPendingBit(EXTI_Line10); //清除
	}
	if(EXTI_GetFlagStatus(EXTI_Line11))
	{
		HalExtiInterrupt(11);
		EXTI_ClearITPendingBit(EXTI_Line11); //清除
	}
	if(EXTI_GetFlagStatus(EXTI_Line12))
	{
		HalExtiInterrupt(12);
		EXTI_ClearITPendingBit(EXTI_Line12); //清除
	}
	if(EXTI_GetFlagStatus(EXTI_Line13))
	{
		HalExtiInterrupt(13);
		EXTI_ClearITPendingBit(EXTI_Line13); //清除
	}
	if(EXTI_GetFlagStatus(EXTI_Line14))
	{
		HalExtiInterrupt(14);
		EXTI_ClearITPendingBit(EXTI_Line14); //清除
	}
	if(EXTI_GetFlagStatus(EXTI_Line15))
	{
		HalExtiInterrupt(15);
		EXTI_ClearITPendingBit(EXTI_Line15); //清除
	}
}

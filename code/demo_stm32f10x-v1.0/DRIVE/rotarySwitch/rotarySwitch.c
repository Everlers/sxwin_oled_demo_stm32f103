#include "rotarySwitch.h"
#include "hal_exti.h"
#include "hal_time.h"
#include "delay.h"

//旋钮开关

static rotarySwSta downRss = ROTARYSW_NOT;
static rotarySwSta rss = ROTARYSW_NOT;

static void rotaryAIntrrupt (void);
static void timeCallBack(void);

void rotarySwInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	HalExtiInit(HAL_EXTI_GPIOB,2,GPIO_Mode_IPU,EXTI_Trigger_Rising_Falling,rotaryAIntrrupt);
	HalTimeRegisteringCallbacks(timeCallBack,1);
	rss = ROTARYSW_NOT;
}

FlagStatus rotarySwGet(rotarySwSta r)
{
	if(rss == r)
	{
		rss = ROTARYSW_NOT;
		return SET;
	}
	else return RESET;
}

//A相
static void rotaryAIntrrupt (void)
{
	delay_us(100);
	if(!ROTARY_SW_IO_A)//下降沿
	{
		if(ROTARY_SW_IO_B)//如果是顺时针
		{
			downRss = ROTARYSW_CW;
		}
		else//逆时针
		{
			downRss = ROTARYSW_CCW;
		}
	}
	else//上升沿
	{
		if(!ROTARY_SW_IO_B && downRss == ROTARYSW_CW)//如果是顺时针
		{
			rss = ROTARYSW_CW;
			downRss = ROTARYSW_NOT;
		}
		else if(ROTARY_SW_IO_B && downRss == ROTARYSW_CCW)//如果是逆时针
		{
			rss = ROTARYSW_CCW;
			downRss = ROTARYSW_NOT;
		}
		else
		{
			downRss = ROTARYSW_NOT;
		}
	}
}

static void timeCallBack(void)
{
	
}

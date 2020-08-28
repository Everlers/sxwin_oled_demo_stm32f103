#include "hal_time.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"

//通用定时器中断

HalTime_t HalTime[HAL_TIME_MAX_CB_NUM];//定时器回调

//定时器初始化
//Prescaler:时钟分频
//Period:自动重装值
void HalTimeInit(u16 Prescaler, u16 Period)
{
	TIM_TimeBaseInitTypeDef TIMInitstruct;
	NVIC_InitTypeDef NVICInitStruct;
	
	for(int i=0;i<HAL_TIME_MAX_CB_NUM;i++){
		HalTime[i].cb = NULL;
		HalTime[i].count = 0;
		HalTime[i].mode = HAL_TIME_MODE_ONCE;
		HalTime[i].prescaler = 0;
	}
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	TIMInitstruct.TIM_Prescaler = Prescaler;//分频
	TIMInitstruct.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	TIMInitstruct.TIM_Period = Period;//重载值
	TIMInitstruct.TIM_ClockDivision = TIM_CKD_DIV1;
	
	TIM_TimeBaseInit(TIM2,&TIMInitstruct);//初始化定时器
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);//使能定时器更新中断
	TIM_Cmd(TIM2,ENABLE);//使能定时器
	
	NVICInitStruct.NVIC_IRQChannel = TIM2_IRQn;
	NVICInitStruct.NVIC_IRQChannelPreemptionPriority = 0x02;//抢占优先级
	NVICInitStruct.NVIC_IRQChannelSubPriority = 0x02;//中断优先级
	NVICInitStruct.NVIC_IRQChannelCmd = ENABLE;//使能中断
	NVIC_Init(&NVICInitStruct);//初始化中断向量表
}

//定时调用
//参数CallBack:回调函数
//参数prescaler:预分频
//返回值: 1:没有剩下的缓冲 0:注册成功
u8 HalTimeExStart(HalTimeCallBack_t CallBack,u16 prescaler)
{
	u8 i;
	for(i=0;i<HAL_TIME_MAX_CB_NUM;i++)//检查空的位置
	{
		if(HalTime[i].cb == RESET){//如果空位
			HalTime[i].mode = HAL_TIME_MODE_ONCE;//设置调用一次
			HalTime[i].prescaler = prescaler;//设置分频
			HalTime[i].count = RESET;//清除计数
			HalTime[i].cb = CallBack;//写入回调
			break;
		}
	}
	if(i == HAL_TIME_MAX_CB_NUM)
		return 1;
	else
		return 0;
}

//注册回调
//参数CallBack:回调函数
//参数prescaler:预分频
//返回值: 1:没有可用回调空间 2:已经注册过  0:设置成功
u8 HalTimeRegisteringCallbacks (HalTimeCallBack_t CallBack,u16 prescaler)
{
	u8 i;
	for(i=0;i<HAL_TIME_MAX_CB_NUM;i++)//检查有没注册过
		if(HalTime[i].cb == CallBack)
			break;
	if(i != HAL_TIME_MAX_CB_NUM)return 2;
	for(i=0;i<HAL_TIME_MAX_CB_NUM;i++)//检查空的位置
	{
		if(HalTime[i].cb == RESET){//如果空位
			HalTime[i].mode = HAL_TIME_MODE_RELOAD;//设置重装模式
			HalTime[i].prescaler = prescaler;//设置分频
			HalTime[i].count = RESET;//清除计数
			HalTime[i].cb = CallBack;//写入回调
			break;
		}
	}
	if(i == HAL_TIME_MAX_CB_NUM)
		return 1;
	else
		return 0;
}
//取消回调
//返回值： 1:错误,没有找到该回调 0:移除完成
u8 HalTimeCancelCallback (HalTimeCallBack_t CallBack)
{
	u8 i;
	for(i=0;i<HAL_TIME_MAX_CB_NUM;i++)
	{
		if(HalTime[i].cb == CallBack){
			HalTime[i].cb = NULL;
			break;
		}
	}
	if(i == HAL_TIME_MAX_CB_NUM)
		return 1;
	else
		return 0;
}
//定时器中断服务函数
void TIM2_IRQHandler(void)
{
	u8 i;
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //溢出中断
	{
		for(i=0;i<HAL_TIME_MAX_CB_NUM;i++)
		{
			if(HalTime[i].cb){
				if(HalTime[i].count >= HalTime[i].prescaler){
					HalTime[i].cb();//回调
					if(HalTime[i].mode == HAL_TIME_MODE_ONCE)//如果只需要调用一次
						HalTime[i].cb = NULL;//清除本回调
					else
						HalTime[i].count = RESET;//清除计数器
				}
				else{
					HalTime[i].count++;//递加计数器
				}
			}
		}
	}
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //清除中断标志位
}



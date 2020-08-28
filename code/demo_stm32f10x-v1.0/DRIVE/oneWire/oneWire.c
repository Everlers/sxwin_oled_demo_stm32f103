#include "oneWire.h"
#include "FreeRtos.h"
#include "Task.h"
#include "hal_exti.h"
#include "string.h"

static ow_t ow;

static void EXTI_IRQHandler (void);

void owInit(void)
{
	TIM_TimeBaseInitTypeDef TIMInitstruct;
	NVIC_InitTypeDef NVICInitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;
	
	ow.recvState = OW_STA_IDLE;
	ow.sendState = OW_STA_IDLE;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource1);
	GPIO_InitStruct.GPIO_Pin = OW_DATA_IO_BIT;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	//用于单总线的定时器4
	TIMInitstruct.TIM_Prescaler = SystemCoreClock/1000000-1;//分频
	TIMInitstruct.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	TIMInitstruct.TIM_Period = 65535;//重载值
	TIMInitstruct.TIM_ClockDivision = TIM_CKD_DIV1;
	
	TIM_TimeBaseInit(TIM4,&TIMInitstruct);//初始化定时器
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);//使能定时器更新中断
	TIM_ARRPreloadConfig(TIM4,DISABLE);//关闭预装载功能
	TIM_Cmd(TIM4,DISABLE);//使能定时器
	
	NVICInitStruct.NVIC_IRQChannel = TIM4_IRQn;
	NVICInitStruct.NVIC_IRQChannelPreemptionPriority = configMAX_SYSCALL_INTERRUPT_PRIORITY-1;//不受RTOS屏蔽的抢占优先级
	NVICInitStruct.NVIC_IRQChannelSubPriority = 0x00;//中断优先级
	NVICInitStruct.NVIC_IRQChannelCmd = ENABLE;//使能中断
	NVIC_Init(&NVICInitStruct);//初始化中断向量表
	
	HalExtiInit(HAL_EXTI_GPIOB,1,GPIO_Mode_IPU,EXTI_Trigger_Rising_Falling,EXTI_IRQHandler);
}

//OW发送 数据低位开始发
//dat:发送的数据
//lenght:数据的长度
void owSend(u8 *dat,u8 lenght)
{
	if(ow.sendState == OW_STA_IDLE && ow.recvState == OW_STA_IDLE)//如果空闲
	{
		OW_DATA_IO_OUT;
		ow.sendBitIndex = 0;
		ow.sendByteIndex = 0;
		ow.sendLenght = lenght;
		ow.sendCntIndex = 0;
		memcpy(ow.sendData,dat,ow.sendLenght);
		ow.sendState = OW_STA_START;
		//OW_DATA_SEND_IO = 0;
		OW_TIM_COUNT_SET(100);//立刻开始发送数据
		OW_TIM_START();//开始计数
	}
}

u8 owRecvDataGet(u8 *data,u8 maxLen)
{
	if(ow.recvState == OW_STA_DONE)
	{
		for(int i=0;i<MIN_VALUE(maxLen,ow.recvLenght);i++)
			data[i] = ow.recvData[i];
		ow.recvState = OW_STA_IDLE;
	}
	else
		return 0;
	return MIN_VALUE(maxLen,ow.recvLenght);
}

owState_t owSendStateGet(void)
{
	return ow.sendState;
}

owState_t owRecvStateGet(void)
{
	return ow.recvState;
}

//oneWire数据格式配置
void owConfig(dataForm_t *form)
{
	ow.form.impulse.start.tpLevel = form->impulse.start.tpLevel;
	ow.form.impulse.start.level1Time = form->impulse.start.level1Time;
	ow.form.impulse.start.level2Time = form->impulse.start.level2Time;
	
	ow.form.impulse.data0.tpLevel = form->impulse.data0.tpLevel;
	ow.form.impulse.data0.level1Time = form->impulse.data0.level1Time;
	ow.form.impulse.data0.level2Time = form->impulse.data0.level2Time;
	
	ow.form.impulse.data1.tpLevel = form->impulse.data1.tpLevel;
	ow.form.impulse.data1.level1Time = form->impulse.data1.level1Time;
	ow.form.impulse.data1.level2Time = form->impulse.data1.level2Time;

	ow.form.impulse.end.tpLevel = form->impulse.end.tpLevel;
	ow.form.impulse.end.level1Time = form->impulse.end.level1Time;
	ow.form.impulse.end.level2Time = form->impulse.end.level2Time;

	ow.form.impulse.series.tpLevel = form->impulse.series.tpLevel;
	ow.form.impulse.series.level1Time = form->impulse.series.level1Time;
	ow.form.impulse.series.level2Time = form->impulse.series.level2Time;
	
	ow.form.sequence = form->sequence;
	ow.form.waitTime = form->waitTime;
	ow.form.numBer = form->numBer;
	//memcpy(&ow.form,form,sizeof(dataForm_t));
}

void TIM4_IRQHandler (void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
		OW_TIM_STOP();//停止计数
		if(ow.recvState == OW_STA_IDLE && ow.sendState != OW_STA_IDLE)
		{
			static u8 level = 0;
			u8 bitSta;
			
			START://开始
			switch((u8)ow.sendState)
			{
				case OW_STA_START:
					if((level && ow.form.impulse.start.level2Time) || (!level && ow.form.impulse.start.level1Time))//如果需要开始信号
						OW_DATA_SEND_IO = level ? 	(ow.form.impulse.start.tpLevel ? 0:1) : (ow.form.impulse.start.tpLevel ? 1:0);//设置电平
					OW_TIM_COUNT_SET(MAX_VALUE(100,level ? ow.form.impulse.start.level2Time : ow.form.impulse.start.level1Time));	//设置电平时间
					if(level)//已经开始第二个电平
						ow.sendState = OW_STA_DATA;
					if(ow.form.impulse.start.level2Time == 0)//开始信号没有第二个电平
					{
						level = 1;
						ow.sendState = OW_STA_DATA;
					}
				break;

				case OW_STA_DATA:
					bitSta = (ow.form.sequence == OW_DATA_SEQUENCE_LOW) ? (ow.sendData[ow.sendByteIndex] & BV(ow.sendBitIndex) ? 1:0) : (ow.sendData[ow.sendByteIndex] & BV(7 - ow.sendBitIndex) ? 1:0);//读取数据位
					OW_DATA_SEND_IO =  level ?  ((bitSta ? ow.form.impulse.data1.tpLevel : ow.form.impulse.data0.tpLevel) ? 0:1) : //设置数据第二个电平
																 ((bitSta ? ow.form.impulse.data1.tpLevel : ow.form.impulse.data0.tpLevel) ? 1:0) ;	//设置数据第一个电平
					OW_TIM_COUNT_SET(level ? (bitSta ? ow.form.impulse.data1.level2Time : ow.form.impulse.data0.level2Time) : (bitSta ? ow.form.impulse.data1.level1Time : ow.form.impulse.data0.level1Time));//设置电平时间
					if(level)//一个位发送完成
					{
						ow.sendBitIndex++;
						if(ow.sendBitIndex >= 8)
						{
							ow.sendBitIndex = 0;
							ow.sendByteIndex++;
							if(/*ow.sendByteIndex < ow.sendLenght && */(ow.form.impulse.series.level1Time || ow.form.impulse.series.level2Time))//如果有连续数据的时序
							{
								ow.sendState = OW_STA_SERIES;
								break;
							}
							CheckEnd:
							if(ow.sendByteIndex >= ow.sendLenght)//如果发送完成
							{
								if(ow.form.impulse.end.level1Time)//如果需要发送结束信号
									ow.sendState = OW_STA_END;//进入结束信号的发送
								else//无需进入结束信号
								{
									if(ow.form.waitTime)//如需要等待
										ow.sendState = OW_STA_WAIT;//进入等待状态
									else
										ow.sendState = OW_STA_DONE;//进入完成状态
								}
							}
						}
					}
				break;

				case OW_STA_SERIES:
					OW_TIM_COUNT_SET(MAX_VALUE(100,level ? ow.form.impulse.series.level2Time : ow.form.impulse.series.level1Time));
					OW_DATA_SEND_IO = level ? (ow.form.impulse.series.tpLevel ? 0:1) : (ow.form.impulse.series.tpLevel ? 1:0);
					if(level)//如果第二个电平完成
					{
						ow.sendState = OW_STA_DATA;
						goto CheckEnd;
					}
					if(ow.form.impulse.series.level2Time == 0)//如果没有第二个电平
					{
						level = 1;
						ow.sendState = OW_STA_DATA;
					}
				break;
					
				case OW_STA_END:
				OW_TIM_COUNT_SET(MAX_VALUE(100,level ? ow.form.impulse.end.level2Time : ow.form.impulse.end.level1Time));//设置电平时间
				OW_DATA_SEND_IO =  level ? (ow.form.impulse.end.tpLevel ? 1:0) :  	//设置第二个电平
															(ow.form.impulse.end.tpLevel ? 0:1) ;		//设置第一个电平
				if(level)//结束信号完成
				{
					if(ow.form.waitTime)//如需要等待
						ow.sendState = OW_STA_WAIT;//进入等待状态
					else
						ow.sendState = OW_STA_DONE;//进入完成状态
				}
				break;

				case OW_STA_WAIT:
						OW_DATA_SEND_IO = 1;//关闭IO
						OW_TIM_COUNT_SET(ow.form.waitTime);//设置等待时间
						ow.sendState = OW_STA_DONE;//进入完成状态
				break;

				case OW_STA_DONE:
					OW_DATA_SEND_IO = 1;//关闭IO
					ow.sendCntIndex++;//计数发送完成一个
					if(ow.sendCntIndex < ow.form.numBer)//需要继续发送
					{
						level = 0;
						ow.sendBitIndex = 0;
						ow.sendByteIndex = 0;
						ow.sendState = OW_STA_START;
						goto START;//继续发送数据
					}
					else//无需发送
					{
						OW_DATA_SEND_IO = 1;
						OW_DATA_IO_IN;
						level = 1;
						ow.sendState = OW_STA_IDLE;
					}
				break;
			}
			level = !level;//下一个电平
			//OW_LED_IO = !OW_DATA_IO;
		}
		else//接收超时/完成一帧数据
		{
			if(ow.sendBitIndex == 0 && ow.recvState == OW_STA_DATA && ow.recvByteIndex >= 1)//是否接收完整 (完成数据接收)
			{
				OW_TIM_PERIOD_SET(0xFFFF);
				ow.recvLenght = ow.recvByteIndex;//获取接收长度
				ow.recvState = OW_STA_DONE;//进入完成状态
			}
			else
				ow.recvState = OW_STA_IDLE;
		}
		if(ow.sendState != OW_STA_IDLE)
				OW_TIM_START();//开始计数
	}
}

static void EXTI_IRQHandler (void)
{
	u8 level;
	u16 count;
	if(ow.sendState != OW_STA_IDLE || ow.recvState == OW_STA_DONE)return;
	OW_TIM_STOP();//停止计数
	count = OW_TIM_COUNT_GET();//读取计数值
	OW_TIM_COUNR_CLR();//清除计数器
	OW_TIM_PERIOD_SET(0xFFFF);
	OW_TIM_START();//开始计数
	level = OW_DATA_RECV_IO ? 0:1;//读取上次电平
	switch((u8)ow.recvState)
	{
		case OW_STA_IDLE:
			if(ow.form.impulse.start.level2Time || ow.form.impulse.start.level1Time)//如果有开始信号
				ow.recvState = OW_STA_START;
			else
				ow.recvState = OW_STA_DATA;
			ow.recvBitIndex = 0;
			ow.recvByteIndex = 0;
			ow.recvCntIndex = 0;
		break;
			
		case OW_STA_START:
			if(OW_TIM_CHECK(count,((level == ow.form.impulse.start.tpLevel) ? ow.form.impulse.start.level1Time : ow.form.impulse.start.level2Time)))//检查开始信号的时序
			{
				if((level != ow.form.impulse.start.tpLevel) || ow.form.impulse.start.level2Time == 0)//如果到第二个电平 或者 没有第二个电平 (开始信号完成)
				{
					ow.recvState = OW_STA_DATA;
				}
			}
			else if(OW_TIM_CHECK(count,10))//如果有个10us左右的信号被拉高则是模块问题 (忽略此沿边)
			{
				
			}
			else//如果错误
			{
				ow.recvState = OW_STA_IDLE;
				OW_TIM_STOP();//停止计数
				return;
			}
		break;
			
		case OW_STA_DATA:
			if(ow.recvByteIndex < OW_MAX_DATA_LEN)
			{
				if(OW_TIM_CHECK(count,((level == ow.form.impulse.data0.tpLevel) ? ow.form.impulse.data0.level1Time : ow.form.impulse.data0.level2Time)))//检查数据0的电平时序
				{
					if(level != ow.form.impulse.data0.tpLevel)//如果到第二个电平(完成一个位的接收)
					{
						ow.recvData[ow.recvByteIndex] = (ow.form.sequence == OW_DATA_SEQUENCE_HIGH) ? ow.recvData[ow.recvByteIndex]<<1 : ow.recvData[ow.recvByteIndex]>>1;
						ow.recvData[ow.recvByteIndex] &= (ow.form.sequence == OW_DATA_SEQUENCE_HIGH) ? ~0x01 : ~0x80;
						ow.recvBitIndex ++;
					}
				}
				else if(OW_TIM_CHECK(count,((level == ow.form.impulse.data1.tpLevel) ? ow.form.impulse.data1.level1Time : ow.form.impulse.data1.level2Time)))//检查数据1的电平时序
				{
					if(level != ow.form.impulse.data1.tpLevel)//如果到第二个电平(完成一个位的接收)
					{
						ow.recvData[ow.recvByteIndex] = (ow.form.sequence == OW_DATA_SEQUENCE_HIGH) ? ow.recvData[ow.recvByteIndex]<<1 : ow.recvData[ow.recvByteIndex]>>1;
						ow.recvData[ow.recvByteIndex] |= (ow.form.sequence == OW_DATA_SEQUENCE_HIGH) ? 0x01 : 0x80;
						ow.recvBitIndex ++;
					}
				}
				else if((ow.form.impulse.series.level1Time || ow.form.impulse.series.level2Time) && //如果有连续字节传输的时序
					OW_TIM_CHECK(count,((level == ow.form.impulse.series.tpLevel) ? ow.form.impulse.series.level1Time : ow.form.impulse.series.level2Time)))//如果时序正确
				{
					if(level != ow.form.impulse.series.tpLevel)//如果到第二个电平(开始下一个字节的接收)
					{
						
					}
				}
				else if((ow.form.impulse.end.level2Time || ow.form.impulse.end.level1Time) &&//如果有结束信号
					OW_TIM_CHECK(count,((level == ow.form.impulse.end.tpLevel) ? ow.form.impulse.end.level1Time : ow.form.impulse.end.level2Time)))//检查结束信号时序
				{
					if(level != ow.form.impulse.end.tpLevel || ow.form.impulse.end.level2Time == 0)//如果到第二个电平 或者 没有第二个电平 (结束信号完成)
					{
						if(ow.recvByteIndex == 0)//是否接收完整 (完成数据接收)
						{
							ow.recvLenght = ow.recvByteIndex;//获取接收长度
							ow.recvState = OW_STA_DONE;//进入完成状态
						}
					}
				}
				else if(ow.form.impulse.start.level1Time && OW_TIM_CHECK(count,((level == ow.form.impulse.start.tpLevel) ? ow.form.impulse.start.level1Time : ow.form.impulse.start.level2Time)))//(重新判断是否为开始信号)
				{
					if((level != ow.form.impulse.start.tpLevel) || ow.form.impulse.start.level2Time == 0)//如果到第二个电平 或者 没有第二个电平 (开始信号完成)
					{
						//ow.recvState = OW_STA_DATA;
					}
				}
				else//错误
				{
					ow.recvState = OW_STA_IDLE;
					OW_TIM_STOP();//停止计数
					return;
				}
				if(ow.recvBitIndex >= 8)//接收完一个字节
				{
					ow.recvBitIndex = 0;
					ow.recvByteIndex++;
					if(ow.recvByteIndex >= OW_MAX_DATA_LEN)
						ow.recvByteIndex--;
					if(ow.form.impulse.end.level2Time == 0 && ow.form.impulse.end.level1Time == 0)//如果没有结束信号的时序
					{
						OW_TIM_STOP();//停止计数
						//OW_TIM_COUNT_SET(0);//清除计数器
						OW_TIM_PERIOD_SET(OW_ONT_PACK_TIME);//设定一包判定的时间
						OW_TIM_START();//开始计数
					}
				}
			}
		break;
	}
}

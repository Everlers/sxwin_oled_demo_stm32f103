#ifndef _ONEWIRE_H_
#define _ONEWIRE_H_
#include "hal_sys.h"

#define OW_DATA_SEND_IO									PBout(1)
#define OW_DATA_RECV_IO									PBin(1)
#define OW_DATA_IO_PORT									GPIOB
#define OW_DATA_IO_BIT									GPIO_Pin_1
#define OW_DATA_IO_BIT_NUM							1
#define OW_DATA_IO_IN										OW_DATA_IO_PORT->CRL &= ~((u32)0x0f << (OW_DATA_IO_BIT_NUM *4));OW_DATA_IO_PORT->CRL |= ((u32)8 << (OW_DATA_IO_BIT_NUM *4))//上拉/下拉输入
#define OW_DATA_IO_OUT									OW_DATA_IO_PORT->CRL &= ~((u32)0x0f << (OW_DATA_IO_BIT_NUM *4));OW_DATA_IO_PORT->CRL |= ((u32)3 << (OW_DATA_IO_BIT_NUM *4))//推挽输出模式
#define OW_LED_IO												PBout(1)

#define OW_TIM_PERIOD_SET(x)						TIM4->ARR																//定时器周期定时器
#define OW_TIM_COUNT_GET()							TIM4->CNT																//定时器计数器查询
#define OW_TIM_COUNT_SET(x)							TIM4->CNT = 0xFFFF - (x)								//定时器计数器设定
#define OW_TIM_COUNR_CLR()							TIM4->CNT = 0														//清除计数器
#define OW_TIM_START()									TIM4->CR1 |= TIM_CR1_CEN								//开启定时器
#define OW_TIM_STOP()										TIM4->CR1 &= ~TIM_CR1_CEN								//停止定时器

#define OW_ONT_PACK_TIME								12000																		//一包数据的判断时间
#define OW_MAX_DATA_LEN									12																			//最大数据长度
#define OW_SCOMPUTE_OFFSET							-20																			//发送时序计算偏差补偿
#define OW_RCOMPUTE_OFFSET							40																			//接受时序计算偏差补偿(接收允许偏差 单位：%)

#define OW_TIM_CHECK(c,x)								(((((c) > (x)) ? c-x:x-c) <= (x * OW_RCOMPUTE_OFFSET / 100)) ? 1:0)

typedef enum{
	OW_DATA_SEQUENCE_HIGH 	= 0,//高位到低位
	OW_DATA_SEQUENCE_LOW 		= 1,//低位到高位
}dataSequence_t;//数据顺序

typedef struct{
	u8	tpLevel;			//优先电平 1/0
	u16 level1Time;		//第一个电平时间
	u16 level2Time;		//第二个电平时间
}impulse_t;//脉冲形式

typedef struct{
	impulse_t start;		//开始时序(同步头)
	impulse_t data1;		//数据1时序
	impulse_t data0;		//数据0时序
	impulse_t	series;		//连续字节传输时序
	impulse_t end;			//结束时序
}dataImpulse_t;				//数据时序

typedef struct{
	dataImpulse_t		impulse;							//数据时序
	dataSequence_t	sequence;							//数据顺序
	u16							waitTime;							//等待时间(数据发送完成后的等待时间)
	u8							numBer;								//发送次数(一帧数据发送次数)
}dataForm_t;//数据格式

typedef enum{
	OW_STA_IDLE 		= 0,
	OW_STA_START 		= 1,
	OW_STA_DATA			= 2,
	OW_STA_SERIES		= 3,
	OW_STA_END			= 4,
	OW_STA_WAIT			= 5,
	OW_STA_DONE			= 6,
}owState_t;

typedef struct{
	owState_t 		sendState;									//当前发送状态
	u8						sendByteIndex;							//当前发送的字节
	u8						sendBitIndex;								//当前发送的位
	u8						sendCntIndex;								//当前发送的帧数
	u8						sendData[OW_MAX_DATA_LEN];	//需要发送的数据
	u8						sendLenght;									//需要发送的字节长度
	
	owState_t 		recvState;									//当前发送状态
	u8						recvByteIndex;							//当前发送的字节
	u8						recvBitIndex;								//当前发送的位
	u8						recvCntIndex;								//当前发送的帧数
	u8						recvData[OW_MAX_DATA_LEN];	//需要发送的数据
	u8						recvLenght;									//需要发送的字节长度
	
	dataForm_t		form;												//数据格式
}ow_t;

void owInit(void);
void owConfig(dataForm_t *form);
void owSend(u8 *dat,u8 lenght);
u8 owRecvDataGet(u8 *data,u8 maxLen);
owState_t owSendStateGet(void);
owState_t owRecvStateGet(void);

#endif

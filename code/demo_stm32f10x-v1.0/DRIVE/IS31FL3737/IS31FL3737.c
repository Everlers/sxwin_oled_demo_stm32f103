#include "IS31FL3737.h"
#include "hal_time.h"
#include "hal_usart.h"
#include "delay.h"

is31fl3737_t is;
u8 isLedDuty[144]={0};
u8 isLedMode[144]={0};

static void writeOneByte(u8 devAddr,u8 writeAddr,u8 data);
static void writeNByte(u8 devAddr,u8 writeAddr,u8 *data,u8 len);
static void timeoutCallBack (void);
static void waitI2CDone(void);
static void i2cConfig (void);

void ISInit(void)
{
	i2cConfig();
	
	HalTimeRegisteringCallbacks(timeoutCallBack,1);
	is.I2CError = I2C_NOT_ERR;
	is.I2CState = I2C_IDLE;
	is.timeout = 0;
	
	IS_IO_SDB = 0;
	
	//设定所有LED占空比
	waitI2CDone();
	ISCmdChange(IS_CMD_PWM_DUTY);//切换PWM占空比控制寄存器 PG1
	for(int i=0;i<144;i++)isLedDuty[i] = 0;
	ISUpdateLedDuty();
	
	//设定LED模式为PWM模式
	waitI2CDone();
	ISCmdChange(IS_CMD_AUTO_BREATH);//切换到模式控制 PG2
	for(int i=0;i<144;i++)isLedMode[i] = 0x00;
	ISUpdateLedMode();
	
	//打开所有LED开关
	waitI2CDone();
	ISCmdChange(IS_CMD_LED_CTRL);//切换LED开关控制 PG0
	for(int i=0;i<24;i++)isLedDuty[i] = 0x3F;
	writeNByte(IS_DEV_ADDR,IS_LED_CTRL_ON_OFF_ADDR,isLedDuty,24);
	
	waitI2CDone();
	for(int i=0;i<144;i++)isLedDuty[i] = 0;
	
	//初始化IS31FL3737
	writeOneByte(IS_DEV_ADDR,IS_REGADDR_INT_MASK,0);//关闭所有中断
	ISCmdChange(IS_CMD_FUN_CONFIG);//切换到功能配置寄存器 PG3
	//自动呼吸模式1的配置
	writeOneByte(IS_DEV_ADDR,IS_FUN_SET_FADE_IN_ABM1,0x00);//设置淡入和保持时间
	writeOneByte(IS_DEV_ADDR,IS_FUN_SET_FADE_OUT_ABM1,0x00);//设置淡出和保持时间
	writeOneByte(IS_DEV_ADDR,IS_FUN_SET_LOOP1_ABM1,0x00);//设置循环1
	writeOneByte(IS_DEV_ADDR,IS_FUN_SET_LOOP2_ABM1,0x00);//设置循环2
	//自动呼吸模式2的配置
	writeOneByte(IS_DEV_ADDR,IS_FUN_SET_FADE_IN_ABM2,0x00);//设置淡入和保持时间
	writeOneByte(IS_DEV_ADDR,IS_FUN_SET_FADE_OUT_ABM2,0x00);//设置淡出和保持时间
	writeOneByte(IS_DEV_ADDR,IS_FUN_SET_LOOP1_ABM2,0x00);//设置循环1
	writeOneByte(IS_DEV_ADDR,IS_FUN_SET_LOOP2_ABM2,0x00);//设置循环2
	//自动呼吸模式3的配置
	writeOneByte(IS_DEV_ADDR,IS_FUN_SET_FADE_IN_ABM3,0x00);//设置淡入和保持时间
	writeOneByte(IS_DEV_ADDR,IS_FUN_SET_FADE_OUT_ABM3,0x00);//设置淡出和保持时间
	writeOneByte(IS_DEV_ADDR,IS_FUN_SET_LOOP1_ABM3,0x00);//设置循环1
	writeOneByte(IS_DEV_ADDR,IS_FUN_SET_LOOP2_ABM3,0x00);//设置循环2
	
	writeOneByte(IS_DEV_ADDR,IS_FUN_SET_ABM_UPDATE_TIME,0);//设置自动更新配置的更新时间 1Byte
	
	writeOneByte(IS_DEV_ADDR,IS_FUN_SWy_PULL_UP_SEL,IS_SWy_PULL_UP_DOWN_NOT);//设定SWy上拉的选择
	writeOneByte(IS_DEV_ADDR,IS_FUN_CSx_PULL_DOWN_SEL,IS_SWy_PULL_UP_DOWN_NOT);//设定CSx下拉的	选择
	
	writeOneByte(IS_DEV_ADDR,IS_FUN_OPRATION_MODE_ADDR,IS_OPRATION_MODE_SYNC_HI|IS_OPRATION_MODE_SSD|IS_OPRATION_MODE_B_EN);//设定操作模式为 同步为高阻抗、正常操作、
	writeOneByte(IS_DEV_ADDR,IS_FUN_GLOBAL_CUR_ADDR,(IS_GLOBAL_CUR_VALUE*255)/(840/IS_REXT_IO_RES_VALUE));//设定全局电流
	
	ISCmdChange(IS_CMD_PWM_DUTY);//切换PWM占空比控制寄存器 PG1
	IS_IO_SDB = 1;
	
	is.isUpdate = IS_AUTO_UPDATE;//开启的自动刷新显存
	
	if(is.isUpdate == IS_AUTO_UPDATE)
	{
		is.autoIndex = 0;
		writeNByte(IS_DEV_ADDR,IS_LED_CTRL_ON_OFF_ADDR,isLedDuty,6);
	}
}

static void i2cConfig (void)
{
	I2C_InitTypeDef I2C_InitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	//尝试解决硬件I2C的BUSY一直置位的情况
	//I2C_DeInit(I2C1);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	GPIOB->ODR |= GPIO_Pin_6|GPIO_Pin_7;//将CLK&SDA拉高
	
	I2C1->CR1 = I2C_CR1_SWRST;//复位I2C控制器
	I2C1->CR1 = 0;//解除复位动作
	
	//初始化I2C
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;//开启ACK
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;//7位地址
	I2C_InitStruct.I2C_ClockSpeed = 400*1000;//时钟速率
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;//快速模式的占空比
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;//设置I2C模式
	I2C_InitStruct.I2C_OwnAddress1 = 0x12;//设定主机地址
	I2C_Init(I2C1,&I2C_InitStruct);
	I2C_ITConfig(I2C1,I2C_IT_EVT,ENABLE);//开启事件中断
	I2C_ITConfig(I2C1,I2C_IT_ERR,ENABLE);//开启错误中断
	I2C_Cmd(I2C1,ENABLE);
	I2C_AcknowledgeConfig(I2C1,ENABLE);
	
	NVIC_InitStruct.NVIC_IRQChannel = I2C1_EV_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStruct);
}

//指令切换
void ISCmdChange(u8 cmd)
{
	writeOneByte(IS_DEV_ADDR,IS_REGADDR_LOCK,IS_UNLOCK);//解锁CMD寄存器一次
	writeOneByte(IS_DEV_ADDR,IS_REGADDR_CMD,cmd);//设置指令寄存器
}

//设定LED占空比
//x and y:0~11
//duty:0~255
void ISSetLedDuty(u8 x,u8 y,u8 duty)
{
	isLedDuty[x+(y*12)] = duty;
}

//更新所有LED占空比
//注意：将指令切换到PG1 （LED占空比控制）
void ISUpdateLedDuty(void)
{
	for(int x=0;x<12;x++)
	{
		waitI2CDone();
		writeNByte(IS_DEV_ADDR,IS_LED_CTRL_ON_OFF_ADDR+(x<<4),&isLedDuty[x*12],6);
		waitI2CDone();
		writeNByte(IS_DEV_ADDR,IS_LED_CTRL_ON_OFF_ADDR+(x<<4)+8,&isLedDuty[x*12],6);
	}
}

void ISUpdateLedMode(void)
{
	for(int x=0;x<12;x++)
	{
		waitI2CDone();
		writeNByte(IS_DEV_ADDR,IS_AUTO_MODE_ADDR+(x<<4),&isLedMode[x*12],6);
		waitI2CDone();
		writeNByte(IS_DEV_ADDR,IS_AUTO_MODE_ADDR+(x<<4)+8,&isLedMode[x*12],6);
	}
}

static void waitI2CDone(void)
{
	is.timeout = 200;
	while(is.timeout && is.I2CState != I2C_IDLE);
}

static void writeOneByte(u8 devAddr,u8 writeAddr,u8 data)
{
	waitI2CDone();
	if(is.I2CState == I2C_IDLE)
	{
		is.dir = I2C_WRITE;
		is.addr = writeAddr;
		is.data = &data;
		is.len = 1;
		is.index = 0;
		is.timeout = IS_I2C_TIMEOUT;
		while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY) == SET && is.timeout);//等待总线空闲
		if(is.timeout)//如果未超时
		{
			is.I2CState = I2C_START;
			I2C_GenerateSTART(I2C1,ENABLE);//发送一个开始信号
		}
		else
			is.I2CError = I2C_HW_TIMEOUT;
	}
	delay_ms(1);
}

static void writeNByte(u8 devAddr,u8 writeAddr,u8 *data,u8 len)
{
	if(is.I2CError != I2C_NOT_ERR)//如果有错误
	{
		is.timeout = 0;
		is.I2CState = I2C_IDLE;
		is.I2CError = I2C_NOT_ERR;
		I2C_GenerateSTOP(I2C1,ENABLE);
		I2C_ITConfig(I2C1, I2C_IT_ERR, ENABLE);
	}
	if(is.I2CState == I2C_IDLE || is.timeout == 0)//如果总线空闲或者超时
	{
		is.dir = I2C_WRITE;
		is.addr = writeAddr;
		is.data = data;
		is.len = len;
		is.index = 0;
		is.timeout = IS_I2C_TIMEOUT;
		while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY) == SET && is.timeout);//等待总线空闲
		if(is.timeout)//如果未超时
		{
			is.I2CState = I2C_START;
			I2C_GenerateSTART(I2C1,ENABLE);//发送一个开始信号
		}
		else
			is.I2CError = I2C_HW_TIMEOUT;
		is.timeout = IS_I2C_TIMEOUT*len;
	}
}

void I2C1_EV_IRQHandler (void)
{
	is.timeout = IS_I2C_TIMEOUT;
	switch((u8)is.I2CState)
	{
		case I2C_START:
			if(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT) == SET)
			{
				is.I2CState = I2C_ADDR;
				I2C_Send7bitAddress(I2C1,IS_DEV_ADDR,I2C_Direction_Transmitter);//发送I2C地址
			}
		break;
			
		case I2C_ADDR:
			if(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
			{
				I2C_SendData(I2C1,is.addr);//发送寄存器地址
				is.I2CState = I2C_DATA;
			}
		break;
			
		case I2C_DATA:
			if(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTING))
			{
				if(is.len > is.index)
				{
					I2C_SendData(I2C1,is.data[is.index]);
					is.I2CState = I2C_DATA;
					is.index++;
				}
				else
				{
					is.I2CState = I2C_IDLE;
					I2C_GenerateSTOP(I2C1,ENABLE);
					if(is.isUpdate == IS_AUTO_UPDATE)//如果开启自动更新
					{
						if(is.autoIndex >= 144)//如果发送完
							is.autoIndex = 0;
						else
							is.autoIndex += 6;
						
						if(is.autoIndex % 12)//如果只发完前6个字节
							is.addr = IS_PWM_DUTY_ADDR+((is.autoIndex/12)<<4)+8;
						else
							is.addr = IS_PWM_DUTY_ADDR+((is.autoIndex/12)<<4);
						is.data = &isLedDuty[is.autoIndex];
						is.len = 6;
						is.index = 0;
						is.timeout = IS_I2C_TIMEOUT;
						is.I2CState = I2C_START;
						while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY) == SET && is.timeout);//等待总线空闲
						I2C_GenerateSTART(I2C1,ENABLE);//发送一个开始信号
					}
				}
			}
		break;
	}
	I2C_ClearITPendingBit(I2C1, I2C_IT_EVT);
}

void I2C1_ER_IRQHandler(void)
{
    if(SET == I2C_GetITStatus(I2C1, I2C_IT_BERR)) is.I2CError = I2C_HW_BERR;
    else if(SET == I2C_GetITStatus(I2C1, I2C_IT_ARLO)) is.I2CError = I2C_HW_ARLO;
    else if(SET == I2C_GetITStatus(I2C1, I2C_IT_AF)) is.I2CError = I2C_HW_AF;
    else if(SET == I2C_GetITStatus(I2C1, I2C_IT_OVR)) is.I2CError = I2C_HW_OVR;
    else if(SET == I2C_GetITStatus(I2C1, I2C_IT_PECERR)) is.I2CError = I2C_HW_PECERR;
    else if(SET == I2C_GetITStatus(I2C1, I2C_IT_TIMEOUT)) is.I2CError = I2C_HW_TIMEOUT;
    else if(SET == I2C_GetITStatus(I2C1, I2C_IT_SMBALERT)) is.I2CError = I2C_HW_SMBALERT;
    else{}
    I2C_ClearITPendingBit(I2C1, I2C_IT_SMBALERT|I2C_IT_TIMEOUT|I2C_IT_PECERR|I2C_IT_OVR|I2C_IT_AF|I2C_IT_ARLO|I2C_IT_BERR);
    I2C_ITConfig(I2C1, I2C_IT_ERR, DISABLE);
}

static void timeoutCallBack (void)
{
	if(is.timeout)
		is.timeout--;
}

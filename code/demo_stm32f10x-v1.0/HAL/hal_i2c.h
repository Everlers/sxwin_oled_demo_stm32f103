#ifndef _HAL_I2C_H_
#define _HAL_I2C_H
#include "stm32f10x.h"
#define IIC_PORT_CLOCK								RCC_APB2Periph_GPIOB
#define IIC_PORT											GPIOB
#define IIC_SCL												GPIO_Pin_6
#define IIC_SDA												GPIO_Pin_7
#define IIC_SCL_BIT										0//设置引脚位置
#define IIC_SDA_BIT										1

#define IIC_SCL_SET										IIC_PORT->ODR |= IIC_SCL
#define IIC_SCL_CLR										IIC_PORT->ODR &= ~IIC_SCL
#define IIC_SDA_SET										IIC_PORT->ODR |= IIC_SDA
#define IIC_SDA_CLR										IIC_PORT->ODR &= ~IIC_SDA
#define IIC_SDA_DAT										IIC_PORT->IDR & IIC_SDA
#define IIC_SDA_IN										IIC_PORT->CRL &= ~((u32)0x0f << (IIC_SDA_BIT *4));IIC_PORT->CRL |= ((u32)8 << (IIC_SDA_BIT *4))//上拉/下拉输入
#define IIC_SDA_OUT										IIC_PORT->CRL &= ~((u32)0x0f << (IIC_SDA_BIT *4));IIC_PORT->CRL |= ((u32)3 << (IIC_SDA_BIT *4))//推挽输出模式

void HalI2cInit(void);
void HalI2cStart(void);
void HalI2cStop(void);
u8 HalI2cWaitAck(void);
void HalI2cWriteByte(u8 txd);
u8 HalI2cReadByte(unsigned char ack);
#endif

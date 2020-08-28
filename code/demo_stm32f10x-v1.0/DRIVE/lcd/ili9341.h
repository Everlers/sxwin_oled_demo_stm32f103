#ifndef _ILI9341_H_
#define _ILI9341_H_
#include "stm32f10x.h"

#define ILI_SPI_PORT				HAL_SPI_1
#define ILI_GPIO_PORT				GPIOB
#define ILI_BLK_BIT					GPIO_Pin_12
#define ILI_DC_BIT					GPIO_Pin_1
#define ILI_RST_BIT					GPIO_Pin_0

#define ILI_BLK_SET					ILI_GPIO_PORT->ODR |= ILI_BLK_BIT
#define ILI_BLK_CLR					ILI_GPIO_PORT->ODR &= ~ILI_BLK_BIT
#define ILI_DC_SET					ILI_GPIO_PORT->ODR |= ILI_DC_BIT
#define ILI_DC_CLR					ILI_GPIO_PORT->ODR &= ~ILI_DC_BIT
#define ILI_RST_SET					ILI_GPIO_PORT->ODR |= ILI_RST_BIT
#define ILI_RST_CLR					ILI_GPIO_PORT->ODR &= ~ILI_RST_BIT

void ili_init(void);
void ili_rst(void);
void ili_blk_on(void);
void ili_blk_off(void);
void ili_write_byte(u8 byte);
void ili_write_short(u16 dat);
void ili_write_reg(u8 byte);
void ili_write_reg_data(u8 reg,u16 data);
void ili_config(void);
void ili_addr_set(u16 x1,u16 y1,u16 x2,u16 y2);

#endif

#include "ili9341.h"
#include "hal_spi.h"
#include "FreeRTOS.h"
#include "task.h"

void ili_init(void)
{
	GPIO_InitTypeDef GPIO_Struct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_Struct.GPIO_Pin = ILI_BLK_BIT|ILI_DC_BIT|ILI_RST_BIT;
	GPIO_Struct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Struct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ILI_GPIO_PORT,&GPIO_Struct);
	GPIO_SetBits(ILI_GPIO_PORT,ILI_DC_BIT|ILI_RST_BIT);
	GPIO_ResetBits(ILI_GPIO_PORT,ILI_BLK_BIT);
	
	/*SPI1∂Àø⁄…Ë÷√*/
	GPIO_Struct.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_Struct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Struct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Struct);
	
	HalSPIInit(ILI_SPI_PORT, HAL_SPI_CPOL_HIGH, HAL_SPI_CPHA_1EDGE, HAL_SPI_BAUD_2);
	ili_rst();
	ili_config();
}

void ili_write_byte(u8 byte)
{
	ILI_DC_SET;
	HalSPIWriteByte(ILI_SPI_PORT,byte);
}

void ili_write_short(u16 dat)
{
	ILI_DC_SET;
	HalSPIWriteByte(ILI_SPI_PORT,dat>>8);
	HalSPIWriteByte(ILI_SPI_PORT,dat);
}

void ili_write_reg(u8 byte)
{
	ILI_DC_CLR;
	HalSPIWriteByte(ILI_SPI_PORT,byte);
}

void ili_write_reg_data(u8 reg,u16 data)
{
	ili_write_reg(reg);
	ili_write_short(data);
}

void ili_addr_set(u16 x1,u16 y1,u16 x2,u16 y2)
{
	ili_write_reg(0x2a);
	ili_write_short(x1);
	ili_write_short(x2);

	ili_write_reg(0x2b);
	ili_write_short(y1);
	ili_write_short(y2);

	ili_write_reg(0x2C);					 						 
}

void ili_config(void)
{
	ili_write_reg(0xCF);  
	ili_write_byte(0x00); 
	ili_write_byte(0xD9); 
	ili_write_byte(0X30); 
	 
	ili_write_reg(0xED);  
	ili_write_byte(0x64); 
	ili_write_byte(0x03); 
	ili_write_byte(0X12); 
	ili_write_byte(0X81); 
	 
	ili_write_reg(0xE8);  
	ili_write_byte(0x85); 
	ili_write_byte(0x10); 
	ili_write_byte(0x78); 
	 
	ili_write_reg(0xCB);  
	ili_write_byte(0x39); 
	ili_write_byte(0x2C); 
	ili_write_byte(0x00); 
	ili_write_byte(0x34); 
	ili_write_byte(0x02); 
	 
	ili_write_reg(0xF7);  
	ili_write_byte(0x20); 
	 
	ili_write_reg(0xEA);  
	ili_write_byte(0x00); 
	ili_write_byte(0x00); 
	 
	ili_write_reg(0xC0);    //Power control 
	ili_write_byte(0x21);   //VRH[5:0] 
	 
	ili_write_reg(0xC1);    //Power control 
	ili_write_byte(0x12);   //SAP[2:0];BT[3:0] 
	
	ili_write_reg(0xC5);    //VCM control 
	ili_write_byte(0x32); 
	ili_write_byte(0x3C); 
	 
	ili_write_reg(0xC7);    //VCM control2 
	ili_write_byte(0XC1); 
	 
	ili_write_reg(0x36);    // Memory Access Control 
	ili_write_byte(0x68);   //0x08: ˙∆¡  0xA8:◊Û∫·∆¡ 0x68:”“∫·∆¡
	 
	ili_write_reg(0x3A);   
	ili_write_byte(0x55); 

	ili_write_reg(0xB1);   
	ili_write_byte(0x00);   
	ili_write_byte(0x18); 
	 
	ili_write_reg(0xB6);    // Display Function Control 
	ili_write_byte(0x0A); 
	ili_write_byte(0xA2); 
	 
	ili_write_reg(0xF2);    // 3Gamma Function Disable 
	ili_write_byte(0x00); 
	 
	ili_write_reg(0x26);    //Gamma curve selected 
	ili_write_byte(0x01); 
	 
	ili_write_reg(0xE0);    //Set Gamma 
	ili_write_byte(0x0F); 
	ili_write_byte(0x20); 
	ili_write_byte(0x1E); 
	ili_write_byte(0x09); 
	ili_write_byte(0x12); 
	ili_write_byte(0x0B); 
	ili_write_byte(0x50); 
	ili_write_byte(0XBA); 
	ili_write_byte(0x44); 
	ili_write_byte(0x09); 
	ili_write_byte(0x14); 
	ili_write_byte(0x05); 
	ili_write_byte(0x23); 
	ili_write_byte(0x21); 
	ili_write_byte(0x00); 
	 
	ili_write_reg(0XE1);    //Set Gamma 
	ili_write_byte(0x00); 
	ili_write_byte(0x19); 
	ili_write_byte(0x19); 
	ili_write_byte(0x00); 
	ili_write_byte(0x12); 
	ili_write_byte(0x07); 
	ili_write_byte(0x2D); 
	ili_write_byte(0x28); 
	ili_write_byte(0x3F); 
	ili_write_byte(0x02); 
	ili_write_byte(0x0A); 
	ili_write_byte(0x08); 
	ili_write_byte(0x25); 
	ili_write_byte(0x2D); 
	ili_write_byte(0x0F); 
	 
	ili_write_reg(0x11);    //Exit Sleep 
	vTaskDelay(120); 
	ili_write_reg(0x29);    //Display on 
}

void ili_blk_on(void)
{
	ILI_BLK_SET;
}

void ili_blk_off(void)
{
	ILI_BLK_CLR;
}

void ili_rst(void)
{
	ILI_RST_CLR;
	vTaskDelay(100);
	ILI_RST_SET;
	vTaskDelay(100);
}

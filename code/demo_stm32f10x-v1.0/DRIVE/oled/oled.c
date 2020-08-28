#include "oled.h"
#include "font.c"
#include "hal_spi.h"
#include "FreeRTOS.h"
#include "task.h"

static void writeByte(u8 dat,oledCmd_t cmd);

void oledInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	HalSPIInit(HAL_SPI_1,HAL_SPI_CPOL_HIGH,HAL_SPI_CPHA_2EDGE,HAL_SPI_BAUD_2);
	OLED_RST = 1;
	vTaskDelay(100);
	OLED_RST = 0;
	vTaskDelay(100);
	OLED_RST = 1;
	OLED_CS = 0;
	
	writeByte(0xAE,OLED_CMD);//--turn off oled panel
	writeByte(0x00,OLED_CMD);//---set low column address
	writeByte(0x10,OLED_CMD);//---set high column address
	writeByte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	writeByte(0x81,OLED_CMD);//--set contrast control register
	writeByte(0xCF,OLED_CMD); // Set SEG Output Current Brightness
	writeByte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	writeByte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	writeByte(0xA6,OLED_CMD);//--set normal display
	writeByte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	writeByte(0x3f,OLED_CMD);//--1/64 duty
	writeByte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	writeByte(0x00,OLED_CMD);//-not offset
	writeByte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
	writeByte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
	writeByte(0xD9,OLED_CMD);//--set pre-charge period
	writeByte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	writeByte(0xDA,OLED_CMD);//--set com pins hardware configuration
	writeByte(0x12,OLED_CMD);
	writeByte(0xDB,OLED_CMD);//--set vcomh
	writeByte(0x40,OLED_CMD);//Set VCOM Deselect Level
	writeByte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
	writeByte(0x02,OLED_CMD);//
	writeByte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
	writeByte(0x14,OLED_CMD);//--set(0x10) disable
	writeByte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
	writeByte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
	writeByte(0xAF,OLED_CMD);//--turn on oled panel
}

void oledFill(u8 x,u8 y,u8 x1,u8 y1)
{
	u8 x2,y2;
	for(y2 = y;y2<(y1+1);y2++)
	{
		oledSetPos(x,y2);
		for(x2=0;x2<x1-x+1;x2++)
			writeByte(0,OLED_DAT);
	}
}

void oledShowString(u8 x,u8 y,char *s)
{
	unsigned char j=0;
	while (s[j]!='\0')
	{
		oledShowChr(x,y,s[j]);
		x+=8;
		if(x>120){x=0;y+=2;}
			j++;
	}
}

//显示一个字符
void oledShowChr(u8 x,u8 y,char chr)
{
	unsigned char c=0,i=0;	
	c=chr-' ';//得到偏移后的值			
	if(x>128-1){x=0;y=y+2;}
	oledSetPos(x,y);	
	for(i=0;i<8;i++)
		writeByte(F8X16[c*16+i],OLED_DAT);
	oledSetPos(x,y+1);
	for(i=0;i<8;i++)
		writeByte(F8X16[c*16+i+8],OLED_DAT);
}

//清除显示
void oledClear(void)
{
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		writeByte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		writeByte (0x00,OLED_CMD);      //设置显示位置—列低地址
		writeByte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)writeByte(0,OLED_DAT); 
	} //更新显示
}

//开启OLED显示
void oledDisplayOn(void)
{
	writeByte(0X8D,OLED_CMD);  //SET DCDC命令
	writeByte(0X14,OLED_CMD);  //DCDC ON
	writeByte(0XAF,OLED_CMD);  //DISPLAY ON
}

//关闭OLED显示
void oledDisplayOff(void)
{
	writeByte(0X8D,OLED_CMD);  //SET DCDC命令
	writeByte(0X10,OLED_CMD);  //DCDC ON
	writeByte(0XAE,OLED_CMD);  //DISPLAY ON
}

//设置x,y坐标
void oledSetPos(u8 x,u8 y)
{
	writeByte(0xb0+y,OLED_CMD);
	writeByte(((x&0xf0)>>4)|0x10,OLED_CMD);
	writeByte((x&0x0f)|0x01,OLED_CMD); 
}

static void writeByte(u8 dat,oledCmd_t cmd)
{
	OLED_DC = cmd;
	HalSPIWriteByte(HAL_SPI_1,dat);  
}

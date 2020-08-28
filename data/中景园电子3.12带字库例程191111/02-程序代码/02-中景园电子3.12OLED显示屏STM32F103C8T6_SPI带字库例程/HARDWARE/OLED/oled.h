#ifndef __OLED_H
#define __OLED_H 

#include "sys.h"

//-----------------测试LED端口定义---------------- 

#define LED_ON GPIO_ResetBits(GPIOA,GPIO_Pin_15)
#define LED_OFF GPIO_SetBits(GPIOA,GPIO_Pin_15)

//-----------------OLED端口定义---------------- 

#define OLED_SCLK_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_0)//SCLK
#define OLED_SCLK_Set() GPIO_SetBits(GPIOA,GPIO_Pin_0)

#define OLED_SDIN_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_1)//SDA
#define OLED_SDIN_Set() GPIO_SetBits(GPIOA,GPIO_Pin_1)

#define OLED_RST_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_2)//RES
#define OLED_RST_Set() GPIO_SetBits(GPIOA,GPIO_Pin_2)

#define OLED_DC_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_3)//DC
#define OLED_DC_Set() GPIO_SetBits(GPIOA,GPIO_Pin_3)
 		     
#define OLED_CS_Clr()  GPIO_ResetBits(GPIOA,GPIO_Pin_4)//CS1
#define OLED_CS_Set()  GPIO_SetBits(GPIOA,GPIO_Pin_4)

//----------------------字库引脚配置---------------------------------------
#define ZK_OUT()    GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)//读取指定管脚的输入FS0
	
#define ZK_CS_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_6)//字库片选CS2
#define ZK_CS_Set() GPIO_SetBits(GPIOA,GPIO_Pin_6)	


#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据


void Show_Pattern(unsigned char *Data_Pointer, unsigned char a, unsigned char b, unsigned char c, unsigned char d);
void OLED_WR_Byte(u8 dat,u8 mode);
void Column_Address(u8 a,u8 b);
void Row_Address(u8 a,u8 b);
void OLED_Fill(u8 color);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 sizey);
void OLED_ShowString(u8 x,u8 y,u8 *dp,u8 sizey);
u32 oled_pow(u8 m,u8 n);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 sizey);
void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 sizey);
void OLED_DrawBMP(u8 *BMP,u8 Gray_Level,u8 Color);
void Init_SSD1322(void);
void OLED_ShowChar6x8(u8 x,u8 y,u8 chr);


//字库函数
void Column_Address1(u8 a,u8 b);
void Row_Address1(u8 a,u8 b);
void ZK_command( unsigned char  data );
unsigned char  get_data_from_ROM(void);
void get_n_bytes_data_from_ROM(unsigned char  AddrHigh,unsigned char  AddrMid,unsigned char  AddrLow,unsigned char  *pBuff,unsigned char  DataLen );
void Display_GB2312_String(unsigned int x,unsigned int y,unsigned char zk_num, unsigned char  text[]);	// 显示汉字字符串
void Display_GB2312(unsigned int x,unsigned int y,unsigned char zk_num);								 //	显示汉字
void Display_GBasc_String(unsigned int x,unsigned int y,unsigned char zk_num,unsigned char  text[]);	  // 显示GB2312 ascii 字符串
void Display_GBasc(unsigned int x,unsigned int y,unsigned char zk_num);								  //  显示GB2312 ascii 
void Display_Asc_String(unsigned int x,unsigned int y,unsigned char zk_num, unsigned char  text[]);		  //   显示ASCII 字符串
void Display_Asc(unsigned int x,unsigned int y,unsigned char zk_num);									  //   显示ASCII
void Con_4_byte(unsigned char DATA);
#endif








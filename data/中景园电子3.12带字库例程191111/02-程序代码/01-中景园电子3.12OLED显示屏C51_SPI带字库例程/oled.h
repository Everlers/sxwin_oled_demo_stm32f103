#ifndef __OLED_H
#define __OLED_H

#include "REG52.h"

sbit SCLK=P1^0;
sbit SDIN=P1^1;
sbit RST=P1^2;
sbit DC=P1^3;
sbit CS=P1^4;
sbit FS0=P1^5;
sbit CS2=P0^6;

#define u8  unsigned char
#define u32 unsigned int
#define OLED_CMD  0
#define OLED_DATA 1

#define OLED_SCLK_Clr() SCLK=0 //CLK
#define OLED_SCLK_Set() SCLK=1

#define OLED_SDIN_Clr() SDIN=0	//DIN
#define OLED_SDIN_Set() SDIN=1


#define OLED_RST_Clr() RST=0	//RES
#define OLED_RST_Set() RST=1

#define OLED_DC_Clr() DC=0	//DC
#define OLED_DC_Set() DC=1

#define OLED_CS_Clr()  CS=0	//CS
#define OLED_CS_Set()  CS=1

//----------------------×Ö¿âÒý½ÅÅäÖÃ---------------------------------------
#define ZK_OUT()     FS0
	
#define ZK_CS_Clr() CS2=0
#define ZK_CS_Set() CS2=1


void delay_ms(unsigned int ms);
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

//×Ö¿âº¯Êý
void Column_Address1(u8 a,u8 b);
void Row_Address1(u8 a,u8 b);
void ZK_command( unsigned char  data1 );
unsigned char  get_data_from_ROM(void);
void get_n_bytes_data_from_ROM(unsigned char  AddrHigh,unsigned char  AddrMid,unsigned char  AddrLow,unsigned char  *pBuff,unsigned char  DataLen );
void Display_GB2312_String(unsigned int x,unsigned int y,unsigned char zk_num, unsigned char  text[]);	// ÏÔÊ¾ºº×Ö×Ö·û´®
void Display_GB2312(unsigned int x,unsigned int y,unsigned char zk_num);								 //	ÏÔÊ¾ºº×Ö
void Display_GBasc_String(unsigned int x,unsigned int y,unsigned char zk_num,unsigned char  text[]);	  // ÏÔÊ¾GB2312 ascii ×Ö·û´®
void Display_GBasc(unsigned int x,unsigned int y,unsigned char zk_num);								  //  ÏÔÊ¾GB2312 ascii 
void Display_Asc_String(unsigned int x,unsigned int y,unsigned char zk_num, unsigned char  text[]);		  //   ÏÔÊ¾ASCII ×Ö·û´®
void Display_Asc(unsigned int x,unsigned int y,unsigned char zk_num);									  //   ÏÔÊ¾ASCII
void Con_4_byte(unsigned char DATA);
#endif


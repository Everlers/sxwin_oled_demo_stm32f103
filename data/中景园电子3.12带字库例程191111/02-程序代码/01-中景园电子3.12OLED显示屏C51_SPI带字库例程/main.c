//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//中景园电子
//店铺地址：http://shop73023976.taobao.com/?spm=2013.1.0.0.M4PqC2
//
//  文 件 名   : main.c
//  版 本 号   : v2.0
//  作    者   : HuangKai
//  生成日期   : 2018-10-31
//  最近修改   : 
//  功能描述   : 灰度OLED SPI接口演示例程(51系列)
//              说明: 
//              ----------------------------------------------------------------
//              GND    电源地
//              VCC  3.3v电源
//              D0   P1^0（SCL）
//              D1   P1^1（SDA）
//              RES  P12
//              DC   P13
//              CS1   P14  
//              FSO   P15
//              CS2   P06 
//              ----------------------------------------------------------------
// 修改历史   :
// 日    期   : 
// 作    者   : HuangKai
// 修改内容   : 创建文件
//版权所有，盗版必究。
//Copyright(C) 中景园电子2018-10-31
//All rights reserved
//******************************************************************************/
#include "REG52.h"
#include "oled.h"
#include "bmp.h"




int main(void)
{
	u8 t=' ';
	Init_SSD1322();
	OLED_Fill(0x00);
	while(1)
	{
		OLED_DrawBMP(BMP1,0,0xff);
		delay_ms(1000);
		OLED_Fill(0x00);
		OLED_DrawBMP(BMP2,1,0xff);
		delay_ms(1000);
		OLED_Fill(0x00);
		Display_Asc_String(0,0,1,"ZJY.ASCII 5X7");
		Display_Asc_String(0,10,2,"ZJY.ASCII 7X8");
		Display_Asc_String(0,20,3,"ZJY.ASCII 6X12");
		Display_Asc_String(0,36,4,"ZJY.ASCII 8X16");
		delay_ms(1000);
		OLED_Fill(0x00);
		Display_Asc_String(0,0,5,"ZJY.ASCII 12X24");
		Display_Asc_String(0,30,6,"ZJY.ASCII 16X32");
		delay_ms(1000);
		OLED_Fill(0x00);
		Display_GB2312_String(0,0,1,"中景园电子技术");//中文12x12
		Display_GB2312_String(0,14,2,"中景园电子技术");//中文15x16
		Display_GB2312_String(0,34,3,"中景园电子技术");//中文24x24
		delay_ms(1000);
		OLED_Fill(0x00);
		Display_GB2312_String(0,20,4,"中景园电子技术");//中文32x32
		delay_ms(1000);
		OLED_Fill(0x00);
  }
}
#include "TM1628.h"
#include "FreeRTOS.h"
#include "Task.h"

static u8 TMShowBuff[14]={0};

static void writeDat(u8 dat);
static void writeCmd(u8 cmd);

void TMInit(void)
{
	u8 i;
	for(i=0;i<14;i++)
		TMShowBuff[i] = 0xFF;
	TM_IO_CLK = 1;
	TM_IO_DIO = 1;
	TM_IO_STB = 1;
	writeCmd(TM_CMD_DISPLAY_MODE|0x03);//写入7位10段
	TMUpdateBuffToShow();//刷新显示
	TMDisplayCtrl(5,1);//亮度调整并打开显示
}

void TMSetShowBit(u8 com,u8 seg)
{
	
}

void TMUpdateBuffToShow(void)
{
	u8 i;
	TM_IO_STB = 0;
	writeDat(TM_CMD_DATA);//地址递增模式写入显示数据
	writeDat(TM_CMD_ADDR_CMD_SET|0);//设定显示地址为0
	for(i=0;i<14;i++)
		writeDat(TMShowBuff[i]);
	TM_IO_STB = 1;
}

//设定显示地址
void TMSetAddr(u8 addr)
{
	writeCmd(TM_CMD_ADDR_CMD_SET|addr);
}

//亮度调节以及显示开关
//brilliance:0~7
//onOrOff:true=on false=off
void TMDisplayCtrl(u8 brilliance,u8 onOrOff)
{
	writeCmd(TM_CMD_DISPLAY_CTRL|((onOrOff ? 1:0)<<3)|brilliance);
}

static void writeCmd(u8 cmd)
{
	TM_IO_STB = 0;
	//delay_ms(1);
	writeDat(cmd);
	TM_IO_STB = 1;
	//delay_ms(1);
}

static void writeDat(u8 dat)
{
	u8 i;
	TM_IO_CLK = 1;
	TM_IO_DIO = 1;
	for(i = 0; i < 8; i++)
	{
		TM_IO_CLK = 0;
		//delay_us(100);
		if((dat & 0x01) == 1)
			TM_IO_DIO = 1;
		else
			TM_IO_DIO = 0;
		dat = dat >> 1;
		TM_IO_CLK = 1;
		//delay_us(100);
	}
}

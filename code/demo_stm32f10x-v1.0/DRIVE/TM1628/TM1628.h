#ifndef _TM1628_H_
#define _TM1628_H_
#include "hal_sys.h"

#define TM_IO_DIO					PBout(4)
#define TM_IO_CLK					PBout(3)
#define TM_IO_STB					PBout(2)

#define TM_CMD_DISPLAY_MODE		0x00	//显示模式
#define TM_CMD_DATA						0x40	//数据命令设置
#define TM_CMD_DISPLAY_CTRL		0x80	//显示控制命令设置
#define TM_CMD_ADDR_CMD_SET		0xC0	//地址命令设置

#define TM_DISPLAY_CTRL_ON		0x08	//打开显示
#define TM_DISPLAY_CTEL_OFF		0x00	//关闭显示

void TMInit(void);
void TMDisplayCtrl(u8 brilliance,u8 onOrOff);
void TMSetAddr(u8 addr);
void TMUpdateBuffToShow(void);

#endif

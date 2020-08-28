#ifndef _LED_H_
#define _LED_H_
#include "hal_sys.h"

#define LED_NUMBER										5
#define LED_BRIGHTNESS_MAX						100//白灯最大亮度
#define LED_STREAM_SPEED							2 	//呼吸速度

#define LED_STA_STREAM_LOOP						0x01//循环流水灯
#define LED_STA_STREAM_MST						0x02//单次流水灯

typedef struct{
	FlagStatus 	sw[LED_NUMBER];//开关
	u8 					duty[LED_NUMBER];
	u8 					cnt;
	
	u8					state;
	u8					index;
	u8					speed;
}led_t;

void ledInit(void);
void ledTask(void);

FlagStatus ledSwitchGet(u8 index);
void ledSwitchSet(u8 index,FlagStatus sw);

u8 ledBrightnesGet (u8 index);
void ledBrightnesSet(u8 index,u8 brightness);

void ledStream(void);
void ledAllClose(void);
#endif

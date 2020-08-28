#ifndef _OLED_H_
#define _OLED_H_
#include "hal_sys.h"

#define OLED_CS								PAout(4)
#define OLED_DC								PAout(1)
#define OLED_RST							PAout(0)

#define OLED_HIGH							64
#define OLED_WIDTH						256

#define OLED_RAM_SIZE					(256*64/2)

typedef enum{
	OLED_CMD = 0,
	OLED_DAT = 1,
}oledCmd_t;

extern u16 oledShowBuff[OLED_WIDTH/4][OLED_HIGH];

void oledInit(void);
void oledClear(void);
void oledDisplayOn(void);
void oledDisplayOff(void);
void oledSetPos(u8 xStart,u8 yStart,u8 xEnd,u8 yEnd);
void oledShowBuffWrite(u8 x,u8 y,u8 depth);
u8 oledShowBuffRead(u8 x,u8 y);
void oledUpdateBuffToShow(void);

void oledShowChr8x16(u8 x,u8 y,char chr,u8 depth,u8 background);
void oledShowGb16x16(u8 x,u8 y,char *gb,u8 depth,u8 background);
void oledShowString(u8 x,u8 y,char *s,u8 depth,u8 background);
void oledShowImage(u8 xStart,u8 yStart,u8 xEnd,u8 yEnd,u8 *image);
void oledShow16GrayImage(u8 xStart,u8 yStart,u8 xEnd,u8 yEnd,u8 *image);
void oledShowBAWImage(u8 xStart,u8 yStart,u8 xEnd,u8 yEnd,u8 *image,u8 depth,u8 background);
void oledFill(u8 xStart,u8 xEnd,u8 yStart,u8 yEnd,u8 color);

void oledWriteNumber24x48ToBuff(u8 x,u8 y,char num,u8 depth,u8 background,u8 *buf,u8 width,u8 high);
void oledShowGb16x16ToBuff(u8 x,u8 y,char *gb,u8 depth,u8 background,u8 *buf,u8 width,u8 high);
void oledFillToBuff(u8 *buf,u8 width,u8 high,u8 xStart,u8 xEnd,u8 yStart,u8 yEnd,u8 depth);
#endif

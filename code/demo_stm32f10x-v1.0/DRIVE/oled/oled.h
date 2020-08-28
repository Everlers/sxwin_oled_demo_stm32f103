#ifndef _OLED_H_
#define _OLED_H_
#include "hal_sys.h"

#define OLED_CS								PBout(5)
#define OLED_DC								PBout(6)
#define OLED_RST							PBout(7)

typedef enum{
	OLED_CMD = 0,
	OLED_DAT = 1,
}oledCmd_t;

void oledInit(void);
void oledClear(void);
void oledDisplayOn(void);
void oledDisplayOff(void);
void oledSetPos(u8 x,u8 y);
void oledShowChr(u8 x,u8 y,char chr);
void oledShowString(u8 x,u8 y,char *s);
void oledFill(u8 x,u8 y,u8 x1,u8 y1);

#endif

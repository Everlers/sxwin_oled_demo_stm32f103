#ifndef _ROTARYSWITCH_H_
#define _ROTARYSWITCH_H_
#include "hal_sys.h"

#define ROTARY_SW_IO_A					PBin(2)
#define ROTARY_SW_IO_B					PBin(3)

typedef enum{
	ROTARYSW_NOT = 0,
	ROTARYSW_CW	 = 1,
	ROTARYSW_CCW = 2,
}rotarySwSta;

void rotarySwInit(void);
FlagStatus rotarySwGet(rotarySwSta r);
#endif

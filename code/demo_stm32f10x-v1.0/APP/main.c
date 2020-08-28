#include "stm32f10x.h"
#include "stdio.h"
#include "hal_sys.h"
#include "hal_key.h"
#include "hal_time.h"
#include "hal_usart.h"
#include "hal_buzzer.h"

#include "malloc.h"
#include "delay.h"
#include "encryption.h"
#include "timers.h"
#include "oled.h"
#include "fun.h"
#include "rtosEx.h"
#include "IS31FL3737.h"
#include "airConditioner.h"
#include "oneWire.h"
#include "voice.h"
#include "rotarySwitch.h"
#include "ui.h"
#include "led.h"
#include "image.h"

static void memerr(void);

int main (void)
{
	HalSysInit();
	delay_init();
	malloc_Init(memerr);
	
	HalTimeInit(72,100);
	HalKeyInit();
	HalBuzzerInit();

	HalTimeRegisteringCallbacks(HalBuzzerTask,10);
	HalTimeRegisteringCallbacks(HalKeyTask,10);
	HalTimeRegisteringCallbacks(ledTask,1);
	
	ledInit();
	owInit();
	ISInit();
	voiceInit();
	rotarySwInit();
	acLedShowInit();
	oledInit();
	funInit();
	uiInit();
	
	HalKeyGetLongPress(HAL_KEY_ALL);
	HalKeyGetShortPress(HAL_KEY_ALL);
	
	while(1)
	{
		acLedShowTask();
		voiceTask();
		funTask();
		uiTask();
	}
}

static void memerr(void)
{
	volatile u8 rate = SET;
	while(rate)rate = mem_rate();
}

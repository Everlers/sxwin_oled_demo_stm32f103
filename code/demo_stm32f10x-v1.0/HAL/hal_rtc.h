#ifndef __RTC_H
#define __RTC_H	    
#include "stm32f10x.h"
#include "stm32f10x_pwr.h"
#include "hal_sys.h"

typedef struct 
{
	vu8 hour;
	vu8 min;
	vu8 sec;			
	//公历日月年周
	vu16 w_year;
	vu8  w_month;
	vu8  w_date;
	vu8  week;		 
}HalRTCTime_t;	

typedef void (*HalRtcCallbacks_t ) (HalRTCTime_t *Calendar);

u8 HalRTCInit(void);        //初始化RTC,返回0,失败;1,成功;
HalRTCTime_t *HalRTCGetTimer(void); //查询RTC时间
u8 HalRTCIsLeapYear(u16 year);//平年,闰年判断
u8 HalRTCAlarmSet(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);//设置闹钟
u8 HalRTCGet(void);         //更新时间   
u8 HalRTCGetWeek(u16 year,u8 month,u8 day);
u8 HalRTCSet(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);//设置时间
void HalRtcRegisteringCallbacks(HalRtcCallbacks_t Callbacks);//注册RTC秒的回调
#endif



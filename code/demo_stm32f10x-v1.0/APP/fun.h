#ifndef _FUN_H_
#define _FUN_H_
#include "hal_sys.h"

#define AC_STA_POWER					0x0001//电源
#define AC_STA_TIME						0x0002//定时
#define AC_STA_AREFACTION			0x0004//除湿
#define AC_STA_SWIND					0x0008//扫风
#define AC_STA_WIFI						0x0010//WiFi

#define AC_STA_MSU						0x0020//模式显示更新
#define AC_STA_WSU						0x0040//扫风显示更新
#define AC_STA_ASU						0x0080//除湿显示更新
#define AC_STA_TSU						0x0100//定时显示更新
#define AC_STA_ISU						0x0200//WiFi显示更新
#define AC_STA_TSC						0x0400//温度显示改变
#define AC_STA_WSSC						0x0800//风速更新处理
#define AC_STA_TVSU						0x1000//定时值显示更新

#define AC_SET_TEMP_SHOW_TIME		3000	//设定温度的显示时间
#define AC_CUR_TEMP_UPDATE_TIME	2000	//当前温度的刷新时间
#define AC_SET_TIME_SHOW_TIME		3000	//设置定时的显示时间
#define AC_PON_FAST_COLD_TIME		3000	//开机速冷时间
#define AC_BUZZER_PERIOD				70

#define AC_HIGH_TEMP						40//最高温度
#define AC_LOW_TEMP							16//最低温度

#define AC_LABEL_TIME						"关机时间"
#define AC_LABEL_SET_TEMP				"设定温度"
#define AC_LABEL_CUR_TEMP				"当前温度"

typedef enum{
	AC_MODE_SLEEP = 0,//睡眠模式
	AC_MODE_WIND 	= 1,//送风模式
	AC_MODE_COLD	=	2,//制冷模式
	AC_MODE_HEAT	=	3,//制热模式
	AC_MODE_AUTO	=	4,//自动模式
}ac_mode_t;

typedef enum{
	AC_VALUE_TYPE_SET_TIME = 0,
	AC_VALUE_TYPE_SET_TEMP = 1,
	AC_VALUE_TYPE_CUR_TEMP = 2,
}valueType_t;

typedef struct{
	u8	type;
	u16	value;
}value_t;

typedef struct{
	u16 			sta;			//状态
	ac_mode_t showMode;	//显示的模式
	ac_mode_t	mode;			//模式
	u16				setTime;	//定时时间
	int				setTemp;	//设定温度
	int				curTemp;	//当前温度
	u8				winds;		//风速
	
	u16				fcoldTime;				//速冷时间
	value_t		showSetTime;
	value_t		showSetTemp;
	value_t 	showCurTemp;
	
	u16				setTempShowTime;	//设定温度显示时间
	u16				curTempUpdateTime;//当前温度更新时间
	u16				setTimeShowTime;	//设置定时显示时间
}ac_fun_t;

typedef struct{
	ac_fun_t ac;	//空调
}fun_t;
extern fun_t fun;

void funInit(void);
void funTask(void);

void funAcPowerOn(void);
void funAcPowerOff(void);
ErrorStatus funAcSetTemp(u8 temp);
void funAcSetWinds(u8 winds);
void funAcSetTime(u16 time);
void funAcSetMode(ac_mode_t mode);
void funAcSWind(FlagStatus sw);
void funAcArefaction(FlagStatus sw);
#endif

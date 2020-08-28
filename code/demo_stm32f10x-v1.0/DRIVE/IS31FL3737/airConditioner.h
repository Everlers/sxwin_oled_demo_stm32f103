#ifndef _AIRCONDITIONER_H_
#define _AIRCONDITIONER_H_
#include "hal_sys.h"

//14176
////////////////////////////////////////////////
//安睡模式 制热模式 自动模式 制冷模式 送风模式//
//																						//
//风速1																	风速3	//
//风速2							  电源							风速4	//
//							wifi 					℃							//
//							扫风 					分							//
//	定					速冷 					时					除	//
//	时																		湿	//
//				 设定温度 当前温度 设定时间					//
////////////////////////////////////////////////

#define AC_HIGHLIGHT_TIME					3000//高亮时间
#define AC_HIGHLIGHT_DUTY					255	//高亮占空比
#define AC_LOWLIGHT_DUTY					40	//低亮占空比
#define AC_NORMAL_DUTY						250	//正常占空比

typedef enum{
	AC_ICON_TEMP = 0,	//温度℃
	AC_ICON_WIFI,			//WiFi
	AC_ICON_POWER,		//电源
	AC_ICON_MIN,			//分
	AC_ICON_HOR,			//时
	AC_ICON_TIME,			//定时
	AC_ICON_AREF,			//除湿
	AC_ICON_FCOLD,		//速冷
	AC_ICON_AUTO,			//自动模式
	AC_ICON_COLD,			//制冷模式
	AC_ICON_HEAT,			//制热模式
	AC_ICON_WIND,			//扫风
	AC_ICON_CWIND,		//送风模式
	AC_ICON_SLEEP,		//睡眠图标
	AC_ICON_RCOLD,		//速冷图标
	AC_ICON_WINDS,		//风速图标
	AC_ICON_LABELSTE,	//标签(设定温度)
	AC_ICON_LABELCTE,	//标签(当前温度)
	AC_ICON_LABELSTI,	//标签(设定时间)
	AC_ICON_VALUE,		//值的显示
}ac_icon;

typedef struct{
	u8	state;			//状态
	u8	event;			//事件
	u8	sw[AC_ICON_VALUE+1];			//每个显示模块的开关
	u8 	curDuty[AC_ICON_VALUE+1];//每个显示模块的当前亮度
	u8 	setDuty[AC_ICON_VALUE+1];//每个显示模块的设定温度
	u8 	showValue;		//显示数值
	u16 highlightTime;//高亮时间
	u8 	pitchOn;			//选中的显示模块
	u8  adjustSpeed;	//调节速度
}ac_t;

void acLedShowInit (void);
void acLedShowTask (void);
u8 acLedShowCurDutyGet(ac_icon icon);
FlagStatus acLedShowAllUpdateDoneGet(void);
FlagStatus acLedsShowUpdateDoneGet (ac_icon icon);
FlagStatus acLedShowIconGet(ac_icon icon);
void acLedShowIconSet(ac_icon icon,FlagStatus highlight,FlagStatus sw,FlagStatus updateNow);
FlagStatus acLedShowValueGetSw(void);
void acLedShowValueSet(u8 value,FlagStatus highlight,FlagStatus sw);
#endif

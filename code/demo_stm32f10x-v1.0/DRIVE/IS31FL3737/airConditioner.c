#include "airConditioner.h"
#include "IS31FL3737.h"
#include "hal_time.h"
#include "fun.h"

static ac_t ac;
const u8 nlList[]={0xFC,0x60,0xDA,0xF2,0x66,0xB6,0xBE,0xE0,0xFE,0xF6,0x00};

static void showNum (u8 index,u8 num,u8 luminance);
static void timeCallBack (void);
static void acValueShow(u8 value,u8 luminance);
static void acIconShow(ac_icon icon,u8 luminance);
static void ledManage (void);

void acLedShowInit (void)
{
	HalTimeRegisteringCallbacks(timeCallBack,10);
	ac.showValue = 25;
	ac.pitchOn = 0;
	ac.highlightTime = 0;
	for(int i=0;i<sizeof(ac.sw);i++)
	{
		ac.curDuty[i] = 0;
		ac.setDuty[i] = 0;
		ac.sw[i] = 0;
	}
}

void acLedShowTask (void)
{
	ledManage();
}

static void ledManage (void)
{
	if(fun.ac.sta & AC_STA_POWER)//开机状态下
	{
		for(int i=0;i<sizeof(ac.setDuty);i++)//将开启的显示模块亮度调到正常占空比
		{
			if(ac.sw[i] && ac.setDuty[i] != AC_NORMAL_DUTY)
				ac.setDuty[i] = AC_NORMAL_DUTY;
		}
		ac.adjustSpeed = 0;
	}
	else//关机状态下
	{
		if(ac.setDuty[AC_ICON_POWER] == ac.curDuty[AC_ICON_POWER])
		{
			if(ac.setDuty[AC_ICON_POWER] == 0 || ac.setDuty[AC_ICON_POWER] == AC_LOWLIGHT_DUTY)
				ac.setDuty[AC_ICON_POWER] = AC_NORMAL_DUTY;
			else if(ac.setDuty[AC_ICON_POWER] == AC_NORMAL_DUTY)
			{
				ac.setDuty[AC_ICON_POWER] = AC_LOWLIGHT_DUTY;
				ac.adjustSpeed = 10;
			}
		}
	}
	//设置关闭的模块占空比为零
	for(int i=0;i<sizeof(ac.setDuty);i++)
		if(ac.sw[i] == RESET)ac.setDuty[i] = 0;
	//刷新亮度
	for(int i=0;i<sizeof(ac.setDuty)-1;i++)
		acIconShow((ac_icon)i,ac.curDuty[i]);
	acValueShow(ac.showValue,ac.curDuty[sizeof(ac.setDuty)-1]);
}

u8 acLedShowCurDutyGet(ac_icon icon)
{
	return ac.curDuty[icon];
}

FlagStatus acLedsShowUpdateDoneGet (ac_icon icon)
{
	return (ac.setDuty[icon] == ac.curDuty[icon]) ? SET:RESET;
}

FlagStatus acLedShowAllUpdateDoneGet(void)
{
	int i=0;
	for(i=0;i<sizeof(ac.curDuty);i++)
		if(ac.curDuty[i] != ac.setDuty[i])break;
	return (i == sizeof(ac.curDuty)) ? SET:RESET;
}

void acLedShowIconSet(ac_icon icon,FlagStatus highlight,FlagStatus sw,FlagStatus updateNow)
{
	ac.sw[icon] = sw;
	if(highlight)
	{
		ac.highlightTime = AC_HIGHLIGHT_TIME;
		ac.pitchOn = icon;
	}
	else if(updateNow)
	{
		if(ac.sw[icon])
			ac.curDuty[icon] = ac.setDuty[icon] = AC_NORMAL_DUTY;
		else
			ac.curDuty[icon] = ac.setDuty[icon] = 0;
	}
}

FlagStatus acLedShowIconGet(ac_icon icon)
{
	return (FlagStatus)ac.sw[icon];
}

void acLedShowValueSet(u8 value,FlagStatus highlight,FlagStatus sw)
{
	if(value != 0xFF)
		ac.showValue = value;
	ac.sw[sizeof(ac.sw)-1] = sw;
	if(highlight)
	{
		ac.highlightTime = AC_HIGHLIGHT_TIME;
		ac.pitchOn = sizeof(ac.sw)-1;
	}
}

FlagStatus acLedShowValueGetSw(void)
{
	return (FlagStatus)ac.sw[16];
}

static void windSpeedSet(u8 speed,u8 luminance)
{
	 if(speed < 1)speed = 1;
	if(speed > 4)speed = 4;
	switch(speed)
	{
		case 1:
			//1
			for(int i=0;i<4;i++)
				ISSetLedDuty(i+6,8,luminance);
			//2
			for(int i=0;i<2;i++)
				ISSetLedDuty(i+10,8,0);
			for(int i=0;i<2;i++)
				ISSetLedDuty(i+0,7,0);
			//3
			for(int i=0;i<4;i++)
				ISSetLedDuty(i+5,6,0);
			//4
			for(int i=0;i<3;i++)
				ISSetLedDuty(i+9,6,0);
				ISSetLedDuty(0,5,0);
		break;
		
		case 2:
			//1
			for(int i=0;i<4;i++)
				ISSetLedDuty(i+6,8,0);
			//2
			for(int i=0;i<2;i++)
				ISSetLedDuty(i+10,8,luminance);
			for(int i=0;i<2;i++)
				ISSetLedDuty(i+0,7,luminance);
			//3
			for(int i=0;i<4;i++)
				ISSetLedDuty(i+5,6,0);
			//4
			for(int i=0;i<3;i++)
				ISSetLedDuty(i+9,6,0);
				ISSetLedDuty(0,5,0);
		break;
		
		case 3:
			//1
			for(int i=0;i<4;i++)
				ISSetLedDuty(i+6,8,0);
			//2
			for(int i=0;i<2;i++)
				ISSetLedDuty(i+10,8,0);
			for(int i=0;i<2;i++)
				ISSetLedDuty(i+0,7,0);
			//3
			for(int i=0;i<4;i++)
				ISSetLedDuty(i+5,6,luminance);
			//4
			for(int i=0;i<3;i++)
				ISSetLedDuty(i+9,6,0);
				ISSetLedDuty(0,5,0);
		break;
		
		case 4:
			//1
			for(int i=0;i<4;i++)
				ISSetLedDuty(i+6,8,0);
			//2
			for(int i=0;i<2;i++)
				ISSetLedDuty(i+10,8,0);
			for(int i=0;i<2;i++)
				ISSetLedDuty(i+0,7,0);
			//3
			for(int i=0;i<4;i++)
				ISSetLedDuty(i+5,6,0);
			//4
			for(int i=0;i<3;i++)
				ISSetLedDuty(i+9,6,luminance);
				ISSetLedDuty(0,5,luminance);
		break;
	}
}

static void acIconShow(ac_icon icon,u8 luminance)
{
	switch((u8)icon)
	{
		case AC_ICON_WINDS:
			windSpeedSet(fun.ac.winds,luminance);
		break;
		
		case AC_ICON_TEMP:
			ISSetLedDuty(1,2,luminance);
			ISSetLedDuty(2,2,luminance);
		break;
		
		case AC_ICON_WIFI:
			ISSetLedDuty(7,3,luminance);
			ISSetLedDuty(8,3,luminance);
		break;
		
		case AC_ICON_POWER:
			for(int i=0;i<4;i++)
				ISSetLedDuty(3+i,3,luminance);
		break;
		
		case AC_ICON_MIN:
			ISSetLedDuty(3,2,luminance);
			ISSetLedDuty(4,2,luminance);
		break;
		
		case AC_ICON_HOR:
			ISSetLedDuty(5,2,luminance);
			ISSetLedDuty(6,2,luminance);
		break;
		
		case AC_ICON_TIME:
			for(int i=0;i<6;i++)
				ISSetLedDuty(2+i,7,luminance);
		break;
		
		case AC_ICON_AREF:
			for(int i=0;i<6;i++)
				ISSetLedDuty(1+i,5,luminance);
		break;
		
		case AC_ICON_AUTO:
			for(int i=0;i<4;i++)
				ISSetLedDuty(6+i,9,luminance);
		break;
		
		case AC_ICON_COLD:
			for(int i=0;i<2;i++)
				ISSetLedDuty(10+i,9,luminance);
			for(int i=0;i<2;i++)
				ISSetLedDuty(0+i,8,luminance);
		break;
		
		case AC_ICON_CWIND:
			for(int i=0;i<4;i++)
				ISSetLedDuty(2+i,8,luminance);
		break;
		
		case AC_ICON_HEAT:
			for(int i=0;i<4;i++)
				ISSetLedDuty(2+i,9,luminance);
		break;
		
		case AC_ICON_SLEEP:
			ISSetLedDuty(11,11,luminance);
			ISSetLedDuty(11,10,luminance);
			ISSetLedDuty(0,9,luminance);
			ISSetLedDuty(1,9,luminance);
		break;
		
		case AC_ICON_WIND:
			ISSetLedDuty(9,3,luminance);
			ISSetLedDuty(10,3,luminance);
		break;
		
		case AC_ICON_FCOLD://速冷
			ISSetLedDuty(11,3,luminance);
			ISSetLedDuty(0,2,luminance);
		break;
		
		case AC_ICON_LABELSTE://设定温度
			for(int i=0;i<3;i++)
				ISSetLedDuty(8+i,7,luminance);
		break;
		
		case AC_ICON_LABELCTE://当前温度
			ISSetLedDuty(11,7,luminance);
			ISSetLedDuty(0,6,luminance);
			ISSetLedDuty(1,6,luminance);
		break;
		
		case AC_ICON_LABELSTI://设定时间
			for(int i=0;i<3;i++)
				ISSetLedDuty(2+i,6,luminance);
		break;
	}
}

//显示值
static void acValueShow(u8 value,u8 luminance)
{
	if(value >= 99)value %= 100;
	if(value / 10)
		showNum(0,value/10,luminance);
	else
		showNum(0,10,0);
	showNum(1,value%10,luminance);
}

static void showNum (u8 index,u8 num,u8 luminance)
{
	if(index == 0)
	{
		ISSetLedDuty(0,11,(nlList[num] & 0x80) ? luminance:0);//a
		ISSetLedDuty(1,11,(nlList[num] & 0x40) ? luminance:0);//b
		ISSetLedDuty(2,11,(nlList[num] & 0x20) ? luminance:0);//c
		ISSetLedDuty(3,11,(nlList[num] & 0x10) ? luminance:0);//d
		ISSetLedDuty(4,11,(nlList[num] & 0x08) ? luminance:0);//e
		ISSetLedDuty(5,11,(nlList[num] & 0x04) ? luminance:0);//f
		ISSetLedDuty(6,11,(nlList[num] & 0x02) ? luminance:0);//g
		
		ISSetLedDuty(7,11,(nlList[num] & 0x40) ? luminance:0);//b'
		ISSetLedDuty(8,11,(nlList[num] & 0x20) ? luminance:0);//c'
		ISSetLedDuty(9,11,(nlList[num] & 0x08) ? luminance:0);//e'
		ISSetLedDuty(10,11,(nlList[num] & 0x04) ? luminance:0);//f'
	}
	else
	{
		ISSetLedDuty(0,10,(nlList[num] & 0x80) ? luminance:0);
		ISSetLedDuty(1,10,(nlList[num] & 0x40) ? luminance:0);
		ISSetLedDuty(2,10,(nlList[num] & 0x20) ? luminance:0);
		ISSetLedDuty(3,10,(nlList[num] & 0x10) ? luminance:0);
		ISSetLedDuty(4,10,(nlList[num] & 0x08) ? luminance:0);
		ISSetLedDuty(5,10,(nlList[num] & 0x04) ? luminance:0);
		ISSetLedDuty(6,10,(nlList[num] & 0x02) ? luminance:0);
		ISSetLedDuty(7,10,(nlList[num] & 0x01) ? luminance:0);
		
		ISSetLedDuty(7,10,(nlList[num] & 0x40) ? luminance:0);//b'
		ISSetLedDuty(8,10,(nlList[num] & 0x20) ? luminance:0);//c'
		ISSetLedDuty(9,10,(nlList[num] & 0x08) ? luminance:0);//e'
		ISSetLedDuty(10,10,(nlList[num] & 0x04) ? luminance:0);//f'
	}
}

static void timeCallBack (void)
{
	static u8 c = 0;
	
	if(ac.highlightTime)
		ac.highlightTime--;
	
	
	if(c!=0){c --;return ;}
	c = ac.adjustSpeed;
	for(int i=0;i<sizeof(ac.curDuty);i++)
	{
		if(ac.curDuty[i] != ac.setDuty[i])
		{
			if(ac.curDuty[i] < ac.setDuty[i])
				ac.curDuty[i] ++;
			else if(ac.curDuty[i] > ac.setDuty[i])
				ac.curDuty[i] --;
		}
	}
	
}

#include "fun.h"
#include "hal_key.h"
#include "hal_time.h"
#include "hal_buzzer.h"
#include "string.h"
#include "ui.h"
#include "delay.h"
#include "oled.h"
#include "image.h"
#include "rotarySwitch.h"
#include "airConditioner.h"
#include "IS31FL3737.h"

fun_t fun;

static void ledShowManage (void);
static void oledShowManage (void);
static void keyCtrlManage (void);
static void timeCallBack (void);
static void simpleShowModelHandle (u16 evt,u8 model,u8 sw);
static void simpleModeShowModelHandle (u16 evt,u8 model,u8 sw);
static void valueShowModelHandle (void);
static void shiftOutAllShowModel (void);
static void windsShowHandle(void);

void funInit(void)
{
	fun.ac.sta = AC_STA_WIFI|AC_STA_SWIND|AC_STA_AREFACTION|AC_STA_TIME;
	fun.ac.setTemp = 25;//默认设定温度
	fun.ac.curTemp = 26;//当前温度
	fun.ac.showSetTemp.type = AC_VALUE_TYPE_SET_TEMP;
	fun.ac.showSetTime.type = AC_VALUE_TYPE_SET_TIME;
	fun.ac.showCurTemp.type = AC_VALUE_TYPE_CUR_TEMP;
	fun.ac.mode = AC_MODE_COLD;//制冷模式
	fun.ac.setTempShowTime = AC_SET_TEMP_SHOW_TIME;//设置显示设定温度的时间
	fun.ac.winds = 5;
	fun.ac.setTime = 30;
	
	HalTimeRegisteringCallbacks(timeCallBack,10);
	
	oledShow16GrayImage(40,0,212,64,(u8 *)gImage_welcome);
	oledUpdateBuffToShow();
	fun.ac.setTempShowTime = 3000;
	while(fun.ac.setTempShowTime);
}

void funTask(void)
{
	ledShowManage();
	oledShowManage();
	keyCtrlManage();
}

static void keyCtrlManage (void)
{
	static u8 loosenCnt = 0;
	if(fun.ac.setTimeShowTime && fun.ac.sta & AC_STA_POWER)
	{
		if(rotarySwGet(ROTARYSW_CW))
		{
			if(fun.ac.setTime == 30)
				funAcSetTime(60);
			else if(fun.ac.setTime < 1440)
				funAcSetTime(fun.ac.setTime+60);
		}
		if(rotarySwGet(ROTARYSW_CCW))
		{
			if(fun.ac.setTime > 60)
				funAcSetTime(fun.ac.setTime-60);
			else
				funAcSetTime(30);
		}
	}
	if(HalKeyGetShortPress(HAL_KEY_2) && fun.ac.sta & AC_STA_POWER)//降低风速
	{
		funAcSetWinds(0);
	}
	if(HalKeyGetShortPress(HAL_KEY_1) && fun.ac.sta & AC_STA_POWER)//加大风速
	{
		funAcSetWinds(0xFF);
	}
	if(fun.ac.setTimeShowTime == 0 && fun.ac.sta & AC_STA_POWER)
	{
		if(rotarySwGet(ROTARYSW_CW))//上调温度
			funAcSetTemp(0xFF);
	}
	if(fun.ac.setTimeShowTime == 0 && fun.ac.sta & AC_STA_POWER)
	{
		if(rotarySwGet(ROTARYSW_CCW))//下调温度
			funAcSetTemp(0x00);
	}
	if(HalKeyGetShortPress(HAL_KEY_ROTARY) && fun.ac.sta & AC_STA_POWER)//模式的切换
	{
		if(fun.ac.mode != AC_MODE_AUTO)
			fun.ac.mode ++;
		else
			fun.ac.mode = AC_MODE_SLEEP;
		fun.ac.sta |= AC_STA_MSU;//模式的更新
		HalBuzzSetPeriod(AC_BUZZER_PERIOD);
	}
	if(HalKeyGetShortPress(HAL_KEY_4) && fun.ac.sta & AC_STA_POWER)//开关扫风
	{
		if(!(fun.ac.sta & AC_STA_SWIND))
			funAcSWind(SET);
		else
			funAcSWind(RESET);
	}
	if(HalKeyGetShortPress(HAL_KEY_3) && fun.ac.sta & AC_STA_POWER)//除湿开关
	{
		if(loosenCnt == 0)
		{
			if(!(fun.ac.sta & AC_STA_AREFACTION))
				fun.ac.sta |= AC_STA_AREFACTION|AC_STA_ASU;
			else
			{
				fun.ac.sta &= ~AC_STA_AREFACTION;
				fun.ac.sta |= AC_STA_ASU;
			}
			HalBuzzSetPeriod(AC_BUZZER_PERIOD);
		}
		loosenCnt = 0xFF;
	}
	if(HalKeyGetShortPress(HAL_KEY_5) && fun.ac.sta & AC_STA_POWER)//定时
	{
		if(fun.ac.sta & AC_STA_TIME)
			funAcSetTime(0);//关闭定时
		else
			funAcSetTime(fun.ac.setTime);//开启定时
	}
	if(HalKeyGetShortPress(HAL_KEY_6))//电源开关
	{
		if(!(fun.ac.sta & AC_STA_POWER))
		{
			funAcPowerOn();
		}
		else
		{
			funAcPowerOff();
		}
	}
	if(loosenCnt)
		loosenCnt--;
}

//打开电源
void funAcPowerOn(void)
{
	if(!(fun.ac.sta & AC_STA_POWER))
	{
		fun.ac.sta |= AC_STA_POWER|(AC_STA_WIFI|AC_STA_ISU)|(AC_STA_TSC);//打开电源
		fun.ac.setTempShowTime = AC_SET_TEMP_SHOW_TIME;//刷新设定温度的显示时间
		fun.ac.fcoldTime = AC_PON_FAST_COLD_TIME;//开机速冷时间
		HalBuzzSetPeriod(AC_BUZZER_PERIOD);
	}
}

//关闭电源
void funAcPowerOff(void)
{
	fun.ac.sta &= ~AC_STA_POWER;
	HalBuzzSetPeriod(AC_BUZZER_PERIOD);
}

//除湿开关
void funAcArefaction(FlagStatus sw)
{
	funAcPowerOn();
	if(sw == SET)
	{
		if(!(fun.ac.sta & AC_STA_AREFACTION))
			fun.ac.sta |= AC_STA_AREFACTION|AC_STA_ASU;
	}
	else
	{
		if(fun.ac.sta & AC_STA_AREFACTION)
		{
			fun.ac.sta &= ~AC_STA_AREFACTION;
			fun.ac.sta |= AC_STA_ASU;
		}
	}
	HalBuzzSetPeriod(AC_BUZZER_PERIOD);
}

//设置扫风开关
void funAcSWind(FlagStatus sw)
{
	funAcPowerOn();
	if(sw == SET)
	{
		if(!(fun.ac.sta & AC_STA_SWIND))
			fun.ac.sta |= AC_STA_SWIND|AC_STA_WSU;
	}
	else
	{
		if(fun.ac.sta & AC_STA_SWIND)
		{
			fun.ac.sta &= ~AC_STA_SWIND;
			fun.ac.sta |= AC_STA_WSU;//模式的更新
		}
	}
	HalBuzzSetPeriod(AC_BUZZER_PERIOD);
}

//设置模式
void funAcSetMode(ac_mode_t mode)
{
	funAcPowerOn();
	if(fun.ac.mode != mode)
	{
		fun.ac.mode = mode;
		fun.ac.sta |= AC_STA_MSU;//模式的更新
	}
	HalBuzzSetPeriod(AC_BUZZER_PERIOD);
}

//设置定时
void funAcSetTime(u16 time)
{
	funAcPowerOn();
	if(time)
	{
		if(fun.ac.setTimeShowTime == 0)
			fun.ac.sta |= AC_STA_TVSU;
		fun.ac.setTimeShowTime = AC_SET_TIME_SHOW_TIME;
		if(!(fun.ac.sta & AC_STA_TIME))//如果是关闭状态
		{
			fun.ac.sta |= AC_STA_TIME|AC_STA_TSU|AC_STA_TVSU;
		}
		fun.ac.setTime = time;
		fun.ac.setTempShowTime = 0;
	}
	else
	{
		if(fun.ac.sta & AC_STA_TIME)
		{
			fun.ac.sta &= ~AC_STA_TIME;
			fun.ac.sta |= AC_STA_TSU|AC_STA_TVSU;
			fun.ac.setTimeShowTime = 0;
		}
	}
	HalBuzzSetPeriod(AC_BUZZER_PERIOD);
}

//设置温度
ErrorStatus funAcSetTemp(u8 temp)
{
	ErrorStatus ret = SUCCESS;
	funAcPowerOn();
	if(temp == 0xFF)
	{
		if(fun.ac.setTemp < AC_HIGH_TEMP)
			fun.ac.setTemp ++;
		else
			ret = ERROR;
	}
	else if(temp == 0)
	{
		if(fun.ac.setTemp > AC_LOW_TEMP)
			fun.ac.setTemp --;
		else
			ret = ERROR;
	}
	else
	{
		if(temp <= AC_HIGH_TEMP && temp >= AC_LOW_TEMP)
			fun.ac.setTemp = temp;
		else
			ret = ERROR;
	}
	fun.ac.setTemp = MIN_VALUE(fun.ac.setTemp,AC_HIGH_TEMP);
	fun.ac.setTemp = MAX_VALUE(fun.ac.setTemp,AC_LOW_TEMP);
	if(fun.ac.setTempShowTime == 0)
		fun.ac.sta |= AC_STA_TSC;
	if(fun.ac.setTimeShowTime)
	{
		fun.ac.sta |= AC_STA_TSC;
		fun.ac.setTimeShowTime = 0;
	}
	fun.ac.setTempShowTime = AC_SET_TEMP_SHOW_TIME;//刷新设定温度的显示时间
	HalBuzzSetPeriod(AC_BUZZER_PERIOD);
	return ret;
}

//设置风速
void funAcSetWinds(u8 winds)
{
	funAcPowerOn();
	if(winds == 0xFF)
	{
		if(fun.ac.winds < 5)
			fun.ac.winds ++;
	}
	else if(winds == 0)
	{
		if(fun.ac.winds > 1)
			fun.ac.winds --;
	}
	else
		fun.ac.winds = winds;
	fun.ac.sta |= AC_STA_WSSC;
	HalBuzzSetPeriod(AC_BUZZER_PERIOD);
}

static void ledShowManage (void)
{
	if(fun.ac.sta & AC_STA_POWER)//如果电源被打开
	{
		if(acLedShowIconGet(AC_ICON_POWER) == SET)
			acLedShowIconSet(AC_ICON_POWER,RESET,RESET,RESET);
		if(acLedShowCurDutyGet(AC_ICON_POWER) == 0)
		{
			if(fun.ac.setTimeShowTime)//显示设定时间
			{
				acLedShowIconSet(AC_ICON_TEMP,RESET,RESET,SET);
				if(fun.ac.setTime >= 60)
				{
					acLedShowIconSet(AC_ICON_HOR,RESET,SET,SET);
					acLedShowIconSet(AC_ICON_MIN,RESET,RESET,SET);
					acLedShowValueSet(fun.ac.setTime/60,(fun.ac.sta & AC_STA_TSU) ? SET:RESET,SET);
				}
				else
				{
					acLedShowIconSet(AC_ICON_HOR,RESET,RESET,SET);
					acLedShowIconSet(AC_ICON_MIN,RESET,SET,SET);
					acLedShowValueSet(fun.ac.setTime,(fun.ac.sta & AC_STA_TSU) ? SET:RESET,SET);
				}
			}
			else if(fun.ac.setTempShowTime)//显示设定温度
			{
				acLedShowIconSet(AC_ICON_TEMP,RESET,SET,SET);
				acLedShowIconSet(AC_ICON_HOR,RESET,RESET,SET);
				acLedShowIconSet(AC_ICON_MIN,RESET,RESET,SET);
				acLedShowValueSet(fun.ac.setTemp,(fun.ac.sta & AC_STA_TSU) ? SET:RESET,SET);
			}
			else//显示当前温度
			{
				acLedShowIconSet(AC_ICON_TEMP,RESET,SET,SET);
				acLedShowIconSet(AC_ICON_HOR,RESET,RESET,SET);
				acLedShowIconSet(AC_ICON_MIN,RESET,RESET,SET);
				acLedShowValueSet(fun.ac.curTemp,(fun.ac.sta & AC_STA_TSU) ? SET:RESET,SET);
			}
			acLedShowIconSet(AC_ICON_WINDS,RESET,SET,RESET);
			if(fun.ac.setTimeShowTime)//如果在设置定时时间
			{
				acLedShowIconSet(AC_ICON_LABELSTI,RESET,SET,SET);
				acLedShowIconSet(AC_ICON_LABELSTE,RESET,RESET,SET);
				acLedShowIconSet(AC_ICON_LABELCTE,RESET,RESET,SET);
			}
			else if(fun.ac.setTempShowTime)//如果再设定温度
			{
				acLedShowIconSet(AC_ICON_LABELSTI,RESET,RESET,SET);
				acLedShowIconSet(AC_ICON_LABELSTE,RESET,SET,SET);
				acLedShowIconSet(AC_ICON_LABELCTE,RESET,RESET,SET);
			}
			else
			{
				acLedShowIconSet(AC_ICON_LABELSTI,RESET,RESET,SET);
				acLedShowIconSet(AC_ICON_LABELSTE,RESET,RESET,SET);
				acLedShowIconSet(AC_ICON_LABELCTE,RESET,SET,SET);
			}
			if(fun.ac.fcoldTime)
				acLedShowIconSet(AC_ICON_FCOLD,RESET,SET,RESET);
			else
				acLedShowIconSet(AC_ICON_FCOLD,RESET,RESET,RESET);
			if(fun.ac.sta & AC_STA_SWIND)
				acLedShowIconSet(AC_ICON_WIND,RESET,SET,RESET);
			else
				acLedShowIconSet(AC_ICON_WIND,RESET,RESET,RESET);
			if(fun.ac.sta & AC_STA_WIFI)
				acLedShowIconSet(AC_ICON_WIFI,RESET,SET,RESET);
			else
				acLedShowIconSet(AC_ICON_WIFI,RESET,RESET,RESET);
			if(fun.ac.sta & AC_STA_AREFACTION)
				acLedShowIconSet(AC_ICON_AREF,RESET,SET,RESET);
			else
				acLedShowIconSet(AC_ICON_AREF,RESET,RESET,RESET);
			if(fun.ac.sta & AC_STA_TIME)
				acLedShowIconSet(AC_ICON_TIME,RESET,SET,RESET);
			else
				acLedShowIconSet(AC_ICON_TIME,RESET,RESET,RESET);
			//模式的显示处理
			if(acLedsShowUpdateDoneGet(AC_ICON_SLEEP) && 
				 acLedsShowUpdateDoneGet(AC_ICON_CWIND) && 
				 acLedsShowUpdateDoneGet(AC_ICON_COLD) && 
				 acLedsShowUpdateDoneGet(AC_ICON_HEAT) && 
				 acLedsShowUpdateDoneGet(AC_ICON_AUTO))
			{
				switch((u8)fun.ac.mode)
				{
					case AC_MODE_SLEEP:
						acLedShowIconSet(AC_ICON_SLEEP,RESET,SET,RESET);
						acLedShowIconSet(AC_ICON_CWIND,RESET,RESET,RESET);
						acLedShowIconSet(AC_ICON_COLD,RESET,RESET,RESET);
						acLedShowIconSet(AC_ICON_HEAT,RESET,RESET,RESET);
						acLedShowIconSet(AC_ICON_AUTO,RESET,RESET,RESET);
					break;
					
					case AC_MODE_WIND:
						acLedShowIconSet(AC_ICON_SLEEP,RESET,RESET,RESET);
						acLedShowIconSet(AC_ICON_CWIND,RESET,SET,RESET);
						acLedShowIconSet(AC_ICON_COLD,RESET,RESET,RESET);
						acLedShowIconSet(AC_ICON_HEAT,RESET,RESET,RESET);
						acLedShowIconSet(AC_ICON_AUTO,RESET,RESET,RESET);
					break;
					
					case AC_MODE_COLD:
						acLedShowIconSet(AC_ICON_SLEEP,RESET,RESET,RESET);
						acLedShowIconSet(AC_ICON_CWIND,RESET,RESET,RESET);
						acLedShowIconSet(AC_ICON_COLD,RESET,SET,RESET);
						acLedShowIconSet(AC_ICON_HEAT,RESET,RESET,RESET);
						acLedShowIconSet(AC_ICON_AUTO,RESET,RESET,RESET);
					break;
					
					case AC_MODE_HEAT:
						acLedShowIconSet(AC_ICON_SLEEP,RESET,RESET,RESET);
						acLedShowIconSet(AC_ICON_CWIND,RESET,RESET,RESET);
						acLedShowIconSet(AC_ICON_COLD,RESET,RESET,RESET);
						acLedShowIconSet(AC_ICON_HEAT,RESET,SET,RESET);
						acLedShowIconSet(AC_ICON_AUTO,RESET,RESET,RESET);
					break;
					
					case AC_MODE_AUTO:
						acLedShowIconSet(AC_ICON_SLEEP,RESET,RESET,RESET);
						acLedShowIconSet(AC_ICON_CWIND,RESET,RESET,RESET);
						acLedShowIconSet(AC_ICON_COLD,RESET,RESET,RESET);
						acLedShowIconSet(AC_ICON_HEAT,RESET,RESET,RESET);
						acLedShowIconSet(AC_ICON_AUTO,RESET,SET,RESET);
					break;
				}
			}
		}
	}
	else
	{
		if(acLedShowCurDutyGet(AC_ICON_VALUE) == 0)
		{
			if(acLedShowIconGet(AC_ICON_POWER) != SET)
				acLedShowIconSet(AC_ICON_POWER,SET,SET,RESET);
		}
		acLedShowIconSet(AC_ICON_SLEEP,RESET,RESET,RESET);
		acLedShowIconSet(AC_ICON_CWIND,RESET,RESET,RESET);
		acLedShowIconSet(AC_ICON_COLD,RESET,RESET,RESET);
		acLedShowIconSet(AC_ICON_HEAT,RESET,RESET,RESET);
		acLedShowIconSet(AC_ICON_AUTO,RESET,RESET,RESET);
		acLedShowIconSet(AC_ICON_TEMP,RESET,RESET,RESET);
		acLedShowIconSet(AC_ICON_TIME,RESET,RESET,RESET);
		acLedShowIconSet(AC_ICON_AREF,RESET,RESET,RESET);
		acLedShowIconSet(AC_ICON_WIND,RESET,RESET,RESET);
		acLedShowIconSet(AC_ICON_WIFI,RESET,RESET,RESET);
		acLedShowIconSet(AC_ICON_WINDS,RESET,RESET,RESET);
		acLedShowIconSet(AC_ICON_FCOLD,RESET,RESET,RESET);
		acLedShowIconSet(AC_ICON_LABELSTI,RESET,RESET,RESET);
		acLedShowIconSet(AC_ICON_LABELSTE,RESET,RESET,RESET);
		acLedShowIconSet(AC_ICON_LABELCTE,RESET,RESET,RESET);
		acLedShowValueSet(0xFF,RESET,RESET);
	}
}

static void oledShowManage (void)
{
	if(fun.ac.sta & AC_STA_POWER)//如果电源被打开
	{
		if(uiModelGetLastSta(UI_MODEL_POWER) == UI_MODEL_STA_SHIFT_OUT_DONE)//如果电源显示被移出
		{
			windsShowHandle();//风速显示模块处理
			valueShowModelHandle();//值显示模块处理
			simpleModeShowModelHandle(AC_STA_MSU,UI_MODEL_MODE,1);//模式显示模块的处理
			simpleShowModelHandle(AC_STA_WSU,UI_MODEL_SWIND,(fun.ac.sta & AC_STA_SWIND) ? 1:0);//扫风显示模块的处理
			simpleShowModelHandle(AC_STA_ASU,UI_MODEL_AREFACTION,(fun.ac.sta & AC_STA_AREFACTION) ? 1:0);//除湿显示模块的处理
			simpleShowModelHandle(AC_STA_TSU,UI_MODEL_TIME,(fun.ac.sta & AC_STA_TIME) ? 1:0);//定时显示模块的处理
			simpleShowModelHandle(AC_STA_ISU,UI_MODEL_WIFI,(fun.ac.sta & AC_STA_WIFI) ? 1:0);//WiFi显示模块的处理
		}
		else if(uiModelGetLastSta(UI_MODEL_POWER) != UI_MODEL_STA_SHIFT_OUT_ING)//如果电源显示模块未执行移出动作
		{
			uiModelSetThisSta(UI_MODEL_POWER,UI_MODEL_STA_SHIFT_OUT_ING);//将电源图标移出
		}
	}
	else//电源被关闭
	{
		//==========================================电源显示模块的处理==========================================
		if(uiModelGetLastSta(UI_MODEL_POWER) != UI_MODEL_STA_SHIFT_IN_DONE && uiModelGetLastSta(UI_MODEL_POWER) != UI_MODEL_STA_SHIFT_IN_ING)//如果还没开始移入电源
		{
			shiftOutAllShowModel();
			if(uiAllModelThisStaDoneGet() && uiModelGetLastSta(UI_MODEL_POWER) != UI_MODEL_STA_SHIFT_IN_DONE)//如果所有显示模块的状态完成 且 电源显示未载入完成
			{
				if(uiModelGetLastSta(UI_MODEL_POWER) != UI_MODEL_STA_SHIFT_IN_ING)//如果没有开始移入
				{
					uiModelSetThisSta(UI_MODEL_POWER,UI_MODEL_STA_SHIFT_IN_ING);//将电源图标移入
				}
			}
		}
	}
}

//移出所有未移出的模块
static void shiftOutAllShowModel (void)
{
	for(int i=0;i<UI_MODEL_NUM;i++)
	{
		if(uiAllModelThisStaDoneGet() && uiAllModelLastStaDoneGet())//如果模块移动完成
		{
			ui_model_sta_t sta = uiModelGetLastSta(i);
			if(sta != UI_MODEL_STA_SHIFT_OUT_ING && sta != UI_MODEL_STA_SHIFT_OUT_DONE && uiModelLastStaDoneGet(i) && sta != UI_MODEL_STA_INIT_DONE)//如果该模块未移出且未完成移出且完成状态
				uiModelSetThisSta(i,UI_MODEL_STA_SHIFT_OUT_ING);//执行移出
		}
	}
}

//风速显示处理
static void windsShowHandle(void)
{
	if(uiModelGetThisSta(UI_MODEL_WINDS) != UI_MODEL_STA_SHIFT_IN_DONE && uiModelGetThisSta(UI_MODEL_WINDS) != UI_MODEL_STA_SHIFT_IN_ING)//如果没有移入完成且未开始移入
	{
		uiModelSetThisSta(UI_MODEL_WINDS,UI_MODEL_STA_SHIFT_IN_ING);//移入模块
	}
	else if(uiModelGetThisSta(UI_MODEL_WINDS) == UI_MODEL_STA_SHIFT_IN_DONE)//如果移入完成
	{
		if(fun.ac.sta & AC_STA_WSSC)//如果更新
		{
			if(uiModelThisStaDoneGet(UI_MODEL_WINDS))//如果完成动作
			{
				uiModelSetThisSta(UI_MODEL_WINDS,UI_MODEL_STA_UPDATE_ING);//更新模块
				fun.ac.sta &= ~AC_STA_WSSC;
			}
		}
	}
}

//值显示模块处理
static void valueShowModelHandle (void)
{
	if(!uiAllModelThisStaDoneGet() || !uiAllModelLastStaDoneGet())return ;//如果有一个未完成退出不动作(节省内存分配需要一个个图标来动)
	if(fun.ac.sta & (AC_STA_TSC|AC_STA_TVSU))//如果温度状态改变/定时时间更新
	{
		//if(uiModelGetThisSta(UI_MODEL_VALUE) == UI_MODEL_STA_SHIFT_IN_DONE)//如果移入完成的状态
		//	uiModelSetThisSta(UI_MODEL_VALUE,UI_MODEL_STA_SHIFT_OUT_ING);//移出
		
		if(uiModelGetThisSta(UI_MODEL_LABEL) == UI_MODEL_STA_SHIFT_IN_DONE)//如果移入完成的状态
			uiModelSetThisSta(UI_MODEL_LABEL,UI_MODEL_STA_SHIFT_OUT_ING);//将标签移出
		fun.ac.sta &= ~(AC_STA_TSC);//清除状态改变
	}
	if(uiModelGetThisSta(UI_MODEL_LABEL) == UI_MODEL_STA_SHIFT_OUT_DONE || uiModelGetThisSta(UI_MODEL_LABEL) == UI_MODEL_STA_INIT_DONE)//如果显示模块被移出
	{
		if(uiModelLastStaDoneGet(UI_MODEL_LABEL))//如果完成动作
		{
			if(fun.ac.setTimeShowTime)//如果在设置定时时间
				uiModelLabelSet(UI_MODEL_LABEL,AC_LABEL_TIME);
			else if(fun.ac.setTempShowTime)//如果设定温度
				uiModelLabelSet(UI_MODEL_LABEL,AC_LABEL_SET_TEMP);
			else
				uiModelLabelSet(UI_MODEL_LABEL,AC_LABEL_CUR_TEMP);
			uiModelSetThisSta(UI_MODEL_LABEL,UI_MODEL_STA_SHIFT_IN_ING);//将显示模块移入
		}
	}
	if(uiModelGetThisSta(UI_MODEL_VALUE) == UI_MODEL_STA_SHIFT_OUT_DONE || uiModelGetThisSta(UI_MODEL_VALUE) == UI_MODEL_STA_INIT_DONE)//如果值的显示还是移出状态 或者 刚开机未移入的状态
	{
		if(uiModelLastStaDoneGet(UI_MODEL_VALUE))//如果完成动作
		{
			fun.ac.showSetTime.value = fun.ac.setTime;
			fun.ac.showSetTemp.value = fun.ac.setTemp;
			fun.ac.showCurTemp.value = fun.ac.curTemp;
			if(fun.ac.setTimeShowTime)//如果在设置定时时间
				uiModelParamSet(UI_MODEL_VALUE,&fun.ac.showSetTime);//显示定时时间
			if(fun.ac.setTempShowTime)//如果在设定温度
				uiModelParamSet(UI_MODEL_VALUE,&fun.ac.showSetTemp);//显示设定温度
			else
				uiModelParamSet(UI_MODEL_VALUE,&fun.ac.showCurTemp);//显示当前温度
			uiModelSetThisSta(UI_MODEL_VALUE,UI_MODEL_STA_SHIFT_IN_ING);//将显示模块移入
		}
	}
	if(uiModelGetThisSta(UI_MODEL_VALUE) == UI_MODEL_STA_UPDATE_DONE || uiModelGetThisSta(UI_MODEL_VALUE) == UI_MODEL_STA_SHIFT_IN_DONE)//如果更新完成或者移入完成
	{
		if(fun.ac.setTimeShowTime)//如果显示定时时间
		{
			if(fun.ac.setTime != fun.ac.showSetTime.value || (fun.ac.sta & AC_STA_TVSU))
			{
				fun.ac.showSetTime.value = fun.ac.setTime;
				uiModelParamSet(UI_MODEL_VALUE,&fun.ac.showSetTime);//显示定时时间
				uiModelSetThisSta(UI_MODEL_VALUE,UI_MODEL_STA_UPDATE_ING);//写入更新显示事件
			}
		}
		else if(fun.ac.setTempShowTime == 0 || (fun.ac.sta & AC_STA_TVSU))//如果不显示设定温度
		{
			if(fun.ac.curTempUpdateTime == 0)//如果更新时间到
			{
				uiModelParamSet(UI_MODEL_VALUE,&fun.ac.showCurTemp);//显示当前温度
				uiModelSetThisSta(UI_MODEL_VALUE,UI_MODEL_STA_UPDATE_ING);//写入更新显示事件
				fun.ac.curTempUpdateTime = AC_CUR_TEMP_UPDATE_TIME;
			}
		}
		else//显示设定温度
		{
			if(fun.ac.showSetTemp.value != fun.ac.setTemp || (fun.ac.sta & AC_STA_TVSU))//如果显示的设定温度有变
			{
				fun.ac.showSetTemp.value = fun.ac.setTemp;
				uiModelParamSet(UI_MODEL_VALUE,&fun.ac.showSetTemp);//显示设定温度
				uiModelSetThisSta(UI_MODEL_VALUE,UI_MODEL_STA_UPDATE_ING);//写入更新显示事件
			}
		}
		fun.ac.sta &= ~(AC_STA_TVSU);//清除状态改变
	}
}

//简单的显示模块处理
static void simpleShowModelHandle (u16 evt,u8 model,u8 sw)
{
	if(!uiAllModelThisStaDoneGet() || !uiAllModelLastStaDoneGet())return ;//如果有一个未完成退出不动作(节省内存分配需要一个个图标来动)
	if(fun.ac.sta & evt)//如果需要更新显示
	{
		if(uiModelLastStaDoneGet(model))//如果上一次动作完成
		{
			uiModelSetThisSta(model,UI_MODEL_STA_SHIFT_OUT_ING);//将显示模块移出
		}
		fun.ac.sta &= ~evt;//清除更新
	}
	if(uiModelGetLastSta(model) != UI_MODEL_STA_SHIFT_IN_DONE)//如果显示未被移入完成
	{
		if(uiModelLastStaDoneGet(model))//如果上一次动作完成
		{
			if(sw)//如果开启显示
				uiModelSetThisSta(model,UI_MODEL_STA_SHIFT_IN_ING);//将显示模块移入
		}
	}
}

//模式的显示模块处理
static void simpleModeShowModelHandle (u16 evt,u8 model,u8 sw)
{
	if(!uiAllModelThisStaDoneGet() || !uiAllModelLastStaDoneGet())return ;//如果有一个未完成退出不动作(节省内存分配需要一个个图标来动)
	if(fun.ac.sta & evt)//如果需要更新显示
	{
		if(uiModelLastStaDoneGet(model))//如果上一次动作完成
		{
			uiModelSetThisSta(model,UI_MODEL_STA_SHIFT_OUT_ING);//将显示模块移出
		}
		fun.ac.sta &= ~evt;//清除更新
	}
	if(uiModelGetLastSta(model) != UI_MODEL_STA_SHIFT_IN_DONE)//如果显示未被移入完成
	{
		if(uiModelLastStaDoneGet(model))//如果上一次动作完成
		{
			fun.ac.showMode = fun.ac.mode;//更换显示模式
			if(sw)//如果开启显示
				uiModelShowDepthSet(model,15,0);//设定显示深度
			else
				uiModelShowDepthSet(model,0,0);//设定显示深度
			uiModelSetThisSta(model,UI_MODEL_STA_SHIFT_IN_ING);//将显示模块移入
		}
	}
}

static void timeCallBack (void)
{
	if(fun.ac.setTempShowTime)
	{
		fun.ac.setTempShowTime--;
		if(fun.ac.setTempShowTime == 0)
			fun.ac.sta |= AC_STA_TSC;
	}
	if(fun.ac.setTimeShowTime)
	{
		fun.ac.setTimeShowTime--;
		if(fun.ac.setTimeShowTime == 0)
			fun.ac.sta |= AC_STA_TSC;
	}
	if(fun.ac.curTempUpdateTime)
		fun.ac.curTempUpdateTime--;
	if(fun.ac.fcoldTime)
		fun.ac.fcoldTime--;
}

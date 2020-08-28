#include "ui.h"
#include "malloc.h"
#include "fun.h"
#include "string.h"
#include "image.h"
#include "oled.h"

#include "hal_time.h"

ui_t ui;

static void uiModelManage (void);
static void getModelInfo (ui_model_info_t *model,u8 index);
static void timeCalBack (void);
static u8 *mallocImage (u8 index,u8 depth,u8 background);

void uiInit (void)
{
	ui.state = 0;
	ui.rate = 0;
	for(int i=0;i<UI_MODEL_NUM;i++)
	{
		ui.model[i].thisSta = ui.model[i].lastSta = UI_MODEL_STA_INIT_DONE;
		ui.model[i].semp.sta = SE_MOVE_STA_DONE;
	}
	
	//设定图像坐标以及宽高
	ui.model[UI_MODEL_POWER].showX = 104;
	ui.model[UI_MODEL_POWER].showY = 15;
	ui.model[UI_MODEL_POWER].showHigh = 48;
	ui.model[UI_MODEL_POWER].showWidth = 48;
	ui.model[UI_MODEL_POWER].semp.inDir = SE_MOVE_DIR_DU;
	ui.model[UI_MODEL_POWER].semp.outDir = SE_MOVE_DIR_UD;
	
	ui.model[UI_MODEL_MODE].showX = 192;
	ui.model[UI_MODEL_MODE].showY = 0;
	ui.model[UI_MODEL_MODE].showHigh = 64;
	ui.model[UI_MODEL_MODE].showWidth = 64;
	ui.model[UI_MODEL_MODE].semp.inDir = SE_MOVE_DIR_RL;
	ui.model[UI_MODEL_MODE].semp.outDir = SE_MOVE_DIR_LR;
	
	ui.model[UI_MODEL_VALUE].showX = 0;
	ui.model[UI_MODEL_VALUE].showY = 0;
	ui.model[UI_MODEL_VALUE].showHigh = 48;
	ui.model[UI_MODEL_VALUE].showWidth = 72;
	ui.model[UI_MODEL_VALUE].semp.inDir = SE_MOVE_DIR_UD;
	ui.model[UI_MODEL_VALUE].semp.outDir = SE_MOVE_DIR_DU;
	
	ui.model[UI_MODEL_SWIND].showX = 80;
	ui.model[UI_MODEL_SWIND].showY = 32;
	ui.model[UI_MODEL_SWIND].showHigh = 32;
	ui.model[UI_MODEL_SWIND].showWidth = 32;
	ui.model[UI_MODEL_SWIND].semp.inDir = SE_MOVE_DIR_DU;
	ui.model[UI_MODEL_SWIND].semp.outDir = SE_MOVE_DIR_UD;
	
	ui.model[UI_MODEL_WINDS].showX = 112;
	ui.model[UI_MODEL_WINDS].showY = 32;
	ui.model[UI_MODEL_WINDS].showHigh = 32;
	ui.model[UI_MODEL_WINDS].showWidth = 64;
	ui.model[UI_MODEL_WINDS].semp.inDir = SE_MOVE_DIR_DU;
	ui.model[UI_MODEL_WINDS].semp.outDir = SE_MOVE_DIR_UD;
	
	ui.model[UI_MODEL_AREFACTION].showX = 144;
	ui.model[UI_MODEL_AREFACTION].showY = 0;
	ui.model[UI_MODEL_AREFACTION].showHigh = 32;
	ui.model[UI_MODEL_AREFACTION].showWidth = 32;
	ui.model[UI_MODEL_AREFACTION].semp.inDir = SE_MOVE_DIR_UD;
	ui.model[UI_MODEL_AREFACTION].semp.outDir = SE_MOVE_DIR_DU;
	
	ui.model[UI_MODEL_TIME].showX = 112;
	ui.model[UI_MODEL_TIME].showY = 0;
	ui.model[UI_MODEL_TIME].showHigh = 32;
	ui.model[UI_MODEL_TIME].showWidth = 32;
	ui.model[UI_MODEL_TIME].semp.inDir = SE_MOVE_DIR_UD;
	ui.model[UI_MODEL_TIME].semp.outDir = SE_MOVE_DIR_DU;
	
	ui.model[UI_MODEL_WIFI].showX = 80;
	ui.model[UI_MODEL_WIFI].showY = 0;
	ui.model[UI_MODEL_WIFI].showHigh = 32;
	ui.model[UI_MODEL_WIFI].showWidth = 32;
	ui.model[UI_MODEL_WIFI].semp.inDir = SE_MOVE_DIR_UD;
	ui.model[UI_MODEL_WIFI].semp.outDir = SE_MOVE_DIR_DU;
	
	ui.model[UI_MODEL_LABEL].showX = 0;
	ui.model[UI_MODEL_LABEL].showY = 48;
	ui.model[UI_MODEL_LABEL].showHigh = 16;
	ui.model[UI_MODEL_LABEL].showWidth = 72;
	ui.model[UI_MODEL_LABEL].semp.inDir = SE_MOVE_DIR_DU;
	ui.model[UI_MODEL_LABEL].semp.outDir = SE_MOVE_DIR_UD;
	
	//设定背景以及图像深度
	for(int i=0;i<UI_MODEL_NUM;i++)
	{
		ui.model[i].depth = 15;
		ui.model[i].background = 0;
	}
	
	//设定回弹效果
	for(int i=0;i<UI_MODEL_NUM;i++)
		ui.model[i].semp.kickBack.on = RESET;
	
	//设定移动速度
	ui.model[UI_MODEL_POWER].curSpeed = ui.model[UI_MODEL_POWER].setSpeed = 1;
	ui.model[UI_MODEL_MODE].curSpeed = ui.model[UI_MODEL_MODE].setSpeed = 0;
	ui.model[UI_MODEL_VALUE].curSpeed = ui.model[UI_MODEL_VALUE].setSpeed = 0;
	ui.model[UI_MODEL_SWIND].curSpeed = ui.model[UI_MODEL_SWIND].setSpeed = 0;
	ui.model[UI_MODEL_AREFACTION].curSpeed = ui.model[UI_MODEL_AREFACTION].setSpeed = 0;
	ui.model[UI_MODEL_TIME].curSpeed = ui.model[UI_MODEL_TIME].setSpeed = 0;
	ui.model[UI_MODEL_WIFI].curSpeed = ui.model[UI_MODEL_WIFI].setSpeed = 0;
	ui.model[UI_MODEL_LABEL].curSpeed = ui.model[UI_MODEL_LABEL].setSpeed = 1;
	
	HalTimeRegisteringCallbacks(timeCalBack,1);
	oledClear();
}

void uiTask (void)
{
	uiModelManage();
}

void uiModelParamSet(u8 index,void *param)
{
	ui.model[index].param = param;
}

void *uiModelParamGet(u8 index)
{
	return ui.model[index].param;
}

void uiModelLabelSet(u8 index,char *label)
{
	ui.model[index].label = label;
}

void uiModelSempDirSet (u8 index,seMoveDir_t inDir,seMoveDir_t outDir)
{
	ui.model[index].semp.inDir = inDir;
	ui.model[index].semp.outDir = outDir;
}

void uiModelShowDepthSet (u8 index,u8 depth,u8 background)
{
	ui.model[index].depth = depth;
	ui.model[index].background = background;
}

FlagStatus uiAllModelThisStaDoneGet (void)
{
	u8 i=0;
	for(i=0;i<UI_MODEL_NUM;i++)
		if(ui.model[i].thisSta % 2)break;
	return (i == UI_MODEL_NUM) ? SET:RESET;
}

FlagStatus uiAllModelLastStaDoneGet (void)
{
	u8 i=0;
	for(i=0;i<UI_MODEL_NUM;i++)
		if(ui.model[i].lastSta % 2)break;
	return (i == UI_MODEL_NUM) ? SET:RESET;
}

FlagStatus uiModelLastStaDoneGet (u8 index)
{
	return (ui.model[index].lastSta % 2) ? RESET:SET;
}

FlagStatus uiModelThisStaDoneGet (u8 index)
{
	return (ui.model[index].thisSta % 2) ? RESET:SET;
}

void uiModelSetThisSta(u8 index,ui_model_sta_t sta)
{
	ui.model[index].thisSta = sta;
}

ui_model_sta_t uiModelGetLastSta (u8 index)
{
	return ui.model[index].lastSta;
}

ui_model_sta_t uiModelGetThisSta (u8 index)
{
	return ui.model[index].thisSta;
}

static void uiModelManage (void)
{
	if(ui.rate == 0)//如果调整速率时间到
	{
		ui.rate = 10;//1ms后再次检查动画
		//============================图像的动作处理============================
		for(int i=0;i<UI_MODEL_NUM;i++)
		{
			if(ui.model[i].thisSta != ui.model[i].lastSta)//如果状态发生改变
			{
				if(uiModelLastStaDoneGet(i))//如果上一次的动作已经完成
				{
					if(ui.model[i].thisSta == UI_MODEL_STA_SHIFT_OUT_ING || ui.model[i].thisSta == UI_MODEL_STA_SHIFT_IN_ING)//如果是移动动作
					{
						ui.model[i].semp.curX = ui.model[i].semp.curY = 0;//清除移动参数变量
						ui.model[i].semp.sta = SE_MOVE_STA_INIT;//初始化移动参数状态
						ui.model[i].semp.info = Malloc(sizeof(seImageInfo_t*)*2);//分配储存seImageInfo_t*类型的空间
						ui.model[i].semp.info[0] = Malloc(sizeof(seImageInfo_t));//配分seImageInfo_t类型的空间
						ui.model[i].semp.info[1] = Malloc(sizeof(seImageInfo_t));
						ui.model[i].image = mallocImage(i,ui.model[i].depth,ui.model[i].background);//分配图片
						getModelInfo(&ui.model[i],i);//获取Info
					}
					else if(ui.model[i].thisSta == UI_MODEL_STA_UPDATE_ING)//如果是更新图像
					{
						//============================图像的更新处理============================
						ui.model[i].image = mallocImage(i,ui.model[i].depth,ui.model[i].background);//分配图片
						oledShowImage(ui.model[i].showX,ui.model[i].showY,ui.model[i].showX + ui.model[i].showWidth,ui.model[i].showY + ui.model[i].showHigh,ui.model[i].image);//刷新到显存
						oledUpdateBuffToShow();//刷新显示
						ui.model[i].thisSta -= 1;//完成动作
						imageFree(ui.model[i].image);//释放图片
					}
					ui.model[i].lastSta = ui.model[i].thisSta;//更新状态
				}
			}
		}
		//============================图像的移动处理============================
		for(int i=0;i<UI_MODEL_NUM;i++)
		{
			if(ui.model[i].semp.sta == SE_MOVE_STA_INIT)//如果图像移动未动作完成
			{
				if(ui.model[i].curSpeed == 0)
				{
					//ui.model[i].image = mallocImage(i,ui.model[i].depth,ui.model[i].background);//分配图片
					//getModelInfo(&ui.model[i],i);//获取Info
					if(ui.model[i].lastSta == UI_MODEL_STA_SHIFT_IN_ING)
						seMoveShiftIn(&ui.model[i].semp);//移入图像
					else if(ui.model[i].lastSta == UI_MODEL_STA_SHIFT_OUT_ING)
						seMoveShiftOut(&ui.model[i].semp);//移出图像
					ui.model[i].curSpeed = ui.model[i].setSpeed;//刷新移动速度
					//imageFree(ui.model[i].image);//释放图像
				}
				else
					ui.model[i].curSpeed --;
				if(ui.model[i].semp.sta == SE_MOVE_STA_DONE)//如果图像移动完成
				{
					ui.model[i].thisSta = ui.model[i].lastSta -= 1;//进入完成状态
					imageFree(ui.model[i].image);//释放图像
					Free(ui.model[i].semp.info[0]);//释放内存
					Free(ui.model[i].semp.info[1]);
					Free(ui.model[i].semp.info);
				}
				oledUpdateBuffToShow();//刷新显示
			}
		}
	}
}

static u8 *modeImage (ac_mode_t mode,u8 depth,u8 background)
{
	switch((u8)mode)
	{
		case AC_MODE_AUTO:	return image16GrayGet(gImage_autoMode,sizeof(gImage_autoMode));
		case AC_MODE_COLD:	return image16GrayGet(gImage_coldMode,sizeof(gImage_coldMode));
		case AC_MODE_HEAT:	return image16GrayGet(gImage_heatMode,sizeof(gImage_heatMode));
		case AC_MODE_SLEEP:	return image16GrayGet(gImage_sleepMode,sizeof(gImage_sleepMode));
		case AC_MODE_WIND:	return image16GrayGet(gImage_blowing_inMode,sizeof(gImage_blowing_inMode));
	}
	return NULL;
}

static u8 *mallocImage (u8 index,u8 depth,u8 background)
{
	switch(index)
	{
		case UI_MODEL_POWER:				return image16GrayGet(gImage_power,sizeof(gImage_power));//电源图像
		case UI_MODEL_SWIND:				return image16GrayGet(gImage_windd,sizeof(gImage_windd));//扫风图像
		case UI_MODEL_WINDS:				return windSpeed(ui.model[UI_MODEL_WINDS].showWidth,ui.model[UI_MODEL_WINDS].showHigh,depth,background,fun.ac.winds);//风速图像
		case UI_MODEL_AREFACTION:		return image16GrayGet(gImage_arefaction,sizeof(gImage_arefaction));//除湿图像
		case UI_MODEL_TIME:					return image16GrayGet(gImage_time,sizeof(gImage_time));//定时图像
		case UI_MODEL_WIFI:					return image16GrayGet(gImage_wifi,sizeof(gImage_wifi));//WiFi图像
		case UI_MODEL_MODE:					return modeImage(fun.ac.showMode,depth,background);//模式图像
		case UI_MODEL_VALUE:				
			if(((value_t *)ui.model[UI_MODEL_VALUE].param)->type == AC_VALUE_TYPE_SET_TIME)//设定时间
				return imageTime(ui.model[UI_MODEL_VALUE].showWidth,ui.model[UI_MODEL_VALUE].showHigh,depth,background,((value_t *)ui.model[UI_MODEL_VALUE].param)->value);//时间图像
			else if(((value_t *)ui.model[UI_MODEL_VALUE].param)->type == AC_VALUE_TYPE_SET_TEMP)//设定温度
				return imageTemp(ui.model[UI_MODEL_VALUE].showWidth,ui.model[UI_MODEL_VALUE].showHigh,depth,background,((value_t *)ui.model[UI_MODEL_VALUE].param)->value);//温度图像
			else if(((value_t *)ui.model[UI_MODEL_VALUE].param)->type == AC_VALUE_TYPE_CUR_TEMP)//当前温度
				return imageTemp(ui.model[UI_MODEL_VALUE].showWidth,ui.model[UI_MODEL_VALUE].showHigh,depth,background,((value_t *)ui.model[UI_MODEL_VALUE].param)->value);//温度图像
		case UI_MODEL_LABEL:				return imageTempLabel(ui.model[UI_MODEL_LABEL].showWidth,ui.model[UI_MODEL_LABEL].showHigh,depth,background,ui.model[index].label);//绘画标签
	}
	return NULL;
}

static void getModelInfo (ui_model_info_t *model,u8 index)
{
	model->semp.info[0]->addr = model->image;
	model->semp.info[0]->width 	= model->showWidth;
	model->semp.info[0]->high 	= model->showHigh;
	model->semp.info[1]->addr = (u8 *)oledShowBuff;
	model->semp.info[1]->xStart = model->showX;
	model->semp.info[1]->yStart = model->showY;
	model->semp.info[1]->xEnd = model->semp.info[1]->xStart + model->semp.info[0]->width;
	model->semp.info[1]->yEnd = model->semp.info[1]->yStart + model->semp.info[0]->high;
	model->semp.info[1]->high = model->semp.info[1]->yEnd - model->semp.info[1]->yStart;
	model->semp.info[1]->width = model->semp.info[1]->xEnd - model->semp.info[1]->xStart;
	model->semp.depth = model->depth;
	model->semp.background = model->background;
}

static void timeCalBack (void)
{
	if(ui.rate)
		ui.rate--;
}

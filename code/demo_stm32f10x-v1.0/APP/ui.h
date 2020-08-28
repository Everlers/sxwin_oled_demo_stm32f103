#ifndef _UI_H_
#define _UI_H_
#include "hal_sys.h"
#include "seMove.h"

//ui.model[x]
#define UI_MODEL_NUM						9//模块数量
#define UI_MODEL_POWER					0//电源
#define UI_MODEL_MODE						1//模式
#define UI_MODEL_VALUE					2//温度/时间
#define UI_MODEL_LABEL					3//标签
#define UI_MODEL_SWIND					4//扫风
#define UI_MODEL_WINDS					5//风速
#define UI_MODEL_AREFACTION			6//除湿
#define UI_MODEL_TIME						7//定时
#define UI_MODEL_WIFI						8//WiFi

typedef enum{
	UI_MODEL_STA_SHIFT_OUT_DONE = 0,//已经移出
	UI_MODEL_STA_SHIFT_OUT_ING 	= 1,//正在移出
	UI_MODEL_STA_SHIFT_IN_DONE 	= 2,//已经移出
	UI_MODEL_STA_SHIFT_IN_ING 	= 3,//正在移入
	UI_MODEL_STA_UPDATE_DONE		= 4,//更新完成
	UI_MODEL_STA_UPDATE_ING			=	5,//更新中
	UI_MODEL_STA_INIT_DONE			=	6,//初始化完成
}ui_model_sta_t;

typedef struct{
	ui_model_sta_t		thisSta;		//本次状态
	ui_model_sta_t		lastSta;		//上次状态
	seMoveParam_t 		semp;				//移动参数
	u8								setSpeed;		//设定移动速度 单位：ms/像素
	u8								curSpeed;		//当前移动速度
	u8								showX;			//显示位置的x坐标
	u8								showY;			//显示位置的y坐标
	u8								showWidth;	//显示的宽度
	u8								showHigh;		//显示的高度
	u8								depth;			//图像显示的深度
	u8								background;	//图像背景的深度
	u8 								*image;			//图像(动态加载，以字节为像素单位)
	char							*label;			//标签(用于填写图像用到的字)
	void 							*param;			//参数(一些特殊功能的参数)
}ui_model_info_t;

typedef struct{
	u16 state;			//状态
	u8	rate;				//刷新速度
	ui_model_info_t model[UI_MODEL_NUM];
}ui_t;

void uiInit (void);
void uiTask (void);

void uiModelShowDepthSet(u8 index,u8 depth,u8 background);//模块灰度设置
void *uiModelParamGet(u8 index);//查询模块的特殊参数
void uiModelParamSet(u8 index,void *param);//设定模块的特殊参数
void uiModelLabelSet(u8 index,char *label);//设定模块的标签
ui_model_sta_t uiModelGetThisSta (u8 index);//查询模块的当前状态
ui_model_sta_t uiModelGetLastSta (u8 index);//查询模块的上次状态
void uiModelSetThisSta (u8 index,ui_model_sta_t sta);//设定模块状态
FlagStatus uiModelLastStaDoneGet (u8 index);//查询上次状态是否完成
FlagStatus uiModelThisStaDoneGet (u8 index);//查询本次状态是否完成
FlagStatus uiAllModelThisStaDoneGet (void);//查询本次状态是否完成
FlagStatus uiAllModelLastStaDoneGet (void);//查询所有模块的状态是否完成
void uiModelSempDirSet (u8 index,seMoveDir_t inDir,seMoveDir_t outDir);//设定模块移动的方向
#endif

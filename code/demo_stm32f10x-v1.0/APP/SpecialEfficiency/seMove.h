#ifndef _SEMOVE_H_
#define _SEMOVE_H_
#include "hal_sys.h"

#define SE_MOVE_ONE_PIXEL				2//单次移动的像素

typedef enum{
	SE_MOVE_STA_INIT = 0,					//正在初始化
	SE_MOVE_STA_SEING = 1,				//正在播放特效
	SE_MOVE_STA_DONE = 2,					//完成
}seMoveState;

typedef enum{
	SE_MOVE_DIR_LR = 0,//图像从左至右移
	SE_MOVE_DIR_RL = 1,//图像从右至左移
	SE_MOVE_DIR_UD = 2,//图像从上至下移
	SE_MOVE_DIR_DU = 3,//图像从下至上移
}seMoveDir_t;

typedef struct{
	FlagStatus on;						//启用/禁用回弹效果
	u8 speed;									//回弹速度
}seMoveKickBack_t;

typedef struct{
	u8 	*addr;				//地址
	u16	high;					//高度
	u16	width;				//宽度
	u16 xStart;				//坐标
	u16 yStart;				//坐标
	u16	xEnd;
	u16	yEnd;
}seImageInfo_t;	//信息

typedef struct{
	//设定参数
	seImageInfo_t			**info;				//信息
	seMoveDir_t 			inDir;				//移入的移动方向
	seMoveDir_t 			outDir;				//移出的移动方向
	seMoveKickBack_t	kickBack;			//回弹效果
	u8 								depth;				//图像深度
	u8								background;		//背景深度
	
	//操作数据(每次动作前清除)
	seMoveState sta;	//当前状态
	u8 curX,curY;			//当前显示的xy坐标位置
}seMoveParam_t;	//图像移动参数

void seMoveShiftIn(seMoveParam_t *sp);
void seMoveShiftOut(seMoveParam_t *sp);
void seMoveShift(seImageInfo_t *showBufInfo,seImageInfo_t *targetInfo,u8 background);

#endif

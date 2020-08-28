#include "seMove.h"
//#include "malloc.h"
#include "string.h"
#include "oled.h"

//图像移入 (从空白移入)
//参数sp->info[0]：提供显示的图像以及图像大小
//参数sp->info[1]：提供OLED显示缓冲以及图片显示的位置
void seMoveShiftIn(seMoveParam_t *sp)
{
	seImageInfo_t *showBufInfo = sp->info[1];
	seImageInfo_t *imageInfo = sp->info[0];
	if(sp->inDir == SE_MOVE_DIR_DU || sp->inDir == SE_MOVE_DIR_UD)//如果是上下移动
	{
		sp->curY+=SE_MOVE_ONE_PIXEL;//移动Y坐标
		if(sp->curY >= imageInfo->high)//整个移动完成
		{
			sp->curY = imageInfo->high;
			if(sp->kickBack.on == SET)//如果开启回弹特效
				sp->sta = SE_MOVE_STA_SEING;//进入特效
			else
				sp->sta = SE_MOVE_STA_DONE;//进入完成状态
		}
		u8 startY = imageInfo->high - sp->curY;//开始复制的y坐标
		for(u16 y=0;y<sp->curY;y++)//复制移动出来的所有数据
		{
			u8 scShowY = startY + y + showBufInfo->yStart;//节省计算
			u16 scImageY = y*imageInfo->width;
			u16 scImageY1 = (imageInfo->high - sp->curY + y) * imageInfo->width;
			u16 scShowY1 = y + showBufInfo->yStart;
			for(u16 x=0;x<imageInfo->width;x++)//复制y向的所有x坐标
			{
				if(sp->inDir == SE_MOVE_DIR_UD)//图像从上到下移入
					oledShowBuffWrite(x + showBufInfo->xStart,scShowY1,imageInfo->addr[x + scImageY1]);
				else//图像从下到上移入
					oledShowBuffWrite(x + showBufInfo->xStart,scShowY,imageInfo->addr[x + scImageY]);
			}
		}
	}
	else if(sp->inDir == SE_MOVE_DIR_LR || sp->inDir == SE_MOVE_DIR_RL)//如果是左右移动
	{
		sp->curX+=SE_MOVE_ONE_PIXEL;//移动X坐标
		if(sp->curX >= imageInfo->width)//整个移动完成
		{
			sp->curX = imageInfo->width;
			if(sp->kickBack.on == SET)//如果开启回弹特效
				sp->sta = SE_MOVE_STA_SEING;//进入特效
			else
				sp->sta = SE_MOVE_STA_DONE;//进入完成状态
		}
		u8 startX = imageInfo->width - sp->curX;//开始复制的坐标
		for(u16 x=0;x<sp->curX;x++)//复制移动出来的所有数据
		{
			u16 scShowX1 = x + showBufInfo->xStart;
			u8 scShowX = startX + x + showBufInfo->xStart;
			u16 scImageX1 = (imageInfo->width - sp->curX + x);
			for(u16 y=0;y<imageInfo->high;y++)//复制y向的所有x坐标
			{
				if(sp->inDir == SE_MOVE_DIR_LR)//图像从左到右移入
					oledShowBuffWrite(scShowX1,y + showBufInfo->yStart,imageInfo->addr[(y*imageInfo->width) + scImageX1]);
				else//图像从下到上移入
					oledShowBuffWrite(scShowX,y + showBufInfo->yStart,imageInfo->addr[(y*imageInfo->width) + x]);
			}
		}
	}
}

//图像移出
//参数sp->info[0]：提供显示的图像以及图像大小
//参数sp->info[1]：提供OLED显示缓冲以及图片显示的位置
void seMoveShiftOut(seMoveParam_t *sp)
{
	seImageInfo_t *showBufInfo = sp->info[1];
	seImageInfo_t *imageInfo = sp->info[0];
	if(sp->outDir == SE_MOVE_DIR_DU || sp->outDir == SE_MOVE_DIR_UD)//如果是上下移动
	{
		sp->curY+=SE_MOVE_ONE_PIXEL;
		if(sp->curY >= showBufInfo->high)//如果移动完成
		{
			sp->curY = showBufInfo->high;
			sp->sta = SE_MOVE_STA_DONE;//进入完成状态
		}
		if(sp->outDir == SE_MOVE_DIR_UD)
		{
			if(sp->curY >= SE_MOVE_ONE_PIXEL)
			{
				for(u8 c=0;c<SE_MOVE_ONE_PIXEL;c++)
				{
					u8 y = (sp->curY - (c+1)) + showBufInfo->yStart;
					for(u16 x=showBufInfo->xStart;x<showBufInfo->xEnd;x++)
						oledShowBuffWrite(x,y,sp->background);
				}
			}
			for(u16 y=0;y<(showBufInfo->high - sp->curY);y++)
			{
				u8 showY = y + showBufInfo->yStart + sp->curY;
				u16 imageY = y * imageInfo->width;
				for(u16 x=0;x<showBufInfo->width;x++)
					oledShowBuffWrite(x + showBufInfo->xStart,showY,imageInfo->addr[ x + imageY]);
			}
		}
		else
		{
			if(sp->curY >= SE_MOVE_ONE_PIXEL)
			{
				for(u8 c=0;c<SE_MOVE_ONE_PIXEL;c++)
				{
					u8 y = (showBufInfo->yEnd - (sp->curY - (c))) + showBufInfo->yStart;
					for(u16 x=showBufInfo->xStart;x<showBufInfo->xEnd;x++)
						oledShowBuffWrite(x,y,sp->background);
				}
			}
			for(u16 y=0;y<(showBufInfo->high - sp->curY);y++)
			{
				u8 showY = y + showBufInfo->yStart;
				u16 imageY = (y + sp->curY) * imageInfo->width;
				for(u16 x=0;x<showBufInfo->width;x++)
					oledShowBuffWrite(x + showBufInfo->xStart,showY,imageInfo->addr[ x + imageY]);
			}
		}
	}
	else if(sp->outDir == SE_MOVE_DIR_LR || sp->outDir == SE_MOVE_DIR_RL)//如果是左右移动
	{
		sp->curX+=SE_MOVE_ONE_PIXEL;
		if(sp->curX >= showBufInfo->width)//如果移动完成
		{
			sp->curX = showBufInfo->width;
			sp->sta = SE_MOVE_STA_DONE;//进入完成状态
		}
		if(sp->outDir == SE_MOVE_DIR_LR)
		{
			if(sp->curX >= SE_MOVE_ONE_PIXEL)
			{
				for(u8 c=0;c<SE_MOVE_ONE_PIXEL;c++)
				{
					u8 x = (sp->curX - (c+1)) + showBufInfo->xStart;
					for(u16 y=showBufInfo->yStart;y<showBufInfo->yEnd;y++)
						oledShowBuffWrite(x,y,sp->background);
				}
			}
			for(u16 x=0;x<(showBufInfo->width - sp->curX);x++)
			{
				u8 showX = x + showBufInfo->xStart + sp->curX;
				for(u16 y=0;y<showBufInfo->high;y++)
					oledShowBuffWrite(showX,y + showBufInfo->yStart,imageInfo->addr[ (y * imageInfo->width) + x]);
			}
		}
		else
		{
			if(sp->curX >= SE_MOVE_ONE_PIXEL)
			{
				for(u8 c=0;c<SE_MOVE_ONE_PIXEL;c++)
				{
					u8 x = (showBufInfo->xEnd - (sp->curX - (c))) + showBufInfo->xStart;
					for(u16 y=showBufInfo->yStart;y<showBufInfo->yEnd;y++)
						oledShowBuffWrite(x,y,sp->background);
				}
			}
			for(u16 x=0;x<(showBufInfo->width - sp->curX);x++)
			{
				u8 showX = x + showBufInfo->xStart;
				u8 imageX = sp->curX + x;
				for(u16 y=0;y<showBufInfo->high;y++)
					oledShowBuffWrite(showX,y + showBufInfo->yStart,imageInfo->addr[ (y * imageInfo->width) + imageX]);
			}
		}
	}
}

//图像移动
//参数showBufInfo：提供OLED显示缓冲以及需要移动的图片位置坐标
//参数targetInfo：需要移动到的位置坐标 此缓冲会存放临时图像
//参数background：移动后的背景颜色
void seMoveShift(seImageInfo_t *showBufInfo,seImageInfo_t *targetInfo,u8 background)
{
	/*if(targetInfo->addr == NULL)//如果未分配缓冲地址
		targetInfo->addr = Malloc(sp->info[2]->high * sp->info[2]->width);//分配图形大小的缓冲
	//进行图片复制 同时转换成字节一个像素的缓冲
	for(u16 y=showBufInfo->yStart;y<showBufInfo->yEnd;y++)//复制整个图片且清除
	{
		for(u16 x=showBufInfo->xStart;x<showBufInfo->xEnd;x++)
		{
			targetInfo->addr[(x - showBufInfo->xStart) + ((y - showBufInfo->yStart) * showBufInfo->width)] = oledShowBuffRead(x,y);//复制当前像素
			oledShowBuffWrite(x,y,sp->background);//清除当前像素
		}
	}
	//复制到目标位置
	for(u16 y=targetInfo->yStart;y<targetInfo->yEnd;y++)
		for(u16 x=targetInfo->xStart;x<targetInfo->xEnd;x++)
			oledShowBuffWrite(x,y,targetInfo->addr[(x - targetInfo->xStart) + ((y - targetInfo->yStart)*targetInfo->width)]);*/
}

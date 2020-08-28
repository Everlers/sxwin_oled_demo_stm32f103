#ifndef _DESTRUCT_H_
#define _DESTRUCT_H_
#include "stm32f10x.h"
#include "Hal_flash.h"

//板子出厂时的程序会写入一份UUID 之后更新则不再写入 如果代码被烧录到另一个没有UUID的板子上则受到次数的限制

#define DES_WRITE_UUID												0//如果板子第一次烧录程序 则需要写入UUID  如果非第一次抄录则写0  
#define DES_UUID_LENGTH												16//128位的UUID
#define DES_CODE_RUN_ADDR											0x08000000//程序代码运行的首地址 (次数用尽后清零代码首地址)
#define DES_FLAG_SAVE_ADDR										DESTRUCT_SAVE_ADDR//标志已经修改过此设备的销毁次数
#define DES_NUM_SAVE_ADDR											DES_FLAG_SAVE_ADDR+1//保存运行次数的Flash地址
#define	DES_UUID_SAVE_ADDR										DES_NUM_SAVE_ADDR + 4//UUID保存地址 

#define DES_WHILE_OR_DESTRUCT									1//死循环或者销毁程序  1:死循环  0:销毁
#define DES_NUMBER														2000//可用次数  次数到后销毁
#define DES_REVISE_FLASG											0xAA//标记  （如果次数用完修改此标记重新设置使用次数）


typedef enum{
DES_UUID,
DES_NUM,
}DestructNumOrUUID_t;//自毁选择检测对象

void DestructSystem(void);//自毁系统(启动程序时调用一次减少一次可用次数)
#endif

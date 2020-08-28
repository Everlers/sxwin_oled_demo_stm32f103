#ifndef _BMS_H_
#define _BMS_H_
#include "stm32f10x.h"
#define BAT_PORT									GPIOB
#define BAT_CHARGE_BIT						GPIO_Pin_4
#define BAT_CHARGE								(BAT_PORT->IDR & BAT_CHARGE_BIT)

#define HIGH_PULL_RES							100.0//上拉阻值(单位：千欧)
#define LOW_PULL_RES							47.0 //下拉阻值(单位：千欧)
#define MAX_VOLTAGE								8.1	 //最高电压(单位：伏)
#define LOW_VOLTAGE								7.0	 //最低电压(单位：伏)
typedef struct{
float voltage;//电压
u8 volume;//容量
}BMS_INFO_t;


void BMS_Init(void);
BMS_INFO_t *BMS_InfoGet(void);
void BMS_Task(void);
#endif

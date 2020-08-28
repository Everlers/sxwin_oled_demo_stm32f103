#ifndef _HAL_KEY_H_
#define _HAL_KEY_H_
#include "hal_sys.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#define HAL_KEY_SHAKE_TIME						2//祛抖时间
#define HAL_KEY_LONG_PRESS_TIME				1000//长按时间

#define HAL_KEY_1										BV(1)
#define HAL_KEY_2										BV(2)
#define HAL_KEY_3										BV(3)
#define HAL_KEY_4										BV(4)
#define HAL_KEY_5										BV(5)
#define HAL_KEY_6										BV(6)
#define HAL_KEY_ROTARY							BV(7)
#define HAL_KEY_ALL									(HAL_KEY_1|HAL_KEY_2|HAL_KEY_3|HAL_KEY_4|HAL_KEY_5|HAL_KEY_6|HAL_KEY_ROTARY)

/*io pin config*/
#define HAL_KEY_PORT  GPIOB->IDR
#define HAL_KEY_SW_1	GPIO_Pin_10
#define HAL_KEY_SW_2	GPIO_Pin_11
#define HAL_KEY_SW_3	GPIO_Pin_12
#define HAL_KEY_SW_4	GPIO_Pin_13
#define HAL_KEY_SW_5	GPIO_Pin_14
#define HAL_KEY_SW_6	GPIO_Pin_15
#define HAL_KEY_SW_7	GPIO_Pin_4
#define HAL_KEY_SW_ALL (HAL_KEY_SW_1|HAL_KEY_SW_2|HAL_KEY_SW_3|HAL_KEY_SW_4|HAL_KEY_SW_5|HAL_KEY_SW_6|HAL_KEY_SW_7)

void HalKeyInit(void);
void HalKeyTask(void);
u8 HalKeyGetShortPress(u8 sw);
u8 HalKeyGetLongPress(u8 sw);
u8 HalKeyGetShortPressNoClear(u8 sw);
u8 HalKeyGetLongPressNoClear(u8 sw);
u8 HalKeyGetLoosen(u8 sw);

#endif

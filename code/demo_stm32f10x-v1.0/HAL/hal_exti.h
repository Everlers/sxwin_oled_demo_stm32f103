#ifndef _HAL_EXTI_H_
#define _HAL_EXTI_H_
#include "hal_sys.h"

#define HAL_EXTI_MAX_CB_NUM				20//最大回调数量 (一个IO可多个回调)

typedef enum{
HAL_EXTI_GPIOA = 0,
HAL_EXTI_GPIOB = 1,
HAL_EXTI_GPIOC = 2,
HAL_EXTI_GPIOD = 3,
HAL_EXTI_GPIOE = 4,
HAL_EXTI_GPIOF = 5,
HAL_EXTI_GPIOG = 6,
}HalGpioTypeDef;

typedef void (*HalExtiCallBack_t)(void);//外部中断回调

typedef struct{
HalExtiCallBack_t cb;//回调函数
u8 line;//需要回调的Pin
}HalExti_t;

//外部中断初始化注册
//参数GPIOx:根据HalGpioTypeDef枚举定义 
//参数PINx:0~15
//参数IOMode:参考EXTITrigger_TypeDef类定义 
//参数Trigger:参考EXTITrigger_TypeDef (选择条边沿)
//参数CB:中断回调函数
void HalExtiInit(HalGpioTypeDef GPIOx,
									 	u8 PINx,
											GPIOMode_TypeDef IOMode,
												EXTITrigger_TypeDef Trigger,
													HalExtiCallBack_t CB);

#endif


#ifndef _HAL_BUZZER_H_
#define _HAL_BUZZER_H_
#include "hal_sys.h"
#include "FreeRTOS.h"
#include "Task.h"
#include "Timers.h"

//Active buzzer only 
#define GPIO_PIN									GPIO_Pin_0
#define GPIO_PORT									GPIOB
#define RCC_APB2									RCC_APB2Periph_GPIOB

#define BUZZER_SET								TIM_SetCompare3(TIM3,128);//GPIO_PORT->ODR |= GPIO_PIN
#define BUZZER_CLR								TIM_SetCompare3(TIM3,0);//GPIO_PORT->ODR &= ~GPIO_PIN

typedef struct{
	u16 Number;//Cycle execution times 
	u16 Period;//Period
	u16 DutyCycle;
	FunctionalState State;//state Enable:Disable
	FunctionalState BuzzerSw;//Buzzer switch 
}Buzzer_t;

/*___________________________使用说明______________________________
前言：这个HAL只适合有源蜂鸣器 																		 |
注意：初始化之前需要先初始化HalTime Buzzer需要注册定时回掉				 |
-void HalBuzzerInit(void)------------------------------------------|
		初始化蜂鸣器																									 |
																																	 |
-void HalBuzzSetPeriod(u16 Period)---------------------------------|
		设置蜂鸣器的导通周期- 一个周期为定时器(HalTime)的回调时间			 |
																																	 |
-void HalBuzzerSet(u8 Number,u16 Period,u16 dutyCucle)-------------|
		设置Buzzer响应的次数以及周期 以及占空比 											 |
		示例 ：																												 |
		Number = 3  Period = 200 dutyCucle = 100 则响三次 						 |
		每次响100个周期停100个周期(周期为HalTime回调时间)							 |
																																	 |
-void HalBuzzerStop(void)------------------------------------------|
										立即停止Buzzer																 |
																																	 |
-FunctionalState HalGetBuzzerState(void)---------------------------|
		查询Buzzer导通状态 ENABLE or DISABLE													 |
																																	 |
-void HalBuzzerSwitch(FunctionalState State)-----------------------|
		开关Buzzer功能-使能或者失能Buzzer 初始化后默认是ENABLE				 |
																																	 |
-FunctionalState HalBuzzerSwitchState(void)------------------------|
		查询Buzzer开关状态																						 |
___________________________________________________________________|
*/

void HalBuzzerInit(void);
void HalBuzzerTask(void);
void HalBuzzSetPeriod(u16 Period);
void HalBuzzerSet(u8 Number,u16 Period,u16 dutyCucle);
void HalBuzzerStop(void);
FunctionalState HalGetBuzzerState(void);
void HalBuzzerSwitch(FunctionalState State);
FunctionalState HalBuzzerSwitchState(void);
#endif

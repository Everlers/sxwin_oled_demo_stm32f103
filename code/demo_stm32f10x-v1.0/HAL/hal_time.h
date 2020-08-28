#ifndef _HAL_TIME_H_
#define _HAL_TIME_H_
#include "stm32f10x.h"

#define HAL_TIME_MAX_CB_NUM										20//设置最大回调数量

typedef void (*HalTimeCallBack_t) ( void );

/*______________________使用说明______________________________
-void HalTimeInit(u16 Prescaler, u16 Period)------------------|
		初始化定时器	Prescaler:预分频  	Period:计数周期												|
																															|
-HalTimeRegisteringCallbacks (HalTimeCallBack_t CallBack)-----|
		注册定时回调																										|
																															|
-u8 HalTimeRemoveCallback (HalTimeCallBack_t CallBack)--------|
		删除定时回调 返回值- 0:OK 1:Error(未找到已经注册的函数)	        		|
______________________________________________________________|
*/

typedef enum{
HAL_TIME_MODE_RELOAD,//重装
HAL_TIME_MODE_ONCE,//一次
}HalTimeCbMode;//回调模式

typedef struct{
u16 count;//计数器
u16 prescaler;//预分频
HalTimeCallBack_t cb;//回调函数
HalTimeCbMode mode;//回调模式
}HalTime_t;

void HalTimeInit(u16 Prescaler, u16 Period);
u8 HalTimeExStart(HalTimeCallBack_t CallBack,u16 prescaler);
u8 HalTimeRegisteringCallbacks (HalTimeCallBack_t CallBack,u16 prescaler);
u8 HalTimeCancelCallback (HalTimeCallBack_t CallBack);
#endif


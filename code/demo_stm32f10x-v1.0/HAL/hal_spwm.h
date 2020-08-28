#ifndef _HAL_SPWM_H_
#define _HAL_SPWM_H_
#include "stm32f10x.h"

#define HAL_SPWM_MAX_CH_NUM									20//定义最大软件PWM通道的数量

typedef u8 pwm_id;

typedef enum{
SPWM_OFF = 0,
SPWM_ON = 1,
}spwm_switch;//开关PWM

typedef enum{
POLARITY_HIGH = 0,
POLARITY_LOW,
}polarity_sta;//输出极性

typedef enum{
SPWM_SLEEP = 0,//此软件PWM在休眠的状态（目前没有使用,可被使用）
SPWM_RUN,//此软件PWM正在工作 （目前正在输出PWM不可被使用）
}spwm_reg_sta;

typedef struct{
u16 counter;//计数器
u16 period;//周期
u16 pulse;//占空比
volatile u32 *io_port;//引脚端口
u32 io_pin;//引脚位
polarity_sta polarity;//输出极性
spwm_switch sw;//软件pwn开关
spwm_reg_sta sta;//状态寄存器
}spwm_t;//软件PWM寄存器

/*__________________________________________________________________________
-void HalSPWMInit(void)-----------------------------------------------------|
		初始化软件PWM功能 确保所有PWM通道关闭且清除的状态																						|
																																						|
-void HalSPWMTask(void)-----------------------------------------------------|
		给软件PWM时钟周期  可以使用HalTime注册一个回调 这是整个Spwm的时钟							  			    			|
																																						|
-pwm_id HalSPWMAdd(spwm_t *pwm)---------------------------------------------|
		添加一个PWM通道  参数pwm=spwm_t(初始化设置)																					|
		返回值pwm_id是每个spwm的身份标识(句柄)																								|
																																						|
-void HalSPWMCmd(pwm_id id,spwm_switch sw)----------------------------------|
		使能or失能此ID的pwm输出功能																												|
		参数id = 添加后返回的ID																													|
		参数sw = 开启或者关闭																														|
																																						|
-void HalSPWMPulseSet(pwm_id id,u16 pulse)----------------------------------|
		占空比设置  																																	|
		参数id = 添加的pwm返回值																												|
		参数pulse = 设置占空比																													|
																																						|
-void HalSPWMPeriodSet(pwm_id id,u16 period)--------------------------------|
		周期设置																																		|
		参数 id = 添加pwm的返回值																												|
		参数period = PWM周期																												|
																																						|
-void HalSPWMCountSet(pwm_id id,u16 counter)--------------------------------|
		计数器设置																																		|
		参数 id = 添加PWM的返回值																												|
		参数counter = 设置计数器的值																											|
																																						|
-u16 HalSPWMPeriodGet(pwm_id id)--------------------------------------------|
		查询pwm周期																																	|
		参数id = addr spwm return value																						|
		返回值：周期																																	|
																																						|
-u16 HalSPWMPulseGet(pwm_id id)---------------------------------------------|
		查询占空比																																		|
																																						|
-u16 HalSPWMCountGet(pwm_id id)---------------------------------------------|
		查询计数值																																		|
																																						|
-spwm_switch HalSPWMSwGet(pwm_id id)----------------------------------------|
		查询spwm的开启状态																															|
____________________________________________________________________________|
*/

void HalSPWMInit(void);
void HalSPWMTask(void);
pwm_id HalSPWMAdd(spwm_t *pwm);
void HalSPWMCmd(pwm_id id,spwm_switch sw);
void HalSPWMPulseSet(pwm_id id,u16 pulse);
void HalSPWMPeriodSet(pwm_id id,u16 period);
void HalSPWMCountSet(pwm_id id,u16 counter);

u16 HalSPWMPeriodGet(pwm_id id);
u16 HalSPWMPulseGet(pwm_id id);
u16 HalSPWMCountGet(pwm_id id);
spwm_switch HalSPWMSwGet(pwm_id id);
#endif

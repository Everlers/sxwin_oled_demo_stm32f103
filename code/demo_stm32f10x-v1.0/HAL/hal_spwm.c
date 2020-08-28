#include "hal_spwm.h"
#include "string.h"

//软件PWM
static spwm_t spwm[HAL_SPWM_MAX_CH_NUM];

void HalSPWMInit(void)
{
	u8 i;
	for(i=0;i<HAL_SPWM_MAX_CH_NUM;i++){
		spwm[i].sta = SPWM_SLEEP;
	}
}

//软件pwm任务
void HalSPWMTask(void)
{
	u8 i;
	for(i=0;i<HAL_SPWM_MAX_CH_NUM;i++)
	{
		if(spwm[i].sta == SPWM_RUN)//是否在运行
		{
			if(spwm[i].sw == SPWM_ON)//如果PWM是开启的
			{
				spwm[i].counter++;
				
				if(spwm[i].counter > spwm[i].period)
				{
					if(spwm[i].pulse != 0)
					{
						if(spwm[i].polarity == POLARITY_HIGH)
							*spwm[i].io_port |= spwm[i].io_pin;
						else
							*spwm[i].io_port &= ~spwm[i].io_pin;
					}
					spwm[i].counter = RESET;
				}
				
				if(spwm[i].counter > spwm[i].pulse)
				{
						if(spwm[i].polarity == POLARITY_HIGH)
							*spwm[i].io_port &= ~spwm[i].io_pin;
						else
							*spwm[i].io_port |= spwm[i].io_pin;
				}
			}
			else //如果PWM是关闭的
			{
				if(spwm[i].polarity == POLARITY_HIGH)
					*spwm[i].io_port &= ~spwm[i].io_pin;
				else
					*spwm[i].io_port |= spwm[i].io_pin;
			}
		}
	}
}

pwm_id HalSPWMAdd(spwm_t *pwm)
{
	u8 i;
	for(i=0;i<HAL_SPWM_MAX_CH_NUM;i++)
	{
		if(spwm[i].sta == SPWM_SLEEP)//如果此PWM外设没有被使用
		{
			memcpy(&spwm[i],pwm,sizeof(spwm_t));//复制pwm设置信息
			spwm[i].sta = SPWM_RUN;//强制设置为运行状态
			pwm->sta = SPWM_RUN;//强制设置为运行状态
			return i;//返回PWM ID
		}
	}
	return 0xff;
}

u16 HalSPWMPeriodGet(pwm_id id)
{
	return spwm[id].period;
}

void HalSPWMPeriodSet(pwm_id id,u16 period)
{
	spwm[id].period = period;
}

u16 HalSPWMPulseGet(pwm_id id)
{
	return spwm[id].pulse;
}

void HalSPWMPulseSet(pwm_id id,u16 pulse)
{
	spwm[id].pulse = pulse;
}

u16 HalSPWMCountGet(pwm_id id)
{
	return spwm[id].counter;
}

void HalSPWMCountSet(pwm_id id,u16 counter)
{
	spwm[id].counter = counter;
}

spwm_switch HalSPWMSwGet(pwm_id id)
{
	return spwm[id].sw;
}

void HalSPWMCmd(pwm_id id,spwm_switch sw)
{
	spwm[id].sw = sw;
}

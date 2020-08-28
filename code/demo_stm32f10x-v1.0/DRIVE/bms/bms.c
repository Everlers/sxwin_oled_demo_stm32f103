#include "bms.h"
#include "hal_adc.h"

BMS_INFO_t bms_info;

void BMS_Init(void)
{
	GPIO_InitTypeDef GPIO_Struct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_Struct.GPIO_Pin = BAT_CHARGE_BIT;
	GPIO_Struct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Struct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BAT_PORT,&GPIO_Struct);
	
	HaladcInit();
}

BMS_INFO_t *BMS_InfoGet(void)
{
	u16 buf[32];
	u16 value;
	float v,i;
	
	HalAdcReadFifo(ADC_Channel_13,buf,32);//读取一个缓冲的ADC值数据
	value = HalAdcRead(ADC_Channel_13);

	v = (3.3/4096)*value;//计算引脚电压
	i = v/LOW_PULL_RES;//计算电流
	bms_info.voltage = i*(HIGH_PULL_RES+LOW_PULL_RES);//计算总电压
	if(bms_info.voltage >= MAX_VOLTAGE)//超出最大电压
		bms_info.volume = 100;
	else if(bms_info.voltage <= LOW_VOLTAGE)//电量过低
		bms_info.volume = 0;
	else
		bms_info.volume = (bms_info.voltage - LOW_VOLTAGE) * (100/(MAX_VOLTAGE - LOW_VOLTAGE));//计算百分比
	return &bms_info;
}


void BMS_Task(void)
{
	BMS_InfoGet();
	if(bms_info.voltage <= LOW_VOLTAGE)//如果电量过低
	{
		
	}
}


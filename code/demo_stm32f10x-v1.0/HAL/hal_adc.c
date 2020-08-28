#include "Hal_adc.h"

#define ADC_SampleTime	ADC_SampleTime_55Cycles5

static void ADCChannelGPIOInit(u8 Channel);
static u16 *ADCChannelBuff;
static void (*ConvertDoneCallback)(void);

static void NVICInit(void)
{
	NVIC_InitTypeDef NVICInitStruct;
	NVICInitStruct.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVICInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVICInitStruct.NVIC_IRQChannelSubPriority = 0;
	NVICInitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVICInitStruct);
}

void HaladcInit(u8 *ChannelTable, u16 *ChannelBuff, int ChannelNum)
{
	ADC_InitTypeDef ADCInitStruct;
	DMA_InitTypeDef DMA_InitStructure;
	ADCChannelBuff = ChannelBuff;
	ConvertDoneCallback = 0;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC,ENABLE);
	
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//复位结束	 
	
	DMA_DeInit(DMA1_Channel1);  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->DR;//ADC地址  
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ChannelBuff; //内存地址  
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //方向(从外设到内存)  
	DMA_InitStructure.DMA_BufferSize = ChannelNum; //传输内容的大小  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //外设地址固定  
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //内存地址固定  
	DMA_InitStructure.DMA_PeripheralDataSize =   
	DMA_PeripheralDataSize_HalfWord ; //外设数据单位  
	DMA_InitStructure.DMA_MemoryDataSize =   
	DMA_MemoryDataSize_HalfWord ;    //内存数据单位  
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular  ; //DMA模式：循环传输  
	DMA_InitStructure.DMA_Priority = DMA_Priority_High ; //优先级：高  
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;   //禁止内存到内存的传输  
		
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);  //配置DMA1的4通道  
	DMA_ITConfig (DMA1_Channel1,DMA_IT_TC,ENABLE);
	DMA_Cmd(DMA1_Channel1,ENABLE);  
	NVICInit();
	
	ADCInitStruct.ADC_Mode = ADC_Mode_Independent;//工作在独立模式
	ADCInitStruct.ADC_ScanConvMode = ENABLE;//单通道扫描
	ADCInitStruct.ADC_ContinuousConvMode = ENABLE;//关闭连续扫描
	ADCInitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//不用外部触发
	ADCInitStruct.ADC_DataAlign = ADC_DataAlign_Right;//向右对齐
	ADCInitStruct.ADC_NbrOfChannel = ChannelNum ;//采集通道数
	ADC_Init(ADC1,&ADCInitStruct);//初始化ADC1
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); //ADC时钟分频 72/8 = 9M
	for(int i = 0; i < ChannelNum; i++)
	{
		ADC_RegularChannelConfig(ADC1, ChannelTable[i], i + 1, ADC_SampleTime); 
		ADCChannelGPIOInit(ChannelTable[i]);
	}
	ADC_DMACmd(ADC1,ENABLE);  
  ADC_Cmd(ADC1,ENABLE); //使能ADC1
	
	ADC_ResetCalibration(ADC1);//复位校准
	while(ADC_GetResetCalibrationStatus(ADC1) != RESET);//等待复位校准完成
	ADC_StartCalibration(ADC1);//开始校准
	while(ADC_GetCalibrationStatus(ADC1) != RESET);//等待校准完成
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	//开始转换
}


void DMA1_Channel1_IRQHandler(void)
{
	if(ConvertDoneCallback != 0)
		ConvertDoneCallback();
	DMA_ClearFlag (DMA1_FLAG_TC1);
}

void HalAdcSetConvertCallback(void (*func)(void))
{
	ConvertDoneCallback = func;
}

u16 HalAdcRead(u8 ADC_Channel)
{
	ADCChannelBuff[ADC_Channel] = 0x8000;
	while(ADCChannelBuff[ADC_Channel] & 0x8000);		//等待转换完成
	return ADCChannelBuff[ADC_Channel];
}

void HalAdcReadFifo(u8 ADC_Channel,u16 *Fifo,u16 Len)
{
	u16 i;
	for(i=0;i<Len;i++)
		Fifo[i] = HalAdcRead(ADC_Channel);
}



static void ADCChannelGPIOInit(u8 Channel)
{
	GPIO_InitTypeDef GPIOInitStruct;
	GPIO_TypeDef *GPIOx;
	switch(Channel)
	{
		case 0:
			GPIOInitStruct.GPIO_Pin = GPIO_Pin_0;
			GPIOx = GPIOA;break;
		case 1:
			GPIOInitStruct.GPIO_Pin = GPIO_Pin_1;
			GPIOx = GPIOA;break;
		case 2:
			GPIOInitStruct.GPIO_Pin = GPIO_Pin_2;
			GPIOx = GPIOA;break;
		case 3:
			GPIOInitStruct.GPIO_Pin = GPIO_Pin_3;
			GPIOx = GPIOA;break;
		case 4:
			GPIOInitStruct.GPIO_Pin = GPIO_Pin_4;
			GPIOx = GPIOA;break;
		case 5:
			GPIOInitStruct.GPIO_Pin = GPIO_Pin_5;
			GPIOx = GPIOA;break;
		case 6:
			GPIOInitStruct.GPIO_Pin = GPIO_Pin_6;
			GPIOx = GPIOA;break;
		case 7:
			GPIOInitStruct.GPIO_Pin = GPIO_Pin_7;
			GPIOx = GPIOA;break;
		case 8:
			GPIOInitStruct.GPIO_Pin = GPIO_Pin_0;
			GPIOx = GPIOB;break;
		case 9:
			GPIOInitStruct.GPIO_Pin = GPIO_Pin_1;
			GPIOx = GPIOB;break;
		case 10:
			GPIOInitStruct.GPIO_Pin = GPIO_Pin_0;
			GPIOx = GPIOC;break;
		case 11:
			GPIOInitStruct.GPIO_Pin = GPIO_Pin_1;
			GPIOx = GPIOC;break;
		case 12:
			GPIOInitStruct.GPIO_Pin = GPIO_Pin_2;
			GPIOx = GPIOC;break;
		case 13:
			GPIOInitStruct.GPIO_Pin = GPIO_Pin_3;
			GPIOx = GPIOC;break;
		case 14:
			GPIOInitStruct.GPIO_Pin = GPIO_Pin_4;
			GPIOx = GPIOC;break;
		case 15:
			GPIOInitStruct.GPIO_Pin = GPIO_Pin_5;
			GPIOx = GPIOC;break;
		
	}
	GPIOInitStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIOInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOx,&GPIOInitStruct);
}

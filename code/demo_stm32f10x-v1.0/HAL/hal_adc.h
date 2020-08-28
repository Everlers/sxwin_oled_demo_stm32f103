#ifndef _HAL_ADC_H_
#define _HAL_ADC_H_
#include "stm32f10x.h"

/*___________________________________________使用说明_______________________________________________
-void HaladcInit(u8 *ChannelTable, u16 *ChannelBuff, int ChannelNum)--------------------------------|
			描述：用于初始化AD通道-初始化完成之后ChannelBuff数组会不断更新对应ChannelTable数组列表的数据 	|
			ChannelTable :需要开启的AD通道 列个数组形式写入 																						 	|
			ChannelBuff  :对应ChannelTable数组顺序的AD采集数据 																					 	|
			ChannelNum	 :使用的通道数量																																 	|
			|示例:____________________________________________________________________										|
			|		 ChannelTable = {0,3,2,1}																							|										|
			|		 ChannelBuff = AdData[sizeof(ChannelTable)]														|									 	|
			|		 ChannelNum = sizeof(ChannelTable)																		|									 	|
			|																																					|										|
			|他们的顺序是 :																														|										|
			|							ChannelBuff[0] = ADC1通道0 AIN0的数据(ChannelTable[0] = 0)	|										|
			|							ChannelBuff[1] = ADC1通道3 AIN3的数据(ChannelTable[1] = 3)	|										|
			|							ChannelBuff[2] = ADC1通道2 AIN2的数据(ChannelTable[1] = 2)	|										|
			|							ChannelBuff[3] = ADC1通道1 AIN1的数据(ChannelTable[1] = 1)	|										|
			|_________________________________________________________________________|										|
																																																		|
-u16 HalAdcRead(u8 ADC_Channel)---------------------------------------------------------------------|
			读取对应通道数据																																							|
			ADC_Channel对应初始化ChannelTable数组的列表顺序																								|
																																																		|
-void HalAdcSetConvertCallback(void (*func)(void))--------------------------------------------------|
			设置转换完成后的回调																																					|
																																																		|
-void HalAdcReadFifo(u8 ADC_Channel,u16 *Fifo,u16 Len)----------------------------------------------|
			读取对应通道的Len个数据																																				|
____________________________________________________________________________________________________|
*/

void HaladcInit(u8 *ChannelTable, u16 *ChannelBuff, int ChannelNum);
u16 HalAdcRead(u8 ADC_Channel);
void HalAdcSetConvertCallback(void (*func)(void));
void HalAdcReadFifo(u8 ADC_Channel,u16 *Fifo,u16 Len);

#endif

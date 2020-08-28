#include "hal_usart.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "malloc.h"

#if DEUSART1
u8 USART1_TX_BUF[USART_TX_LEN];
u8 USART1_RX_BUF[USART1_REC_LEN];
u16 USART1_RX_STA = RESET;
u8 USART1_TX_STA = RESET;
USARTReceiveIDLE_CB USART1RxIDLE_CB;
#endif
#if DEUSART2
u8 USART2_TX_BUF[USART_TX_LEN];
u8 USART2_RX_BUF[USART_REC_LEN]; 
u16 USART2_RX_STA = RESET;
u16 USART2_TX_STA = RESET;
USARTReceiveIDLE_CB USART2RxIDLE_CB;
#endif
#if DEUSART3
u8 USART3_TX_BUF[USART_TX_LEN];
u8 USART3_RX_BUF[USART_REC_LEN]; 
u16 USART3_RX_STA = RESET;
u16 USART3_TX_STA = RESET;
USARTReceiveIDLE_CB USART3RxIDLE_CB;
#endif

static void uart1_init(u32 bound,USARTReceiveIDLE_CB Callback);
static void uart2_init(u32 bound,USARTReceiveIDLE_CB Callback);
static void uart3_init(u32 bound,USARTReceiveIDLE_CB Callback);

void HalUSARTInit(USART_Port Portx, u32 bound, USARTReceiveIDLE_CB Callback)
{
   switch (Portx)
   {
      case HAL_USART_PORT1:
         uart1_init(bound,Callback);
				 
      break;
      case HAL_USART_PORT2:
         uart2_init(bound,Callback);
      break;
      case HAL_USART_PORT3:
         uart3_init(bound,Callback);
      break;
   }
}

static void uart1_init(u32 bound,USARTReceiveIDLE_CB Callback)
{
#if DEUSART1
   GPIO_InitTypeDef GPIO_InitStructure;
   USART_InitTypeDef USART_InitStructure;
   NVIC_InitTypeDef NVIC_InitStructure;
   DMA_InitTypeDef DMA_InitStructure;
	 
	 USART1RxIDLE_CB = Callback;//保存回调
   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟

   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
   GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
   GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

   NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= configMAX_SYSCALL_INTERRUPT_PRIORITY;//抢占优先级
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
   NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

   USART_InitStructure.USART_BaudRate = bound;//串口波特率
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
   USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
   USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
   USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
   USART_Init(USART1, &USART_InitStructure); //初始化串口
   USART_ClearFlag(USART1,USART_FLAG_IDLE);//清除中断标志位
	 USART_ClearFlag(USART1,USART_FLAG_RXNE);
	 USART_ClearFlag(USART1,USART_FLAG_TC);
   USART_ITConfig(USART1,USART_IT_TC,DISABLE); 
   USART_ITConfig(USART1,USART_IT_RXNE,DISABLE); 
   USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);//打开空闲中断
   /*DMA USART TX */
   DMA_DeInit(DMA1_Channel4);
   DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART1->DR;
   DMA_InitStructure.DMA_MemoryBaseAddr = (u32)USART1_TX_BUF;
   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
   DMA_InitStructure.DMA_BufferSize = USART_TX_LEN;
   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
   DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
   DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
   DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
   DMA_Init(DMA1_Channel4,&DMA_InitStructure);
   DMA_ClearFlag(DMA1_FLAG_TC4);
   DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
   DMA_Cmd(DMA1_Channel4,DISABLE);

   NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configMAX_SYSCALL_INTERRUPT_PRIORITY;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);

   /*DMA USART RX*/
   DMA_DeInit(DMA1_Channel5);
   DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART1->DR;
   DMA_InitStructure.DMA_MemoryBaseAddr = (u32 )USART1_RX_BUF;
   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
   DMA_InitStructure.DMA_BufferSize = USART1_REC_LEN;
   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
   DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
   DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
   DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
   DMA_Init(DMA1_Channel5,&DMA_InitStructure);
   DMA_Cmd(DMA1_Channel5,ENABLE);

   USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
   USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
   USART_Cmd(USART1, ENABLE);                    //使能串口
	 
#endif
}

static void uart2_init(u32 bound,USARTReceiveIDLE_CB Callback)
{
#if DEUSART2
   GPIO_InitTypeDef GPIO_InitStructure;
   USART_InitTypeDef USART_InitStructure;
   NVIC_InitTypeDef NVIC_InitStructure;
   DMA_InitTypeDef DMA_InitStructure;
	 
	 USART2RxIDLE_CB = Callback;//保存回调
   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
   GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.2
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
   GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.3 

   NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= configMAX_SYSCALL_INTERRUPT_PRIORITY;//抢占优先级
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级3
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
   NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

   USART_InitStructure.USART_BaudRate = bound;//串口波特率
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
   USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
   USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
   USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
   USART_Init(USART2, &USART_InitStructure); //初始化串口
   USART_ClearFlag(USART3,USART_FLAG_IDLE);//清除中断标志位
	 USART_ClearFlag(USART3,USART_FLAG_RXNE);
	 USART_ClearFlag(USART3,USART_FLAG_TC);
   USART_ITConfig(USART2,USART_IT_TC,DISABLE); 
   USART_ITConfig(USART2,USART_IT_RXNE,DISABLE); 
   USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);//打开空闲中断
   /*DMA USART TX */
   DMA_DeInit(DMA1_Channel7);
   DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART2->DR;
   DMA_InitStructure.DMA_MemoryBaseAddr = (u32)USART2_TX_BUF;
   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
   DMA_InitStructure.DMA_BufferSize = USART_TX_LEN;
   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
   DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
   DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
   DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
   DMA_Init(DMA1_Channel7,&DMA_InitStructure);
   DMA_ClearFlag(DMA1_FLAG_TC7);
   DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);
   DMA_Cmd(DMA1_Channel7,DISABLE);

   NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configMAX_SYSCALL_INTERRUPT_PRIORITY;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);

   /*DMA USART RX*/
   DMA_DeInit(DMA1_Channel6);
   DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART2->DR;
   DMA_InitStructure.DMA_MemoryBaseAddr = (u32 )USART2_RX_BUF;
   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
   DMA_InitStructure.DMA_BufferSize = USART_REC_LEN;
   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
   DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
   DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
   DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
   DMA_Init(DMA1_Channel6,&DMA_InitStructure);
   DMA_Cmd(DMA1_Channel6,ENABLE);

   USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);
   USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
   USART_Cmd(USART2, ENABLE);                    //使能串口
#endif
}

static void uart3_init(u32 bound,USARTReceiveIDLE_CB Callback)
{
#if DEUSART3
   GPIO_InitTypeDef GPIO_InitStructure;
   USART_InitTypeDef USART_InitStructure;
   NVIC_InitTypeDef NVIC_InitStructure;
   DMA_InitTypeDef DMA_InitStructure;
	 
	 USART3RxIDLE_CB = Callback;//保存回调
   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);

   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
   GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB.10
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
   GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB.11

   NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= configMAX_SYSCALL_INTERRUPT_PRIORITY;//抢占优先级
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级3
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
   NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

   USART_InitStructure.USART_BaudRate = bound;//串口波特率
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
   USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
   USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
   USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
   USART_Init(USART3, &USART_InitStructure); //初始化串口
   USART_ClearFlag(USART3,USART_FLAG_IDLE);//清除中断标志位
	 USART_ClearFlag(USART3,USART_FLAG_RXNE);
	 USART_ClearFlag(USART3,USART_FLAG_TC);
   USART_ITConfig(USART3,USART_IT_TC,DISABLE); 
   USART_ITConfig(USART3,USART_IT_RXNE,DISABLE); 
   USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);//空闲中断
   
   /*DMA USART TX */
   DMA_DeInit(DMA1_Channel2);
   DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART3->DR;
   DMA_InitStructure.DMA_MemoryBaseAddr = (u32)USART3_TX_BUF;
   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
   DMA_InitStructure.DMA_BufferSize = USART_TX_LEN;
   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
   DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
   DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
   DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
   DMA_Init(DMA1_Channel2,&DMA_InitStructure);
   DMA_ClearFlag(DMA1_FLAG_TC2);
   DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);
   DMA_Cmd(DMA1_Channel2,DISABLE);

   NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configMAX_SYSCALL_INTERRUPT_PRIORITY;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);

   /*DMA USART RX*/
   DMA_DeInit(DMA1_Channel3);
   DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART3->DR;
   DMA_InitStructure.DMA_MemoryBaseAddr = (u32 )USART3_RX_BUF;
   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
   DMA_InitStructure.DMA_BufferSize = USART_REC_LEN;
   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
   DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
   DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
   DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
   DMA_Init(DMA1_Channel3,&DMA_InitStructure);
   DMA_Cmd(DMA1_Channel3,ENABLE);

   USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
   USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);
   USART_Cmd(USART3, ENABLE);                    //使能串口
#endif
}

void HalUSARTReceiveCmd(USART_Port USARTx,FunctionalState NewState)
{
	switch (USARTx)
	{
		case HAL_USART_PORT1:
			#if DEUSART1
			DMA_Cmd(DMA1_Channel5,NewState);
			#endif
		break;
		case HAL_USART_PORT2:
			#if DEUSART2
			DMA_Cmd(DMA1_Channel6,NewState);
			#endif
		break;
		case HAL_USART_PORT3:
			#if DEUSART3
			DMA_Cmd(DMA1_Channel3,NewState);
			#endif
		break;
	}
}

void HalUSARTWriteData (USART_Port USARTx,u8 *Buff,u32 DataLen)
{
	switch (USARTx)
	{
		case HAL_USART_PORT1:
			#if DEUSART1
			while(USART1_TX_STA);
			USART1_TX_STA = ENABLE;
			DMA1_Channel4->CNDTR = DataLen;
			memcpy(USART1_TX_BUF,Buff,DataLen);
			DMA_Cmd(DMA1_Channel4,ENABLE);
			#endif
		break;
		case HAL_USART_PORT2:
			#if DEUSART2
			while(USART2_TX_STA);
			USART2_TX_STA = ENABLE;
			DMA1_Channel7->CNDTR = DataLen;
			memcpy(USART2_TX_BUF,Buff,DataLen);
			DMA_Cmd(DMA1_Channel7,ENABLE);
			#endif
		break;
		case HAL_USART_PORT3:
			#if DEUSART3
			while(USART3_TX_STA);
			USART3_TX_STA = ENABLE;
			DMA1_Channel2->CNDTR = DataLen;
			memcpy(USART3_TX_BUF,Buff,DataLen);
			DMA_Cmd(DMA1_Channel2,ENABLE);
			#endif
		break;
	}
}

void HalUSARTString(USART_Port USARTx,void *character)
{
   u8 DataLen = RESET;
   u8 *ptr = character;
   while(ptr[DataLen])DataLen++;
	 switch (USARTx)
	 {
			case HAL_USART_PORT1:
				#if DEUSART1
				while(USART1_TX_STA);
				USART1_TX_STA = 1;
				DMA1_Channel4->CNDTR = DataLen;
				memcpy(USART1_TX_BUF,ptr,DataLen);
				DMA_Cmd(DMA1_Channel4,ENABLE);
				#endif
			break;
			case HAL_USART_PORT2:
				#if DEUSART2
				while(USART2_TX_STA);
				USART2_TX_STA = ENABLE;
				DMA1_Channel7->CNDTR = DataLen;
				memcpy(USART2_TX_BUF,ptr,DataLen);
				DMA_Cmd(DMA1_Channel7,ENABLE);
				#endif
			break;
			case HAL_USART_PORT3:
				#if DEUSART3
				while(USART3_TX_STA);
				USART3_TX_STA = ENABLE;
				DMA1_Channel2->CNDTR = DataLen;
				memcpy(USART3_TX_BUF,ptr,DataLen);
				DMA_Cmd(DMA1_Channel2,ENABLE);
				#endif
			break;
	 }
}

#if DEUSART1
void DMA1_Channel4_IRQHandler(void)
{
	DMA_ClearFlag(DMA1_FLAG_TC4);//清除标志位
	DMA_Cmd(DMA1_Channel4,DISABLE);//关闭DMA发送
	USART1_TX_STA = 0;//清除标志位
}
#endif

#if DEUSART2
void DMA1_Channel7_IRQHandler(void)
{
	DMA_ClearFlag(DMA1_FLAG_TC7);//清除标志位
	DMA_Cmd(DMA1_Channel7,DISABLE);//关闭DMA发送
	USART2_TX_STA = RESET;//清除标志位
}
#endif

#if DEUSART3
void DMA1_Channel2_IRQHandler(void)
{
  DMA_ClearFlag(DMA1_FLAG_TC2);//清除标志位
  DMA_Cmd(DMA1_Channel2,DISABLE);//关闭DMA发送
  USART3_TX_STA = RESET;//清除标志位
}
#endif

#if DEUSART1
void USART1_IRQHandler(void)                	//串口1中断服务程序
{
   volatile u8 Clean;
   if(USART_GetITStatus(USART1,USART_IT_IDLE) != RESET) //空闲中断 (接收到一帧数据)
   {
     Clean = USART1->SR;
     Clean = USART1->DR;
     HalUSARTReceiveCmd(HAL_USART_PORT1,DISABLE);//关闭串口接收
     /*								用户数据处理								*/
     USART1_RX_STA = USART1_REC_LEN - DMA_GetCurrDataCounter(DMA1_Channel5); //接收到数据的长度
     if(USART1RxIDLE_CB)
			 USART1RxIDLE_CB(USART1_RX_BUF,USART1_RX_STA);
		 
     DMA1_Channel5->CNDTR = USART1_REC_LEN;
     USART_ClearITPendingBit(USART1,USART_IT_IDLE);
     USART_ClearITPendingBit(USART1,USART_IT_IDLE);
   }
   if(USART_GetITStatus(USART1,USART_IT_PE|USART_IT_FE|USART_IT_NE)!=RESET)//出错
   {
     USART_ClearITPendingBit(USART1,USART_IT_PE|USART_IT_FE|USART_IT_NE);
   }
   USART_ClearITPendingBit(USART1,USART_IT_TC);
} 
#endif

#if DEUSART2
void USART2_IRQHandler(void)                	//串口1中断服务程序
{
	 volatile u8 Clean;
   if(USART_GetITStatus(USART2,USART_IT_IDLE) != RESET) //空闲中断 (接收到一帧数据)
   {
     Clean = USART2->SR;
     Clean = USART2->DR;
     HalUSARTReceiveCmd(HAL_USART_PORT2,DISABLE);//关闭串口接收
     /*								用户数据处理								*/
     USART2_RX_STA = USART_REC_LEN - DMA_GetCurrDataCounter(DMA1_Channel6); //接收到数据的长度
     if(USART2RxIDLE_CB)
			 USART2RxIDLE_CB(USART2_RX_BUF,USART2_RX_STA);
		 
     DMA1_Channel6->CNDTR = USART_REC_LEN;
     HalUSARTReceiveCmd(HAL_USART_PORT2,ENABLE);//开启串口接收
     USART_ClearITPendingBit(USART2,USART_IT_IDLE);
   }
   if(USART_GetITStatus(USART2,USART_IT_PE|USART_IT_FE|USART_IT_NE)!=RESET)//出错
   {
     USART_ClearITPendingBit(USART2,USART_IT_PE|USART_IT_FE|USART_IT_NE);
   }
   USART_ClearITPendingBit(USART2,USART_IT_TC);
} 
#endif

#if DEUSART3
void USART3_IRQHandler(void)                	//串口1中断服务程序
{
   volatile u8 Clean;
   if(USART_GetITStatus(USART3,USART_IT_IDLE) != RESET) //空闲中断 (接收到一帧数据)
   {
     Clean = USART3->SR;
     Clean = USART3->DR;
     HalUSARTReceiveCmd(HAL_USART_PORT3,DISABLE);//关闭串口接收
     /*								用户数据处理								*/
     USART3_RX_STA = USART_REC_LEN - DMA_GetCurrDataCounter(DMA1_Channel3); //接收到数据的长度
     if(USART3RxIDLE_CB)
			 USART3RxIDLE_CB(USART3_RX_BUF,USART3_RX_STA);
		 
     DMA1_Channel3->CNDTR = USART_REC_LEN;
     HalUSARTReceiveCmd(HAL_USART_PORT3,ENABLE);//开启串口接收
		 USART_ClearITPendingBit(USART3,USART_IT_IDLE);
   }
	 if(USART_GetITStatus(USART3,USART_IT_RXNE) != RESET)//字节中断  接收到一个字节
	 {
		 USART3_RX_BUF[0] = USART_ReceiveData(USART3);
		 if(USART3RxIDLE_CB)
			 USART3RxIDLE_CB(USART3_RX_BUF,1);
		 USART_ClearITPendingBit(USART3,USART_IT_RXNE);
	 }
   if(USART_GetITStatus(USART3,USART_IT_PE|USART_IT_FE|USART_IT_NE)!=RESET)//出错
   {
     USART_ClearITPendingBit(USART3,USART_IT_PE|USART_IT_FE|USART_IT_NE);
   }
   USART_ClearITPendingBit(USART3,USART_IT_TC);
}
#endif


//JLINK CDC USART
#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))
#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))
#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))
#define DEMCR           (*((volatile unsigned long *)(0xE000EDFC)))
#define TRCENA          0x01000000

void JlinkUSART(char *fmt,...)
{
	//vTaskSuspendAll();
	char *string = Malloc(USART_TX_LEN);
	if(string != NULL)
	{
		u16 len;
		va_list ap;
		va_start(ap,fmt);  
		vsprintf(string,fmt,ap);  
		len = strlen(string);
		for(u16 i=0;i<len;i++)
		{
			if (DEMCR & TRCENA) {
		    while (ITM_Port32(0) == 0);
		    ITM_Port8(0) = string[i];
		  }
		}
		va_end(ap);  
		Free(string);
	}
	//xTaskResumeAll();
} 

int fputc(int ch, FILE *f)
{    
#if 0
	while((USART1->SR&0X40)==0){};//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;     
#endif
	return ch;
}

void MyPrintf(char *fmt,...)
{
	//vTaskSuspendAll();
	char *string = Malloc(USART_TX_LEN);
	if(string != NULL)
	{
		va_list ap;
		va_start(ap,fmt);  
		vsprintf(string,fmt,ap);  
		HalUSARTWriteData(HAL_USART_PORT1,(u8 *)string,strlen(string));  
		va_end(ap);  
		Free(string);
	}
	//xTaskResumeAll();
} 

//累加校验
u8 AccumulationCheck (u8 *Data,u8 Length)
{
   u8 i;
   u32 Check = RESET;
   for(i=RESET;i<Length;i++)
     Check += Data[i];
   return Check;
}

//异或校验
u8 XOR_Check(u8 *Data,u8 Length)
{
   u8 i,Check = RESET;
   Check = Data[0] ^ Data[1];
   for(i=2;i<Length;i++)
     Check ^= Data[i];
   return Check;
}

//虚拟打印 调试中查看str数组

#define WIDTH   64
#define HEIGTH	24

char str_sta = RESET;
char str[(WIDTH+1)*(HEIGTH+1)];
char str_x = RESET,str_y = RESET;

void strframe(u8 x,u8 y,u8 data)
{
	str[y*WIDTH+x] = data;
}

void PrintfString(char *fmt,...)  
{
	//vTaskSuspendAll();//挂起所有任务
	char *string = Malloc(512); 
	if(string != NULL)
	{
		va_list ap;  
		u16 len,i;	
		va_start(ap,fmt);  
		vsprintf(string,fmt,ap);  
		len = strlen(string);
		if(!str_sta)
		{
			memset(str,' ',sizeof(str));
			str_sta = SET;
		}
		for(i=0;i<len;i++)
		{
			if(string[i] == '\r')
			{
				str_x = RESET;
				str_y++;
			}
			else if(string[i] == '\n')
			{
				//str_y++;
			}
			else
			{
				if(str_y >= HEIGTH)
				{
					memcpy(str,&str[WIDTH],WIDTH*HEIGTH-WIDTH);
					memset(&str[WIDTH*HEIGTH-WIDTH],' ',WIDTH);
					str_y = HEIGTH-1;
				}
				strframe(str_x,str_y,string[i]);
				str_x++;
				if(str_x >= WIDTH)
				{
					str_y++;
					str_x = RESET;
				}
			}                                                                                 
		}
		va_end(ap);
		Free(string);
	}
	//xTaskResumeAll();//唤醒所有任务
} 

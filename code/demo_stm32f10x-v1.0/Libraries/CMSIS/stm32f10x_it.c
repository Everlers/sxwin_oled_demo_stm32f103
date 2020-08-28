/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.4.0
  * @date    10/15/2010
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}


//#include "led.h"
//栈数据定义 CPU寄存器在栈中的排列
typedef struct
{
	u32 R0;
	u32 R1;
	u32 R2;
	u32 R3;
	u32 R12;
	u32 LR;
	u32 PC;
	u32 xPSR;
}STACK_DATA_TYPE;

//获取当前CPU堆栈指针MSP
__asm u32 getMSP(void) 
{
  mrs r0, msp
	bx lr
}

//获取当前CPU堆栈指针PSP
__asm u32 getPSP(void) 
{
  mrs r0, psp
	bx lr
}

//没有操作系统时的异常处理
void NotOSHardFault_Handler(u32 msp_addr)
{
	STACK_DATA_TYPE *p;									//堆栈中存储的数据
	msp_addr -= 4;											//堆栈指针减去4，因为默认堆栈指针指向的是下一个空的地方-所以必须减去4
	if((msp_addr >> 20) == 0x200)		//判断地址范围，必须是0x200xxxxx 范围 否则堆栈指针被破坏，无法记录现场！
	{
		msp_addr += 8;											//进入中断后，堆栈又进入了2个u32数据，因此需要往后推
		p = (STACK_DATA_TYPE *)msp_addr;
		p->PC -= 3;													//PC指针要减去3
		if(p == (STACK_DATA_TYPE *)0)
			p = (STACK_DATA_TYPE *)0x20000000;
	}
	while(1);
}

//有操作系统时的异常处理
void OSHardFault_Handler(u32 psp_addr)
{
	STACK_DATA_TYPE *p;										//堆栈中存储的数据
	psp_addr -= 4;												//堆栈指针减去4，因为默认堆栈指针指向的是下一个空的地方-所以必须减去4
	if((psp_addr >> 20) != 0x200)		//判断地址范围，必须是0x200xxxxx 范围 否则堆栈指针被破坏，无法记录现场！
		return;
	p = (STACK_DATA_TYPE *)psp_addr;
	p->PC -= 3;												//PC指针要减去3
	if(p == (STACK_DATA_TYPE *)0)
		p = (STACK_DATA_TYPE *)0x20000000;
	while(p);
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
	u32 msp_addr = getMSP();		//获取线程模式下堆栈指针位置
	u32 psp_addr = getPSP();		//获取中断下的堆栈指针位置-用于OS启动后

	NVIC_SystemReset();
	//ledWarning();//LED警告
	//OSHardFault_Handler(psp_addr);//使能操作系统
	NotOSHardFault_Handler(msp_addr);	//没有使能操作系统
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
/*
void SVC_Handler(void)
{
}
*/

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
/*
void PendSV_Handler(void)
{
}
*/

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
/*
void SysTick_Handler(void)
{
}
*/

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/

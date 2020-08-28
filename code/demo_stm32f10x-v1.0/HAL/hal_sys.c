#include "hal_sys.h"

void HalSysInit (void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//配置0~15级的抢占式优先级 不需要子优先级
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//关闭JTAG
	DBGMCU->CR  &= ~((uint32_t)1<<5);//关闭异步跟踪(无法使用Jlink ITM)
}

void WFI_SET(void)
{
	__ASM volatile("wfi");		  
}
//关闭所有中断
void INTX_DISABLE(void)
{		  
	__ASM volatile("cpsid i");
}
//开启所有中断
void INTX_ENABLE(void)
{
	__ASM volatile("cpsie i");		  
}
/*
__asm void SystemReset(void)
{
	MOV R0, #1 //;
	MSR FAULTMASK, R0 //; ??FAULTMASK ????????
	LDR R0, =0xE000ED0C //;
	LDR R1, =0x05FA0004 //;
	STR R1, [R0] //; ??????
	deadloop
	B deadloop //; ???????????????
}*/
//设置栈顶地址
//addr:栈顶地址
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}

/*
 * FreeRTOS Kernel V10.1.1
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H
#include "rtosEx.h"
/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE. 
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

/*FreeRTOS基础配置配置选项*/
#define configUSE_PREEMPTION											1																//使能抢占式优先级 为1时RTOS使用抢占式调度器，为0时RTOS使用协作式调度器（时间片）。
#define configUSE_TIME_SLICING										1																//使能相同优先级的多个任务间进行任务切换
#define configUSE_IDLE_HOOK												0																//使用空闲任务的回调函数(打开后自动创建空闲任务最低优先级,释放分配给已删除任务的堆栈内存)
#define configUSE_TICK_HOOK												0																//使能滴答定时器终端里面的回调函数
#define configCPU_CLOCK_HZ												( ( unsigned long ) 72000000 )	//系统时钟 单位：Hz
#define configTICK_RATE_HZ												( ( TickType_t ) 1000 )					//滴答定时器速度 单位：Hz
#define configUSE_TICKLESS_IDLE                   0                       				//启用低功耗tickless模式
#define configMAX_PRIORITIES											( 5 )														//最大优先级
#define configMINIMAL_STACK_SIZE									( ( unsigned short ) 128 )			//最小堆大小(空闲任务的栈大小)
#define configTOTAL_HEAP_SIZE											( ( size_t ) ( 6 * 1024 ) )			//总堆栈大小
#define configMAX_TASK_NAME_LEN										( 12 )													//最大任务名长度
#define configUSE_16_BIT_TICKS										0																//针对8位16位处理器的选项 1:portTickType类型为16位 0：portTickType类型为32位
#define configIDLE_SHOULD_YIELD										0																//为1时空闲任务放弃CPU使用权给其他同优先级的用户任务
#define configUSE_TASK_NOTIFICATIONS            	1                       				//使用任务通知功能，默认开启
#define configUSE_NEWLIB_REENTRANT								0																//每一个创建的任务会分配一个newlib（一个嵌入式C库）reent结构
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS		0																//设置每个任务的线程本地存储指针数组大小

#define configUSE_MUTEXES													1																//使用互斥功能
#define configUSE_RECURSIVE_MUTEXES								1																//使用递归互斥功能
#define configUSE_COUNTING_SEMAPHORES							1																//使用计数器型信号量功能

#define configUSE_TIMERS													1																//使用系统软件定时器
#define configTIMER_TASK_PRIORITY									0																//设置软件定时器的优先级
#define configTIMER_QUEUE_LENGTH									10															//设置软件定时器任务的命令队列深度，设置定时器都是通过发送消息到该队列实现的。
#define configTIMER_TASK_STACK_DEPTH 							128															//设置软件定时器任务的栈大小

/*FreeRTOS扩展(调试配置)*/
//配置跟踪功能(显示任务执行状态信息)
#define configUSE_MALLOC_FAILED_HOOK							1																//RTOS内存分配失败后产生回调
#define configUSE_STATS_FORMATTING_FUNCTIONS			1																//用于使用@vTaskList 和 @vTaskGetRunTimeStats 函数(跟踪功能)
#define configUSE_TRACE_FACILITY									1																//跟踪功能(在使用IAR中的FreeRTOS插件时要使用到的配置,否则无法显示任务栈的使用情况)
#define configGENERATE_RUN_TIME_STATS							0																//用于查询任务RunTime时间 											使用@vTaskGetRunTimeStats 函数
#define configUSE_QUEUE_SETS                    	1                       				//启用队列集
#define configQUEUE_REGISTRY_SIZE                	0                       				//不为0时表示启用队列记录，具体的值是可以记录的队列和信号量最大数目 (除了进行内核调试外，队列记录没有其它任何目的)
#define configCHECK_FOR_STACK_OVERFLOW						2																//任务堆栈溢出检查 (两种检测方法如下)
/*方法一
在任务切换时检测任务栈指针是否过界了，如果过界了，在任务切换的时候会触发栈溢出钩子函
数。
void vApplicationStackOverflowHook( TaskHandle_t xTask,signed char *pcTaskName );(此函数在rtosEx.c实现)
用户可以在钩子函数里面做一些处理。这种方法不能保证所有的栈溢出都能检测到。比如任务在执行
的过程中出现过栈溢出。任务切换前栈指针又恢复到了正常水平，这种情况在任务切换的时候是检测
不到的。又比如任务栈溢出后，把这部分栈区的数据修改了，这部分栈区的数据不重要或者暂时没有
用到还好，但如果是重要数据被修改将直接导致系统进入硬件异常，这种情况下，栈溢出检测功能也
是检测不到的。
使用方法一需要用户在 FreeRTOSConfig.h 文件中配置如下宏定义：
#define configCHECK_FOR_STACK_OVERFLOW 1 

方法二
任务创建的时候将任务栈所有数据初始化为 0xa5，任务切换时进行任务栈检测的时候会检测末
尾的 16 个字节是否都是 0xa5，通过这种方式来检测任务栈是否溢出了。相比方法一，这种方法的速
度稍慢些，但是这样就有效地避免了方法一里面的部分情况。 不过依然不能保证所有的栈溢出都能检
测到，比如任务栈末尾的 16 个字节没有用到，即没有被修改，但是任务栈已经溢出了，这种情况是
检测不到的。 另外任务栈溢出后，任务栈末尾的 16 个字节没有修改，但是溢出部分的栈区数据被修
改了，这部分栈区的数据不重要或者暂时没有用到还好，但如果是重要数据被修改将直接导致系统进
入硬件异常，这种情况下，栈溢出检测功能也是检测不到的。
使用方法二需要用户在 FreeRTOSConfig.h 文件中配置如下宏定义：
#define configCHECK_FOR_STACK_OVERFLOW 2
*/

/*提供RunTime*/
#if configGENERATE_RUN_TIME_STATS == 1
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()	rtosExConfigureRunTime()				//用于配置RunTime时间（RunTime清零操作）使用@vTaskGetRunTimeStats 函数 配置一个高精度定时器/计数器提供时基
#define portGET_RUN_TIME_COUNTER_VALUE()					rtosExGetRunTimeCounterValue()	//用于查询RunTime时间 												使用@vTaskGetRunTimeStats 函数 读取时期的时间值
#endif

/* FreeRTOS与协程有关的配置选项 */
//注：在多任务管理机制上，操作系统可以分为抢占式和协作式两种。协作式操作系统是任务主动释放CPU后，切换到下一个任务。任务切换的时机完全取决于正在运行的任务。
#define configUSE_CO_ROUTINES 						0//启用协程，启用协程以后必须添加文件croutine.c
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )//协程的有效优先级数目

/*FreeRTOS可选函数配置选项 将下列定义设置为1，以包含API函数，或为零，以排除API函数。*/
#define INCLUDE_vTaskPrioritySet			1
#define INCLUDE_uxTaskPriorityGet			1
#define INCLUDE_vTaskDelete						1
#define INCLUDE_vTaskCleanUpResources	0
#define INCLUDE_vTaskSuspend					1
#define INCLUDE_vTaskDelayUntil				1
#define INCLUDE_vTaskDelay						1

/*FreeRTOS与中断有关的配置选项*/
//FreeRTOS系统的中断和滴答定时器的中断优先级都为最低(15)。
//受FreeRTOS管理的中断需要大于等于11的值，如果小于此值的优先级则不受RTOS影响，可以被抢占。
//故NVIC需要配置为 NVIC_PriorityGroup_4
/* This is the raw value as per the Cortex-M3 NVIC.  Values can be 255
(lowest) to 0 (1?) (highest). */
#define configKERNEL_INTERRUPT_PRIORITY 		255
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	191 /* equivalent to 0xBF, or priority 11. 也就是=0xBF，或者优先级11 这是系统调用终端优先级 RTOS临界区只屏蔽高于此优先级的任务*/

/* This is the value being used as per the ST library which permits 16
priority values, 0 to 15.  This must correspond to the
configKERNEL_INTERRUPT_PRIORITY setting.  Here 15 corresponds to the lowest
NVIC value of 255. */
#define configLIBRARY_KERNEL_INTERRUPT_PRIORITY	15

/* Map the FreeRTOS port interrupt handlers to their CMSIS standard names. */
#define xPortPendSVHandler                    PendSV_Handler
#define vPortSVCHandler                       SVC_Handler
#define xPortSysTickHandler                   SysTick_Handler

#endif /* FREERTOS_CONFIG_H */


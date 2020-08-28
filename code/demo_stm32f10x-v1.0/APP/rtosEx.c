#include "rtosEx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "string.h"
#include "stdio.h"
#include "hal_usart.h"

//任务状态 查看内存：stringTaskList
#if configGENERATE_RUN_TIME_STATS == 1
static u32  tsakRunTimeCount;//时间计数
#endif

#if configUSE_TRACE_FACILITY == 1
#if RTOSEX_LCD_SHOW == 1
#include "lcd_tft.h"
#include "ili9341.h"
#endif
char stringTaskList[RTOSEX_MAX_TASK_LIST_NUM+RTOSEX_SHOW_OTHER_INFO_NUM+1][configMAX_TASK_NAME_LEN + RTOSEX_TASK_LIST_PARAM_LEN]={0};
#endif

#if configUSE_TRACE_FACILITY == 1

//显示状态
void rtosShowStatus(void)
{
	char cStatus;
	static UBaseType_t oldArraySize = 0;
	UBaseType_t arraySize;
	u32 ulTotalTime,ulStatsAsPercentage;
	UBaseType_t alreadyArray[RTOSEX_MAX_TASK_LIST_NUM];//已经显示的序号
	char *string = pvPortMalloc(configMAX_TASK_NAME_LEN+RTOSEX_TASK_LIST_PARAM_LEN+1);
	TaskStatus_t *taskStatus = pvPortMalloc(sizeof(TaskStatus_t) * RTOSEX_MAX_TASK_LIST_NUM);
	if(string == NULL || taskStatus == NULL)return;
	
	arraySize = uxTaskGetSystemState(taskStatus,RTOSEX_MAX_TASK_LIST_NUM,&ulTotalTime);//获取任务状态
	ulTotalTime /= 100UL;//计算百分比使用率
	memset(alreadyArray,0,RTOSEX_MAX_TASK_LIST_NUM);//清除序号

	for(UBaseType_t i=0;i<arraySize;i++)
	{
		UBaseType_t minTaskNumber,index;
		minTaskNumber = 0xFFFF;
		for(UBaseType_t x=0;x<arraySize;x++)//寻找未显示且最小的任务序号给任务排序
		{
			UBaseType_t y;
			for(y=0;y<i;y++)//检查是否已经显示的数据
				if(x == alreadyArray[y])
					break;
			if(y == i)//如果未显示的数据
				minTaskNumber = MIN_VALUE(minTaskNumber,taskStatus[x].xTaskNumber);//取最小值
		}
		for(UBaseType_t x=0;x<arraySize;x++)//寻找最小任务序号的结构体
		{
			if(taskStatus[x].xTaskNumber == minTaskNumber)
			{
				alreadyArray[i] = x;
				index = x;
				break;
			}
		}
		
		switch( taskStatus[ index ].eCurrentState )//任务状态的显示转化
		{
			case eRunning:cStatus = tskRUNNING_CHAR;break;
			case eReady:cStatus = tskREADY_CHAR;break;
			case eBlocked:cStatus = tskBLOCKED_CHAR;break;
			case eSuspended:cStatus = tskSUSPENDED_CHAR;break;
			case eDeleted:cStatus = tskDELETED_CHAR;break;
			case eInvalid:		/* Fall through. */
			default:			/* Should not get here, but it is included
								to prevent static checking errors. */
				cStatus = ( char ) 0x00;
			break;
		}
		ulStatsAsPercentage = taskStatus[ index ].ulRunTimeCounter / ulTotalTime;//计算任务使用率
		memset(string,0,configMAX_TASK_NAME_LEN+RTOSEX_TASK_LIST_PARAM_LEN+1);
		vTaskSuspendAll();//挂起所有任务不可跳出
		snprintf(string,configMAX_TASK_NAME_LEN+RTOSEX_TASK_LIST_PARAM_LEN+1, "%c  %u %4u %2u %10u %3u%%"
		,cStatus
		, ( unsigned int ) taskStatus[ index ].uxCurrentPriority
		, ( unsigned int ) taskStatus[ index ].usStackHighWaterMark
		, ( unsigned int ) taskStatus[ index].xTaskNumber
		, ( unsigned int ) taskStatus[ index ].ulRunTimeCounter
		, ( unsigned int ) ulStatsAsPercentage);
		{//将任务列表显示到内存
			if(oldArraySize != arraySize)//如果任务数量发生改变
				memset(stringTaskList,' ',sizeof(stringTaskList));//清除显示内存的所有数据
			memset(stringTaskList[0],' ',configMAX_TASK_NAME_LEN/2-2);//清除name前面的内存
			memcpy(stringTaskList[0]+(configMAX_TASK_NAME_LEN/2)-2,"NAME",4);//写入name字符
			memset(stringTaskList[0]+(configMAX_TASK_NAME_LEN/2)+2,' ',configMAX_TASK_NAME_LEN - (configMAX_TASK_NAME_LEN/2)+2);//清除name后面的内存
			memcpy(stringTaskList[0]+configMAX_TASK_NAME_LEN-1,"STA PI RS  NB     RT      AP ",RTOSEX_TASK_LIST_PARAM_LEN);
			memcpy(stringTaskList[i+1],taskStatus[index].pcTaskName,strlen(taskStatus[index].pcTaskName));//复制任务名
			memset(stringTaskList[i+1]+strlen(taskStatus[index].pcTaskName),' ',configMAX_TASK_NAME_LEN - strlen(taskStatus[index].pcTaskName));//任务名后面补空格
			memcpy(&stringTaskList[i+1][configMAX_TASK_NAME_LEN],string,strlen(string));//复制任务信息
		}
		xTaskResumeAll();//唤醒所有任务
		#if RTOSEX_LCD_SHOW == 1
		{//将内存的任务列表显示到LCD
			if(oldArraySize != arraySize)//如果任务数量发生改变
				lcd_clear(0);
			memcpy(string,stringTaskList[0],sizeof(stringTaskList[0]));
			string[sizeof(stringTaskList[0])] = 0;
			lcd_show_string(0,0,string,WHITE,FILL_MODE);
			memcpy(string,stringTaskList[i+1],sizeof(stringTaskList[i+1]));
			string[sizeof(stringTaskList[i+1])] = 0;
			lcd_show_string(0,(i+1)*16,string,WHITE,FILL_MODE);
		}
		#endif
		oldArraySize = arraySize;//刷新任务数量
	}
	//显示其他信息
	memcpy(stringTaskList[arraySize+1],string,sprintf(string,"Other:"));
	memcpy(stringTaskList[arraySize+2],string,sprintf(string,"FreeHeapSize: %5u",( unsigned int )xPortGetFreeHeapSize()));//显示当前剩余栈
	memcpy(stringTaskList[arraySize+3],string,sprintf(string,"MinimumEverFreeHeapSize: %5u",( unsigned int )xPortGetMinimumEverFreeHeapSize()));//显示剩余栈历史最小值
	#if RTOSEX_LCD_SHOW == 1
	sprintf(string,"Other:");
	lcd_show_string(0,(arraySize+1)*16+8,string,WHITE,FILL_MODE);
	sprintf(string,"FreeHeapSize: %5u",( unsigned int )xPortGetFreeHeapSize());
	lcd_show_string(0,(arraySize+2)*16+8,string,WHITE,FILL_MODE);
	sprintf(string,"MinimumEverFreeHeapSize: %5u",( unsigned int )xPortGetMinimumEverFreeHeapSize());
	lcd_show_string(0,(arraySize+3)*16+8,string,WHITE,FILL_MODE);
	#endif
	vPortFree(taskStatus);
	vPortFree(string);
}

#endif

#if configGENERATE_RUN_TIME_STATS == 1

//用于使用@vTaskGetRunTimeStats 函数 配置一个高精度定时器/计数器提供时基
void rtosExConfigureRunTime (void)//50us计数
{
	TIM_TimeBaseInitTypeDef TIMInitstruct;
	NVIC_InitTypeDef NVICInitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	TIMInitstruct.TIM_Prescaler = 72;//分频
	TIMInitstruct.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	TIMInitstruct.TIM_Period = 50;//重载值
	TIMInitstruct.TIM_ClockDivision = TIM_CKD_DIV1;
	
	TIM_TimeBaseInit(TIM2,&TIMInitstruct);//初始化定时器
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);//使能定时器更新中断
	TIM_Cmd(TIM2,ENABLE);//使能定时器
	
	NVICInitStruct.NVIC_IRQChannel = TIM2_IRQn;
	NVICInitStruct.NVIC_IRQChannelPreemptionPriority = 0x02;//抢占优先级
	NVICInitStruct.NVIC_IRQChannelSubPriority = 0x02;//中断优先级
	NVICInitStruct.NVIC_IRQChannelCmd = ENABLE;//使能中断
	NVIC_Init(&NVICInitStruct);//初始化中断向量表
	tsakRunTimeCount = 0;
}

//用于使用@vTaskGetRunTimeStats 函数 读取时期的时间值
u32 rtosExGetRunTimeCounterValue (void)
{
	return tsakRunTimeCount;
}

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //溢出中断
		tsakRunTimeCount++;
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //清除中断标志位
}

#endif

//内存分配失败回调
void vApplicationMallocFailedHook (void)
{
	RTOSEX_DEBUG("Malloc failed !!! Lack of stack memory!\r\n");
}

//任务堆栈溢出回调
void vApplicationStackOverflowHook( TaskHandle_t xTask,signed char *pcTaskName )
{
	RTOSEX_DEBUG("Task stack overflow!!!\r\n");
}


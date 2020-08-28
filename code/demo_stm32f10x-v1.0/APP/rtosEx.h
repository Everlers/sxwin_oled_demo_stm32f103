#ifndef _RTOSEX_H_
#define _RTOSEX_H_
#include "hal_sys.h"

#define RTOSEX_DEBUG								debug	//RTOS扩展函数打印
#define RTOSEX_LCD_SHOW							0//用LCD显示任务列表
#define RTOSEX_MAX_TASK_LIST_NUM		10//最大任务列表的显示数量
#define RTOSEX_TASK_LIST_PARAM_LEN	28//任务列表的参数长度
#define RTOSEX_SHOW_OTHER_INFO_NUM	2	//显示其他信息的数量 需要添加n个行来显示其他的信息

#define tskRUNNING_CHAR		( 'X' )//运行
#define tskBLOCKED_CHAR		( 'B' )//阻塞
#define tskREADY_CHAR			( 'R' )//就绪
#define tskDELETED_CHAR		( 'D' )//删除
#define tskSUSPENDED_CHAR	( 'S' )//休眠

void rtosShowStatus(void);
void rtosExConfigureRunTime (void);
u32 rtosExGetRunTimeCounterValue (void);

#endif


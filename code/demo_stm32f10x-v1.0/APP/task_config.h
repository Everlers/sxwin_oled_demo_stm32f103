#ifndef _TASK_CONFIG_H_
#define _TASK_CONFIG_H_
#include "hal_sys.h"

#define FUN_TASK_STACK_SIZE				( configMINIMAL_STACK_SIZE + 50 )
#define FUN_TASK_PRIORITY					( tskIDLE_PRIORITY + 0 )

#define LCD_TASK_STACK_SIZE				( configMINIMAL_STACK_SIZE + 50 )
#define LCD_TASK_PRIORITY					( tskIDLE_PRIORITY + 0 )

#define LED_TASK_STACK_SIZE				( configMINIMAL_STACK_SIZE + 50 )
#define LED_TASK_PRIORITY					( tskIDLE_PRIORITY + 0 )

#endif

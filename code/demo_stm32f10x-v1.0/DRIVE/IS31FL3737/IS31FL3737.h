#ifndef _IS31FL3737_H_
#define _IS31FL3737_H_
#include "hal_sys.h"

#define IS_IO_SDB							PBout(5)

#define IS_DEV_ADDR						0xA0	//IS31FL3737 addr
#define IS_I2C_TIMEOUT				2			//2ms

//寄存器地址
#define IS_REGADDR_CMD				0xFD	//指令寄存器地址
#define IS_REGADDR_LOCK				0xFE	//解锁寄存器地址
#define IS_REGADDR_INT_MASK		0xF0	//中断屏蔽寄存器地址
#define IS_REGADDR_INT_STATE	0xF1	//中断状态寄存器地址

//IS_REGADDR_CMD + ↓
#define IS_CMD_LED_CTRL				0x00	//LED开关控制 		PG0
#define IS_CMD_PWM_DUTY				0x01	//PWM占空比设置		PG1
#define IS_CMD_AUTO_BREATH		0x02	//自动呼吸模式		PG2
#define IS_CMD_FUN_CONFIG			0x03	//功能寄存器配置	PG3

//IS_REGADDR_LOCK + ↓
#define IS_UNLOCK							0xC5	//解锁指令寄存器
#define IS_LOCK								0x00	//锁定指令寄存器

//IS_REGADDR_INT_MASK + ↓
#define IS_INTE_IAC						0x08	//自动清除中断使能
#define IS_INTE_IAB						0x04	//自动呼吸中断使能
#define IS_INTE_IS						0x02	//点短中断标使能
#define IS_INTE_IO						0x01	//点打开的中断使能

//IS_REGADDR_INT_STATE + ↓
#define IS_INTF_ABM3					0x10	//自动模式呼吸3完成标志位
#define IS_INTF_ABM2					0x08	//自动模式呼吸2完成标志位
#define IS_INTF_ABM1					0x04	//自动模式呼吸1完成标志位
#define IS_INTF_SB						0x02	//短路标志位
#define	IS_INTF_OB						0x01	//开路标志位

//IS_REGADDR_CMD + IS_CMD_LED_CTRL + ↓
#define IS_LED_CTRL_ON_OFF_ADDR		0x00	//设置每个灯的开关0x00~0x17 1:打开LED 0:关闭LED 0~5bit
#define IS_LED_CTRL_OPEN_ADRR			0x18	//灯是否打开状态0x10~0x2F	1:短路 0:正常
#define IS_LED_CTRL_SHORT_ADDR		0x30	//灯是否短路状态0x30~0x47 1:开路 0:正常

//IS_REGADDR_CMD + IS_CMD_PWM_DUTY + ↓
#define IS_PWM_DUTY_ADDR					0x00	//设置每个灯的PWM占空比0x00~0xBD Byte:0~255调节占空比

//IS_REGADDR_CMD + IS_CMD_AUTO_BREATH + ↓
#define IS_AUTO_MODE_ADDR					0x00	//设置每个灯的模式0x00~0xBD	Byte:0~2三种模式

//IS_REGADDR_CMD + IS_CMD_FUN_CONFIG + ↓
#define IS_FUN_OPRATION_MODE_ADDR	0x00	//设置操作模式	1Byte 配置见 @FunConfig
#define IS_FUN_GLOBAL_CUR_ADDR		0x01	//设置全局电流 	1Byte 计算公式：IS_FUN_GLOBAL_CUR_ADDR * 840/20K(REXT_IO) (例如:IOUT = (128(cur_reg)/256) * (840/20K(REXT_IO)) = 21mA)

#define IS_FUN_SET_FADE_IN_ABM1			0x02	//设置淡入和保持时间 模式1	
#define IS_FUN_SET_FADE_OUT_ABM1		0x03	//设置淡出和保持时间 模式1
#define IS_FUN_SET_LOOP1_ABM1				0x04	//设置循环1 模式1
#define IS_FUN_SET_LOOP2_ABM1				0x05	//设置循环2 模式1

#define IS_FUN_SET_FADE_IN_ABM2			0x06	//设置淡入和保持时间 模式1
#define IS_FUN_SET_FADE_OUT_ABM2		0x07	//设置淡出和保持时间 模式1
#define IS_FUN_SET_LOOP1_ABM2				0x08	//设置循环1 模式1
#define IS_FUN_SET_LOOP2_ABM2				0x09	//设置循环2 模式1

#define IS_FUN_SET_FADE_IN_ABM3			0x0A	//设置淡入和保持时间 模式1
#define IS_FUN_SET_FADE_OUT_ABM3		0x0B	//设置淡出和保持时间 模式1
#define IS_FUN_SET_LOOP1_ABM3				0x0C	//设置循环1 模式1
#define IS_FUN_SET_LOOP2_ABM3				0x0D	//设置循环2 模式1

#define IS_FUN_SET_ABM_UPDATE_TIME	0x0E	//设置0x02~0x0D的更新时间 1Byte
#define IS_FUN_SWy_PULL_UP_SEL			0x0F	//SWy上拉的选择
#define IS_FUN_CSx_PULL_DOWN_SEL		0x10	//CSx下拉的	选择
#define IS_FUN_RESET_REG_ADDR				0x11	//将所有寄存器重置为POR状态 (对此寄存器进行读取操作即可重置所有寄存器的值)

//IS_REGADDR_CMD + IS_CMD_FUN_CONFIG + IS_FUN_OPRATION_MODE_ADDR + ↓    @FunConfig
#define IS_OPRATION_MODE_SYNC_HI			0x00	//(同步配置)高阻抗
#define IS_OPRATION_MODE_SYNC_MASTER	0x40	//(同步配置)主
#define IS_OPRATION_MODE_SYNC_SLAVE		0x80	//(同步配置)从
#define IS_OPRATION_MODE_OSD_EN				0x04	//(开路/短路检测)使能
#define IS_OPRATION_MODE_B_EN					0x02	//(自动呼吸)使能
#define IS_OPRATION_MODE_SSD					0x01	//1:正常操作 0:软件关机

//IS_REGADDR_CMD + IS_CMD_FUN_CONFIG + IS_FUN_SWy_PULL_UP_SEL/IS_FUN_CSx_PULL_DOWN_SEL + ↓
#define IS_SWy_PULL_UP_DOWN_NOT				0x00	//不接上拉
#define IS_SWy_PULL_UP_DOWN_0_5K			0x01	//0.5K
#define IS_SWy_PULL_UP_DOWN_1K				0x02	//1.0K
#define IS_SWy_PULL_UP_DOWN_2K				0x03	//2.0K
#define IS_SWy_PULL_UP_DOWN_4K				0x04	//4.0K
#define IS_SWy_PULL_UP_DOWN_8K				0x05	//8.0K
#define IS_SWy_PULL_UP_DOWN_16K				0x06	//16.0K
#define IS_SWy_PULL_UP_DOWN_32K				0x07	//32.0K

//参数配置
#define IS_REXT_IO_RES_VALUE					20		//设定REXT引脚电阻值 单位：K欧姆
#define IS_GLOBAL_CUR_VALUE						42		//设定全局电流值 单位：mA

typedef enum{
	I2C_NOT_ERR = 0,
	I2C_HW_BERR,
	I2C_HW_ARLO,
	I2C_HW_AF,
	I2C_HW_OVR,
	I2C_HW_PECERR,
	I2C_HW_TIMEOUT,
	I2C_HW_SMBALERT,
}I2CError_t;

typedef enum{
	I2C_IDLE = 0,
	I2C_START,
	I2C_ADDR,
	I2C_DATA,
}I2CState_t;

typedef enum{
	I2C_WRITE = 0,
	I2C_READ = 1,
}I2CDir_t;

typedef enum{
	IS_LED_OPEN = 1,
	IS_LED_CLOSE = 0,
}isLedSwitch;

typedef enum{
	IS_AUTO_UPDATE = 1,
	IS_MANUAL_UPDATE = 0,
}isAutoUpdate;

typedef struct{
	I2CError_t I2CError;
	I2CState_t I2CState;
	u16 timeout;
	isAutoUpdate isUpdate;
	
	I2CDir_t dir;		//方向
	u8	addr;				//地址
	u8 	*data;			//数据
	u8 	len;				//长度
	u8 	index;     	//当前位置
	u8 	autoIndex;
}is31fl3737_t;

extern u8 isLedDuty[144];

void ISInit(void);
void ISCmdChange(u8 cmd);
void ISUpdateLedDuty(void);
void ISUpdateLedMode(void);
void ISSetLedDuty(u8 x,u8 y,u8 duty);
#endif

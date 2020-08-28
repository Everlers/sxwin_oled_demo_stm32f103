#include "My_FAT.h"
#include "sdio_sdcard.h"
#include "hal_usart.h"
#include "stdlib.h"
#include "delay.h"

static void SD_Card_Printf_Info(void);

static SDCardExintCB SDCardCB;//内存卡被插拔回调
FATFS SD_FATFS;//FAT文件系统
profile_t default_profile={
PROFILE_EXPLAIN,
"GPS=0\r\n",
"GPRS=1\r\n",
};//如果没有配置文件 默认的配置

profile_t *get_default_profile (void)
{
	return &default_profile;
}

//读取配置文件参数
FRESULT ReadProfileParam(profile_t *profile_s)
{
	FIL profile;
	UINT len;
	
	if(profile_s == NULL)
		return FR_INVALID_PARAMETER;
	if(f_open(&profile,"profile.txt",FA_OPEN_EXISTING|FA_READ) != FR_OK)//打开配置文件 如果不成功
	{
		if(f_open(&profile,"profile.txt",FA_OPEN_ALWAYS|FA_WRITE) != FR_OK)//打开配置文件如果没有则创建个新的文件
			return FR_NO_FILE;//创建文件失败没有此文件
		if(f_write(&profile,&default_profile,sizeof(profile_t),&len) != FR_OK)//写入默认的配置文件
			return FR_NO_FILE;
		f_close(&profile);//关闭并保存
		if(f_open(&profile,"profile.txt",FA_OPEN_EXISTING|FA_READ) != FR_OK)//重新打开
			return FR_NO_FILE;
	}
	if(f_read(&profile,profile_s,sizeof(profile_t),&len) != FR_OK)//读取数据
		return FR_NO_FILE;
	f_close(&profile);
	return FR_OK;
}

FRESULT Mf_Mount(void)
{
	FRESULT res;
	if(disk_initialize(0)){//初始化SD卡通讯
		//MyPrintf("SD init error\r\n");
		return res = FR_INVALID_DRIVE;
	}
	else
		SD_Card_Printf_Info();//打印SD卡信息
	if((res = f_mount(&SD_FATFS,"0:",0)) != FR_OK)//挂载SD卡
		return res;
		//MyPrintf("SD mount error\r\n");
	//else
		//MyPrintf("SD mount OK\r\n");
	return res;
}

//加载所有音乐
//参数 	path 路径  
//			chr 文件名保存的位置 
//			suffix只保存此后缀文件 
//			maxfnl 最大文件名长度
//			maxfn最大保存数量
//返回值 读取到的文件总数 
u16 LoadAllMusic(char *path,char **chr,char *suffix,u16 maxfnl,u16 maxfn)
{
	DIR dir;
	FILINFO fileinfo;
	FRESULT res;
	u8 i,num = 0;
	fileinfo.lfname = malloc(_MAX_LFN);
	res = f_opendir(&dir,path);
	if(res != FR_OK)
			return 0;
	for(i=0;i<maxfn;i++)
	{
		res = Mf_ReadDir(&dir,&fileinfo);
		if(res != FR_OK)
			break;
		//if(fileinfo.lfname[0] == 0)
		//	break;
		if(strstr(fileinfo.lfname,suffix) != NULL)
		{
			if(chr[num] == RESET)
			{
				chr[num] = malloc(strlen(fileinfo.lfname)+1);
			}
			if(chr[num] == 0)return i;//内存分配错误 直接返回已经读取到的文件
			if(strlen(fileinfo.lfname) > maxfnl)return i;//文件名过长 直接返回已经读取到的文件
			memcpy(chr[num],fileinfo.lfname,strlen(fileinfo.lfname));
			chr[num][strlen(fileinfo.lfname)] = '\0';
			num ++;
		}
	}
	return num;
}
//内存卡插入外部中断
void SDCardInsertExintRegister(SDCardExintCB CB)
{
	GPIO_InitTypeDef GPIO_InitStrict;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_InitStrict.GPIO_Pin = SD_CD_BIT;
	GPIO_InitStrict.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStrict.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SD_CD_PORT,&GPIO_InitStrict);//初始化SD卡的CD引脚
	
	EXTI_ClearITPendingBit(EXTI_Line15);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource15);
	EXTI_InitStructure.EXTI_Line = EXTI_Line15;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;//上下沿中断
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;                     
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);
	
	SDCardCB = CB;
}

//SD卡CD引脚发生改变 内存卡被拔出或插入
void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line15) != RESET)  
	{
		delay_ms(20);
		if(SDCardCB)
		{
			if(SD_CD)
				SDCardCB(SD_NULL);//内存卡被拔出
			else
				SDCardCB(SD_VALID);//内存卡插入
		}
		EXTI_ClearITPendingBit(EXTI_Line15);
	}
}

//打读取文件夹
//返回值:执行结果
FRESULT Mf_ReadDir(DIR *dir,FILINFO *fileinfo)
{
	FRESULT res;
 	fileinfo->lfsize = _MAX_LFN;
	//fileinfo->lfname = Malloc(fileinfo->lfsize);	//分配内存  
	if(fileinfo->lfname == NULL)
		return FR_NOT_ENOUGH_CORE;
	res = f_readdir(dir,fileinfo);//读取一个文件的信息
	fileinfo->lfname=(fileinfo->lfname[0]!=0)? fileinfo->lfname:fileinfo->fname;//选择长名字或者短的名字
	return res;
}

/**********************************************************
* 函数功能 ---> 打印SD卡信息到串口
* 入口参数 ---> none
* 返回数值 ---> none
* 功能说明 ---> 1、打印卡的容量到串口
*               2、打印卡的类型到串口
*               3、打印卡的其他信息
**********************************************************/
static void SD_Card_Printf_Info(void)
{
	switch(SDCardInfo.CardType)	//卡类型
	{
		case SDIO_HIGH_CAPACITY_SD_CARD:	//高容量卡
			//MyPrintf("Card Type: SDHC V2.0\r\n");
			break;
			
		case SDIO_STD_CAPACITY_SD_CARD_V1_1:	//标准容量V1.1
			//MyPrintf("Card Type: SDSC V1.1\r\n");
			break;
			
		case SDIO_STD_CAPACITY_SD_CARD_V2_0:	//标准容量V2.0
			//MyPrintf("Card Type: SDSC V2.0\r\n");
			break;

		case SDIO_MULTIMEDIA_CARD:	//MMC卡
			//MyPrintf("Card Type: MMC Card\r\n");
			break;
	}

	//MyPrintf("Card ManufacturerID: %d\r\n",SDCardInfo.SD_cid.ManufacturerID);			//制造商ID
 	//MyPrintf("Card RCA: %d\r\n",SDCardInfo.RCA);										//卡相对地址
	//MyPrintf("Card Capacity: %d MB\r\n",(uint32_t)SDCardInfo.CardCapacity);	//显示容量
 	//MyPrintf("Card BlockSize: %d\r\n",SDCardInfo.CardBlockSize);					//显示块大小
}

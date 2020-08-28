#include "hal_flash.h"
#include "FreeRTOS.h"

#if HAL_FLASH_FREE_RTOS
u8 *FlashPageTmpData;
#else
u8 FlashPageTmpData[FLASH_PAGE_SIZE];
#endif

void HalFlashRead(u32 FlashAddr,u8 *ReadData,u32 ReadLen)
{
	for(u32 i=0;i<ReadLen;i++){
		ReadData[i] = ((u8 *)FlashAddr)[i];
	}
}

HalFlashErrorCode HalFlashWrite(u32 FlashAddr,u8 *WriteData,u16 WriteLen)
{
	volatile u32 secpos = FlashAddr & (~(FLASH_PAGE_SIZE -1 ));//扇区地址 
	volatile u16 secoff = FlashAddr & (FLASH_PAGE_SIZE -1);    //在扇区内的偏移
	#if HAL_FLASH_FREE_RTOS
		FlashPageTmpData = pvPortMalloc(FLASH_PAGE_SIZE);
		if(FlashPageTmpData == NULL)return FlashWriteError;
	#endif
	FLASH_Unlock();
		for(u16 i=0;i<FLASH_PAGE_SIZE;i++)//读出整个扇区
			FlashPageTmpData[i] = ((u8 *)secpos)[i];
		
		if(FLASH_COMPLETE != FLASH_ErasePage(secpos)){//擦除扇区
			FLASH_Lock();
			#if HAL_FLASH_FREE_RTOS
				vPortFree(FlashPageTmpData);
			#endif
			return FlashEraseError;
		}
		
		for(u16 i=0;i<WriteLen;i++){//复制修改扇区数据
			FlashPageTmpData[secoff+i] = WriteData[i];
		}
		
		for(u16 i=0;i<FLASH_PAGE_SIZE/4;i++){//写入整个扇区
			if(FLASH_ProgramWord(secpos+(i*4),((u32 *)FlashPageTmpData)[i]) != FLASH_COMPLETE){
				FLASH_Lock();
				#if HAL_FLASH_FREE_RTOS
					vPortFree(FlashPageTmpData);
				#endif
				return FlashWriteError;
			}
		}
	FLASH_Lock();
	#if HAL_FLASH_FREE_RTOS
		vPortFree(FlashPageTmpData);
	#endif
	return FlashDone;
}

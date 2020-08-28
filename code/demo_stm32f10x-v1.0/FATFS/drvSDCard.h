#ifndef DRV_SDCARD_H001
#define DRV_SDCARD_H001

#include "stm32f10x.h"

#define SD_DETECT_PIN                    GPIO_Pin_11                 /* PF.11 */
#define SD_DETECT_GPIO_PORT              GPIOF                       /* GPIOF */
#define SD_DETECT_GPIO_CLK               RCC_APB2Periph_GPIOF

#define SDIO_FIFO_ADDRESS                ((uint32_t)0x40018080)
/** 
  * @brief  SDIO Intialization Frequency (400KHz max)=72M/(0xb2+2)
  */
#define SDIO_INIT_CLK_DIV                ((uint8_t)0xc0)//200k
/** 
  * @brief  SDIO Data Transfer Frequency (25MHz max) 
  */
#define SDIO_TRANSFER_CLK_DIV            ((uint8_t)0x2) 

void drvSDCardConfig(void);
void SD_LowLevel_DeInit(void);
void SD_LowLevel_Init(void);
void SD_LowLevel_DMA_TxConfig(uint32_t *BufferSRC, uint32_t BufferSize);
void SD_LowLevel_DMA_RxConfig(uint32_t *BufferDST, uint32_t BufferSize);
uint32_t SD_DMAEndOfTransferStatus(void);


#endif

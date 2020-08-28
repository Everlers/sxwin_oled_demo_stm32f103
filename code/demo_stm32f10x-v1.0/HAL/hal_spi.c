#include "hal_spi.h"

void HalSPIInit(SPI_Select SPIx, CPOL_Select Cpol, CPHA_Select Cpha,Baud_Select Baud)
{
  SPI_InitTypeDef  SPI_InitStructure;

	if(SPIx == HAL_SPI_1)
	{
		RCC_APB2PeriphClockCmd(	RCC_APB2Periph_SPI1,  ENABLE );//SPI1时钟使能 	
		
		SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
		SPI_InitStructure.SPI_CPOL = Cpol;		//串行同步时钟的空闲状态为高电平
		SPI_InitStructure.SPI_CPHA = Cpha;	//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
		SPI_InitStructure.SPI_BaudRatePrescaler = Baud;		//定义波特率预分频的值:波特率预分频值为256
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
		SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
		SPI_Init(SPI1, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
	 
		SPI_Cmd(SPI1, ENABLE); //使能SPI外设
		
	}
	else if(SPIx == HAL_SPI_2)
	{
		RCC_APB1PeriphClockCmd(	RCC_APB1Periph_SPI2,  ENABLE );//SPI2时钟使能 	

		SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
		SPI_InitStructure.SPI_CPOL = Cpol;		//串行同步时钟的空闲状态为高电平
		SPI_InitStructure.SPI_CPHA = Cpha;	//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
		SPI_InitStructure.SPI_BaudRatePrescaler = Baud;		//定义波特率预分频的值:波特率预分频值为256
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
		SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
		SPI_Init(SPI2, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
	 
		SPI_Cmd(SPI2, ENABLE); //使能SPI外设
		
	}
}

void HalSPIDataSize(SPI_TypeDef *SPIx,u16 data_size)
{
	SPI_DataSizeConfig(SPIx,data_size);
}

//SPI 速度设置函数
//SpeedSet:
//SPI_BaudRatePrescaler_2   2分频   
//SPI_BaudRatePrescaler_8   8分频   
//SPI_BaudRatePrescaler_16  16分频  
//SPI_BaudRatePrescaler_256 256分频 
void HalSPISetSpeed(SPI_Select SPIx,Baud_Select SPI_BaudRatePrescaler)
{
	if(SPIx == HAL_SPI_1)
	{
		assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
		SPI1->CR1&=0XFFC7;
		SPI1->CR1|=SPI_BaudRatePrescaler;	//设置SPI2速度 
		SPI_Cmd(SPI1,ENABLE); 
	}
	else if(SPIx == HAL_SPI_2)
	{
		assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
		SPI2->CR1&=0XFFC7;
		SPI2->CR1|=SPI_BaudRatePrescaler;	//设置SPI2速度 
		SPI_Cmd(SPI2,ENABLE); 
	}
}

SPI_Error HalSPIWriteByte(SPI_Select SPIx,u16 TxData)
{
	u16 retry = RESET;
	if(SPIx == HAL_SPI_1)
	{
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
		{
			retry++;
			if(retry>2000)return HAL_SPI_ERROR;
		}
		SPI_I2S_SendData(SPI1, TxData); //通过外设SPIx发送一个数据
	}
	else if(SPIx == HAL_SPI_2)
	{
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
		{
			retry++;
			if(retry>2000)return HAL_SPI_ERROR;
		}
		SPI_I2S_SendData(SPI2, TxData); //通过外设SPIx发送一个数据
	}
	return HAL_SPI_DONE;
}

SPI_Error HalSPIReadByte(SPI_Select SPIx,u8 *RxData)
{
	u16 retry = RESET;
	if(SPIx == HAL_SPI_1)
	{
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)//检查指定的SPI标志位设置与否:接受缓存非空标志位
		{
			retry++;
			if(retry>2000)return HAL_SPI_ERROR;
		}	  						    
		*RxData = SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据	
	}
	else if(SPIx == HAL_SPI_2)
	{
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)//检查指定的SPI标志位设置与否:接受缓存非空标志位
		{
			retry++;
			if(retry>2000)return HAL_SPI_ERROR;
		}	  						    
		*RxData = SPI_I2S_ReceiveData(SPI2); //返回通过SPIx最近接收的数据	
	}
	return HAL_SPI_DONE;
}

//SPIx 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 HalSPIReadWriteByte(SPI_Select SPIx,u8 TxData)
{
	u16 retry;
	if(SPIx == HAL_SPI_1)
	{
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
		{
			retry++;
			if(retry>2000)return 0;
		}
		SPI_I2S_SendData(SPI1, TxData); //通过外设SPIx发送一个数据
		retry=0;

		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)//检查指定的SPI标志位设置与否:接受缓存非空标志位
		{
			retry++;
			if(retry>2000)return 0;
		}	  						    
		return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据	
	}
	else if(SPIx == HAL_SPI_2)
	{
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
		{
			retry++;
			if(retry>2000)return 0;
		}
		SPI_I2S_SendData(SPI2, TxData); //通过外设SPIx发送一个数据
		retry=0;

		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)//检查指定的SPI标志位设置与否:接受缓存非空标志位
		{
			retry++;
			if(retry>2000)return 0;
		}	  						    
		return SPI_I2S_ReceiveData(SPI2); //返回通过SPIx最近接收的数据	
	}		
	return 0;
}
//返回发送的数据长度
u8 HalSPIWrite(SPI_Select SPIx,u8 *TxData,u8 Len)
{
	for(u8 i=0;i<Len;i++)
	{
		if(HalSPIWriteByte(SPIx,TxData[i]) == HAL_SPI_ERROR)
			return i+1;
	}
	return Len;
}

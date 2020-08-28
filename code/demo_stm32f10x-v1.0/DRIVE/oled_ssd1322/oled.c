#include "oled.h"
#include "font.c"
#include "hal_spi.h"
#include "delay.h"

//SSD1322特性
//每个字节包含两个像素
//每次刷新四个像素(两个字节)
//每字节的高四位在前像素
u16 oledShowBuff[OLED_WIDTH/4][OLED_HIGH];//显示缓冲(两个像素一个字节高四位在先)

static void writeByte(u8 dat,oledCmd_t cmd);

void oledInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef  SPI_InitStructure;
	DMA_InitTypeDef DMA_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE );//SPI1时钟使能
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;//串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;//定义波特率预分频的值:波特率预分频值
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;//CRC值计算的多项式
	SPI_Init(SPI1, &SPI_InitStructure);//根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
	SPI_Cmd(SPI1, ENABLE); //使能SPI外设
	
	OLED_RST = 0;
	delay_ms(100);
	OLED_RST = 1;
	OLED_CS = 0;
	
	writeByte(0xfd,OLED_CMD);  /*Command Lock*/ 
	writeByte(0x12,OLED_DAT);
	
	writeByte(0xae,OLED_CMD); //Sleep In 
	
	writeByte(0xb3,OLED_CMD);	//Set Display Clock Divide Ratio/Oscillator Frequency 
	writeByte(0x91,OLED_DAT);
	
	writeByte(0xca,OLED_CMD);	//Set Multiplex Ratio 
	writeByte(0x3f,OLED_DAT);
	
	writeByte(0xa2,OLED_CMD);	//Set Display Offset 
	writeByte(0x00,OLED_DAT);	//
	
	writeByte(0xa1,OLED_CMD);	//Set Display Start Line 
	writeByte(0x00,OLED_DAT);	//
	
	writeByte(0xa0,OLED_CMD);	//Set Re-Map $ Dual COM Line Mode
	writeByte(0x15,OLED_DAT);	//
	
	
	writeByte(0xB5,OLED_CMD);	//Set GPIO 
	writeByte(0x00,OLED_DAT);
		
	writeByte(0xab,OLED_CMD);	//Function Selection
	writeByte(0x01,OLED_DAT);	//
	
	writeByte(0xb4,OLED_CMD);	//Enable External VSL 
	writeByte(0xa0,OLED_DAT);	//
	writeByte(0xfd,OLED_DAT);	//
	
	writeByte(0xc1,OLED_CMD);	//Set Contrast Current
	writeByte(0xff,OLED_DAT);	
	
	writeByte(0xc7,OLED_CMD);	//Master Contrast Current Control
	writeByte(0x0f,OLED_DAT);	//

	writeByte(0xb9,OLED_CMD);	//Select Default Linear Gray Scale Table 
	
	writeByte(0xb1,OLED_CMD);	//Set Phase Length
	writeByte(0xe2,OLED_DAT);	
	
	writeByte(0xd1,OLED_CMD);	//Enhance Driving Scheme Capability 
	writeByte(0x82,OLED_DAT);	
	writeByte(0x20,OLED_DAT);	

	writeByte(0xbb,OLED_CMD);	//Set Pre-Charge Voltage 
	writeByte(0x1f,OLED_DAT);	

	writeByte(0xb6,OLED_CMD);	//Set Second Pre-Charge Period 
	writeByte(0x08,OLED_DAT);	

	writeByte(0xbe,OLED_CMD);	//Set VCOMH Deselect Level 
	writeByte(0x07,OLED_DAT);		

	writeByte(0xa6,OLED_CMD);	//Set Display Mode
	//Clear Screen

	writeByte(0xaf,OLED_CMD);	//Sleep Out
	
	oledClear();
	oledSetPos(0,0,63,63);
	OLED_DC = 1;
	
	/*writeByte(0xFA,OLED_DAT);
	writeByte(0x73,OLED_DAT);
	while(1);*/
	//SPI_DataSizeConfig(SPI1,SPI_DataSize_16b);//将SPiI配置成16位数据模式
	
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;//正常模式(单次触发)
	DMA_InitStruct.DMA_BufferSize = sizeof(oledShowBuff);//缓冲大小
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;//外设做为传输目标
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;//不开启内存到内存
	DMA_InitStruct.DMA_MemoryBaseAddr = (u32)oledShowBuff;//显存地址
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//内存半个机器字为单位
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;//内存递增
	DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)&SPI1->DR;//SPI外设地址
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设半个机器字为单位
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址不递增
	DMA_InitStruct.DMA_Priority = DMA_Priority_VeryHigh;//优先权非常高
	DMA_Init(DMA1_Channel3,&DMA_InitStruct);//初始化DMA
	SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Tx,ENABLE);//使能DMA SPI发送
	DMA_ClearFlag(DMA1_FLAG_TC3);
	DMA_ITConfig(DMA1_Channel3,DMA_IT_TC,ENABLE);
	DMA_ITConfig(DMA1_Channel3,DMA_IT_HT,DISABLE);
	DMA_ITConfig(DMA1_Channel3,DMA_IT_TE,DISABLE);
	
	NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStruct);
	
	DMA_SetCurrDataCounter(DMA1_Channel3,sizeof(oledShowBuff));//设定传输长度
	DMA_Cmd(DMA1_Channel3,ENABLE);//开始DMA1通道3
	while(DMA_GetCurrDataCounter(DMA1_Channel3));//等待SPI传输完成
}

void DMA1_Channel3_IRQHandler (void)
{
	if(DMA_GetFlagStatus(DMA1_FLAG_TC3) != RESET)
	{
		DMA_ClearFlag(DMA1_FLAG_TC3);
		DMA_Cmd(DMA1_Channel3,DISABLE);
	}
	if(DMA_GetFlagStatus(DMA1_FLAG_TE3) != RESET)
	{
		DMA_ClearFlag(DMA1_FLAG_TE3);
	}
	if(DMA_GetFlagStatus(DMA1_FLAG_HT3) != RESET)
	{
		DMA_ClearFlag(DMA1_FLAG_HT3);
	}
}

void oledUpdateBuffToShow(void)
{
	while(DMA_GetCurrDataCounter(DMA1_Channel3));//等待SPI传输完成
	DMA_Cmd(DMA1_Channel3,DISABLE);
	DMA1_Channel3->CMAR = (u32)oledShowBuff;//设定传输缓冲
	DMA_SetCurrDataCounter(DMA1_Channel3,sizeof(oledShowBuff));//设定传输长度
	DMA_Cmd(DMA1_Channel3,ENABLE);//开始传输
}

void oledShowBuffWrite(u8 x,u8 y,u8 depth)
{
	u16 tmp,index = (x*10/4)%10;
	if(y >= 64)y = 63;
	tmp = oledShowBuff[x/4][y];
	switch(index)
	{
		case 0:
			tmp |= (((u16)depth)<<4)&0x00F0;
			tmp &= (((u16)depth)<<4)|0xFF0F;
		break;
		
		case 2:
			tmp |= (((u16)depth)<<0)&0x000F;
			tmp &= (((u16)depth)<<0)|0xFFF0;
		break;
		
		case 5:
			tmp |= (((u16)depth)<<12)&0xF000;
			tmp &= (((u16)depth)<<12)|0x0FFF;
		break;
		
		case 7:
			tmp |= (((u16)depth)<<8)&0x0F00;
			tmp &= (((u16)depth)<<8)|0xF0FF;
		break;
	}
	oledShowBuff[x/4][y] = tmp;
}

u8 oledShowBuffRead(u8 x,u8 y)
{
	return 0;
}

//16度灰色 一个字节两个像素
void oledShow16GrayImage(u8 xStart,u8 yStart,u8 xEnd,u8 yEnd,u8 *image)
{
	u16 xSize = xEnd-xStart;
	if(yStart >= 64)yStart = 63;
	if(yEnd >= 64)yEnd = 63;
	for(int yy=0;yy<yEnd-yStart;yy++)
	{
		u16 y = yStart+yy;
		for(int xx=0;xx<xSize/2;xx++)
		{
			for(int z=0;z<2;z++)
			{
				oledShowBuffWrite(xStart+(xx*2)+z,y,(z%2) ? image[(yy*(xSize/2))+xx]&0x0F : image[(yy*(xSize/2))+xx]>>4);
			}
		}
	}
}

//单色图片 低位在前
void oledShowBAWImage(u8 xStart,u8 yStart,u8 xEnd,u8 yEnd,u8 *image,u8 depth,u8 background)
{
	u16 xSize = xEnd-xStart;
	if(yStart >= 64)yStart = 63;
	if(yEnd >= 64)yEnd = 63;
	for(int yy=0;yy<yEnd-yStart;yy++)
	{
		u16 y = yStart+yy;
		for(int xx=0;xx<xSize/8;xx++)
		{
			for(int z=0;z<8;z++)
			{
				oledShowBuffWrite(xStart+(xx*8)+z,y,(image[(yy*(xSize/8))+xx]&(1<<z)) ? depth:background);
			}
		}
	}
}

//16度灰色 一个字节一个像素
void oledShowImage(u8 xStart,u8 yStart,u8 xEnd,u8 yEnd,u8 *image)
{
	u16 xSize = xEnd-xStart;
	if(yStart >= 64)yStart = 63;
	if(yEnd >= 64)yEnd = 63;
	for(int yy=0;yy<yEnd-yStart;yy++)
	{
		u16 y = yStart+yy;
		for(int xx=0;xx<xSize;xx++)
			oledShowBuffWrite(xStart+xx,y,image[(yy*xSize)+xx]);
	}
}

void oledFill(u8 xStart,u8 xEnd,u8 yStart,u8 yEnd,u8 depth)
{
	for(int y=yStart;y<yEnd;y++)//256*64分辨率 一个字节2个像素
	{
		for(int x=xStart;x<xEnd;x++)
			oledShowBuffWrite(x,y,depth);
	}
}

void oledFillToBuff(u8 *buf,u8 width,u8 high,u8 xStart,u8 xEnd,u8 yStart,u8 yEnd,u8 depth)
{
	for(int y=yStart;y<yEnd;y++)//256*64分辨率 一个字节2个像素
	{
		for(int x=xStart;x<xEnd;x++)
			buf[(y*width)+x] = depth;
	}
}


void oledShowString(u8 x,u8 y,char *s,u8 depth,u8 background)
{
	unsigned char j=0;
	while (s[j]!='\0')
	{
		if(!(s[j]&0x80))
		{
			oledShowChr8x16(x,y,s[j],depth,background);
			if(x <= 255-8)
				x+=8;
			else
			{
				x=0;
				y+=16;
			}
			j++;
		}
		else
		{
			oledShowGb16x16(x,y,&s[j],depth,background);
			if(x <= 255-16)
				x+=16;
			else
			{
				x = 0;
				y+=16;
			}
			j+=2;
		}
		if(y > 64-16)y = 0;
	}
}

void oledShowGb16x16(u8 x,u8 y,char *gb,u8 depth,u8 background)
{
	u8 i,*msk;
	if(x > OLED_WIDTH-16)x = 0;
	if(y > OLED_HIGH-16)y = 0;
	
	//查找字模
	for(i=0;i<FNT_GB16_NUM;i++)
	{
		if(GB_16[i].Index[0] == gb[0] && GB_16[i].Index[1] == gb[1])
		{
			msk = (u8 *)GB_16[i].Msk;
			break;
		}
	}
	if(i != FNT_GB16_NUM)
	{
		//显示中文
		for(int yy=0;yy<16;yy++)
		{
			for(int xx=0;xx<2;xx++)
			{
				for(int z=0;z<8;z++)
				{
					if(msk[(yy*2) + xx] & (1<<z))
						oledShowBuffWrite(x + ((xx * 8) + z),y + yy,depth);
					else
						oledShowBuffWrite(x + ((xx * 8) + z),y + yy,background);
				}
			}
		}
	}
}

void oledShowGb16x16ToBuff(u8 x,u8 y,char *gb,u8 depth,u8 background,u8 *buf,u8 width,u8 high)
{
	u8 i,*msk;
	if(x > width-16)x = 0;
	if(y > high-16)y = 0;
	while(*gb != '\0')
	{
		//查找字模
		for(i=0;i<FNT_GB16_NUM;i++)
		{
			if(GB_16[i].Index[0] == gb[0] && GB_16[i].Index[1] == gb[1])
			{
				msk = (u8 *)GB_16[i].Msk;
				break;
			}
		}
		if(i != FNT_GB16_NUM)
		{
			//显示中文
			for(int yy=0;yy<16;yy++)
			{
				for(int xx=0;xx<2;xx++)
				{
					for(int z=0;z<8;z++)
					{
						if(msk[(yy*2) + xx] & (1<<z))
							buf[x + ((xx * 8) + z) + ((y + yy) * width)] = depth;
						else
							buf[x + ((xx * 8) + z) + ((y + yy) * width)] = background;
					}
				}
			}
		}
		x += 16;
		gb += 2;
	}
}

void oledWriteNumber24x48ToBuff(u8 x,u8 y,char num,u8 depth,u8 background,u8 *buf,u8 width,u8 high)
{
	if(y >= high-48)y = 0;
	if(x >= width-24)x = 0;
	for(int yy=0;yy<48;yy++)
	{
		for(int z=0;z<3;z++)
		{
			for(int xx=0;xx<8;xx++)
			{
				if(number24x48[(num * 144) + (yy * 3) + z] & (1<<xx))
					buf[((yy * width) + y) + (((z * 8) + xx) + x)] = depth;
				else
					buf[((yy * width) + y) + (((z * 8) + xx) + x)] = 0;
			}
		}
	}
}

//显示一个字符
void oledShowChr8x16(u8 x,u8 y,char chr,u8 depth,u8 background)
{
	u8 c=0;	
	c=chr-' ';//得到偏移后的值
	if(x >= OLED_WIDTH-8)x = 0;
	if(y >= OLED_HIGH-8)y = 0;
	for(int yy=0;yy<16;yy++)
	{
		for(int xx=0;xx<8;xx++)
		{
			if(nAsciiDot[(c * 16) + yy] & (1<<xx))
				oledShowBuffWrite(x+xx,y+yy,depth);
			else
				oledShowBuffWrite(x+xx,y+yy,background);
		}
	}
}

//清除显示
void oledClear(void)
{
	for(int y=0;y<OLED_HIGH;y++)
		for(int x=0;x<OLED_WIDTH/4;x++)
			oledShowBuff[x][y] = 0;
}

//设置列的起始和终止地址
void oledSetColumnAddr(u8 start,u8 end)
{
	writeByte(0x15,OLED_CMD);       // Set Column Address
	writeByte(0x1c+start,OLED_DAT);
	writeByte(0x1c+end,OLED_DAT);
}

//设置行的起始和终止地址
void oledSetRowAddr(u8 start,u8 end)
{
	writeByte(0xa0,OLED_CMD);	//Set Re-Map $ Dual COM Line Mode
	writeByte(0x15,OLED_DAT);	//
	writeByte(0x75,OLED_CMD);       // Row Column Address
	writeByte(start,OLED_DAT);
	writeByte(end,OLED_DAT);
}

//设置x,y坐标
//坐标范围是 x:0~63 y:0~63
//因SSD1322一次发送4个像素(2Byte)
void oledSetPos(u8 xStart,u8 yStart,u8 xEnd,u8 yEnd)
{
	if(xStart > 63)xStart = 0;
	if(yStart > 63)yStart = 0;
	if(xEnd > 63)xEnd = 63;
	if(yEnd > 63)yEnd = 63;
	oledSetColumnAddr(xStart,xEnd);
	oledSetRowAddr(yStart,yEnd);
	writeByte(0x5C,OLED_CMD);
}

static void writeByte(u8 dat,oledCmd_t cmd)
{
	OLED_DC = cmd;
	HalSPIWriteByte(HAL_SPI_1,dat);  
}

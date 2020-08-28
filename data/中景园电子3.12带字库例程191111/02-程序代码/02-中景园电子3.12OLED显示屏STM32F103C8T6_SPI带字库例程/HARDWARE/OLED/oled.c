#include "oledfont.h"
#include "delay.h"
#include "oled.h"

//******************************************************************************
//    函数说明：OLED写入一个字节
//    入口数据：dat    数据
//              mode   数据/命令标志 0,表示命令;1,表示数据;
//    返回值：  无
//******************************************************************************
void OLED_WR_Byte(u8 dat,u8 mode)
{
	u8 i;
	if(mode)
	{
    OLED_DC_Set();		//写数据
	}
	else
	{
		OLED_DC_Clr();     //写命令
  }
	OLED_CS_Clr();  
	for(i=0;i<8;i++)
	{
		OLED_SCLK_Clr();//将时钟信号设置为低电平
		if(dat&0x80)//将dat的8位从最高位依次写入
		{
			OLED_SDIN_Set();
    }
		else
		{
			OLED_SDIN_Clr();
    }
		OLED_SCLK_Set();//将时钟信号设置为高电平
		dat<<=1;
  }
	OLED_CS_Set();//将CS置1
 
}


//******************************************************************************
//    函数说明：OLED显示列的起始终止地址
//    入口数据：a  列的起始地址
//              b  列的终止地址
//    返回值：  无
//******************************************************************************
void Column_Address(u8 a,u8 b)
{
	OLED_WR_Byte(0x15,OLED_CMD);       // Set Column Address
	OLED_WR_Byte(0x1c+a,OLED_DATA);
	OLED_WR_Byte(0x1c+b,OLED_DATA);
}


//******************************************************************************
//    函数说明：OLED显示行的起始终止地址
//    入口数据：a  行的起始地址
//              b  行的终止地址
//    返回值：  无
//******************************************************************************
void Row_Address(u8 a,u8 b)
{
	OLED_WR_Byte(0xa0,OLED_CMD);	//Set Re-Map $ Dual COM Line Mode
	OLED_WR_Byte(0x15,OLED_DATA);	//
	OLED_WR_Byte(0x75,OLED_CMD);       // Row Column Address
	OLED_WR_Byte(a,OLED_DATA);
	OLED_WR_Byte(b,OLED_DATA);

}


//******************************************************************************
//    函数说明：OLED清屏显示
//    入口数据：无
//    返回值：  无
//******************************************************************************
void OLED_Fill(u8 color)
{
	u8 x,y;
	Column_Address(0,63);
	Row_Address(0,63);
	OLED_WR_Byte(0x5C,OLED_CMD);    //写RAM命令
	for(y=0;y<64;y++)
	{
		for(x=0;x<128;x++)
		{
			OLED_WR_Byte(color,OLED_DATA);
    }
  }
}


//******************************************************************************
//    函数说明：OLED显示字符函数 
//    此函数适用范围：字符宽度是4的倍数  字符高度是宽度的2倍
//    入口数据：x,y   起始坐标
//              chr   要写入的字符
//              sizey 字符高度 
//    返回值：  无
//******************************************************************************
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 sizey)
{
	u8 c,i,j,k;
	u8 data1,data2,data3,data4,DATA1=0,DATA2=0;
	u8 size2=sizey/8;
	c=chr-' ';//得到偏移后的值
	for(i=0;i<size2;i++)
	{
		Column_Address(x/4,x/4+sizey/8-1);
		Row_Address(y+i*8,y+i*8+7);
		OLED_WR_Byte(0x5C,OLED_CMD);
		for(j=0;j<size2;j++)
		{
			if(sizey==16)
			{
				data1=F8X16[c][4*j+i*sizey/2+0];//8x16 ASCII码
				data2=F8X16[c][4*j+i*sizey/2+1];//此例程可实现如下情况的ASCII码
				data3=F8X16[c][4*j+i*sizey/2+2];//1：字宽是4的倍数 2：字高是字宽的2倍
				data4=F8X16[c][4*j+i*sizey/2+3];
			}
			else if(sizey==24)
			{
				data1=F12X24[c][4*j+i*sizey/2+0];//12x24 ASCII码
				data2=F12X24[c][4*j+i*sizey/2+1];//此例程可实现如下情况的ASCII码
				data3=F12X24[c][4*j+i*sizey/2+2];//1：字宽是4的倍数 2：字高是字宽的2倍
				data4=F12X24[c][4*j+i*sizey/2+3];
			}
			else if(sizey==32)
			{
				data1=F16X32[c][4*j+i*sizey/2+0];//16x32 ASCII码
				data2=F16X32[c][4*j+i*sizey/2+1];//此例程可实现如下情况的ASCII码
				data3=F16X32[c][4*j+i*sizey/2+2];//1：字宽是4的倍数 2：字高是字宽的2倍
				data4=F16X32[c][4*j+i*sizey/2+3];
			}
//			else if(sizey==xx)               //如果需要添加新的字体，只需要将xx替换成所需汉字高度
//			{                                //然后新建数组FXXXX[][]并将取好的模放入其中
//				data1=FXXXX[c][4*j+i*sizey/2+0];
//				data2=FXXXX[c][4*j+i*sizey/2+1];
//				data3=FXXXX[c][4*j+i*sizey/2+2];
//				data4=FXXXX[c][4*j+i*sizey/2+3];
//			}
			for(k=0;k<8;k++)
			{
				if(data1&(0x01<<k))
				{
					DATA1=0xf0;
				}
				if(data2&(0x01<<k))
				{
					DATA1|=0x0f;
				}
				OLED_WR_Byte(DATA1,OLED_DATA);
				DATA1=0;
				if(data3&(0x01<<k))
				{
					DATA2=0xf0;
				}
				if(data4&(0x01<<k))
				{
					DATA2|=0x0f;
				}
				OLED_WR_Byte(DATA2,OLED_DATA);
				DATA2=0;
			}
		}
  }
}
//******************************************************************************
//    函数说明：OLED显示字符串
//    入口数据：x,y  起始坐标
//              *dp  要写入的字符串
//    返回值：  无
//******************************************************************************
void OLED_ShowString(u8 x,u8 y,u8 *dp,u8 sizey)
{
	while(*dp!='\0')
	{
	  OLED_ShowChar(x,y,*dp,sizey);
		dp++;
		x+=sizey/2;
	}
}


//******************************************************************************
//    函数说明：m^n
//    入口数据：m:底数 n:指数
//    返回值：  result
//******************************************************************************
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;
	while(n--)result*=m;    
	return result;
}


//******************************************************************************
//    函数说明：OLED显示字符码值
//    入口数据：x,y :起点坐标	 
//              len :数字的位数
//              size:字体大小
//    返回值：  无
//******************************************************************************
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 sizey)
{         	
	u8 t,temp;
	u8 enshow=0;
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(sizey/2)*t,y,' ',sizey);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(sizey/2)*t,y,temp+'0',sizey); 
	}
}


//******************************************************************************
//    函数说明：OLED显示汉字
//    入口数据：x,y :起点坐标
//              len :数字的位数
//              size:字体大小
//    返回值：  无
//******************************************************************************
void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 sizey)
{
	u8 i,j,k;
	u8 data1,data2,data3,data4,DATA1=0,DATA2=0;
	for(i=0;i<sizey/8;i++)
	{
		Column_Address(x/4,x/4+sizey/4-1);
		Row_Address(y+i*8,y+i*8+7);
		OLED_WR_Byte(0x5C,OLED_CMD);
		for(j=0;j<sizey/4;j++)
		{
			if(sizey==16)
			{
				data1=Hzk16x16[num][4*j+i*sizey+0];//16x16 汉字
				data2=Hzk16x16[num][4*j+i*sizey+1];//此例程可实现字宽=字高，字高是8的倍数的汉字
				data3=Hzk16x16[num][4*j+i*sizey+2];
				data4=Hzk16x16[num][4*j+i*sizey+3];
			}
			else if(sizey==24)
			{
				data1=Hzk24x24[num][4*j+i*sizey+0];//24x24 汉字
				data2=Hzk24x24[num][4*j+i*sizey+1];//此例程可实现字宽=字高，字高是8的倍数的汉字
				data3=Hzk24x24[num][4*j+i*sizey+2];
				data4=Hzk24x24[num][4*j+i*sizey+3];
			}
			else if(sizey==32)
			{
				data1=Hzk32x32[num][4*j+i*sizey+0];//32x32 汉字
				data2=Hzk32x32[num][4*j+i*sizey+1];//此例程可实现字宽=字高，字高是8的倍数的汉字
				data3=Hzk32x32[num][4*j+i*sizey+2];
				data4=Hzk32x32[num][4*j+i*sizey+3];
			}
//			else if(sizey==xx)               //如果需要添加新的字体，只需要将xx替换成所需汉字高度
//			{                                //然后新建数组FXXXX[][]并将取好的模放入其中
//				data1=FXXXX[num][4*j+i*sizey+0];
//				data2=FXXXX[num][4*j+i*sizey+1];
//				data3=FXXXX[num][4*j+i*sizey+2];
//				data4=FXXXX[num][4*j+i*sizey+3];
//			}
			for(k=0;k<8;k++)
			{
				if(data1&(0x01<<k))
				{
					DATA1=0xf0;
				}
				if(data2&(0x01<<k))
				{
					DATA1|=0x0f;
				}
				OLED_WR_Byte(DATA1,OLED_DATA);
				DATA1=0;
				if(data3&(0x01<<k))
				{
					DATA2=0xf0;
				}
				if(data4&(0x01<<k))
				{
					DATA2|=0x0f;
				}
				OLED_WR_Byte(DATA2,OLED_DATA);
				DATA2=0;
			}
		}
  }
}


//******************************************************************************
//    函数说明：OLED屏初始化
//    入口数据：BMP :为图像数据
//              Gray_Level=0时全屏只显示一种颜色，Gray_Level=1时显示支持灰度图片；
//              Color:灰度值0～15;Color越大，屏幕亮度越高。
//    返回值：  无
//******************************************************************************
void OLED_DrawBMP(u8 *BMP,u8 Gray_Level,u8 Color)
{
	u8 i,j,k;
	u8 data1,data2,data3,data4,DATA1,DATA2;
	if(Gray_Level==0)
	{
		for(k=0;k<8;k++)
		{
			Column_Address(0,64);
			Row_Address(8*k,8*k+7);
			OLED_WR_Byte(0x5C,OLED_CMD);
			for(j=0;j<64;j++)
			{
				data1=*BMP++;
				data2=*BMP++;
				data3=*BMP++;
				data4=*BMP++;
				for(i=0;i<8;i++)
				{
					if(data1&(0x01<<i))	
					{
						DATA1=(Color << 4);
					}
					if(data2&(0x01<<i))
					{
						DATA1|=Color;
					}
					OLED_WR_Byte(DATA1,OLED_DATA);
					DATA1=0;
					if(data3&(0x01<<i))	
					{
						DATA2=(Color << 4);
					}
					if(data4&(0x01<<i))
					{
						DATA2|=Color;
					}
					OLED_WR_Byte(DATA2,OLED_DATA);
					DATA2=0;
        }
			}
    }
  }
	else
	{
		for(k=0;k<64;k++)
		{
			Column_Address(k,k+2);
			Row_Address(0,63);
			OLED_WR_Byte(0x5C,OLED_CMD);
			for(j=0;j<64;j++)
			{
				OLED_WR_Byte(*(BMP+128*k+j),OLED_DATA);
				OLED_WR_Byte(*(BMP+128*k+j+64),OLED_DATA);
      }
    }
  }
}


//******************************************************************************
//    函数说明：OLED屏初始化
//    入口数据：无
//    返回值：  无
//******************************************************************************
void Init_SSD1322(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);	 //使能A端口时钟
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_6|GPIO_Pin_15;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	         //初始化GPIOA
 	GPIO_SetBits(GPIOA,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_6|GPIO_Pin_15);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 	 //下拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	         //初始化GPIOA
 	GPIO_SetBits(GPIOA,GPIO_Pin_5);
	
	OLED_RST_Clr();	
	delay_ms(100);	
	OLED_RST_Set();

	OLED_WR_Byte(0xfd,OLED_CMD);  /*Command Lock*/ 
	OLED_WR_Byte(0x12,OLED_DATA);
	
	OLED_WR_Byte(0xae,OLED_CMD); //Sleep In 
	
	OLED_WR_Byte(0xb3,OLED_CMD);	//Set Display Clock Divide Ratio/Oscillator Frequency 
	OLED_WR_Byte(0x91,OLED_DATA);
	
	OLED_WR_Byte(0xca,OLED_CMD);	//Set Multiplex Ratio 
	OLED_WR_Byte(0x3f,OLED_DATA);
	
	OLED_WR_Byte(0xa2,OLED_CMD);	//Set Display Offset 
	OLED_WR_Byte(0x00,OLED_DATA);	//
	
	OLED_WR_Byte(0xa1,OLED_CMD);	//Set Display Start Line 
	OLED_WR_Byte(0x00,OLED_DATA);	//
	
	OLED_WR_Byte(0xa0,OLED_CMD);	//Set Re-Map $ Dual COM Line Mode
	OLED_WR_Byte(0x15,OLED_DATA);	//
	
	
	OLED_WR_Byte(0xB5,OLED_CMD);	//Set GPIO 
	OLED_WR_Byte(0x00,OLED_DATA);
		
	OLED_WR_Byte(0xab,OLED_CMD);	//Function Selection
	OLED_WR_Byte(0x01,OLED_DATA);	//
	
	OLED_WR_Byte(0xb4,OLED_CMD);	//Enable External VSL 
	OLED_WR_Byte(0xa0,OLED_DATA);	//
	OLED_WR_Byte(0xfd,OLED_DATA);	//
	
	OLED_WR_Byte(0xc1,OLED_CMD);	//Set Contrast Current
	OLED_WR_Byte(0xff,OLED_DATA);	
	
	OLED_WR_Byte(0xc7,OLED_CMD);	//Master Contrast Current Control
	OLED_WR_Byte(0x0f,OLED_DATA);	//

	OLED_WR_Byte(0xb9,OLED_CMD);	//Select Default Linear Gray Scale Table 
	
	OLED_WR_Byte(0xb1,OLED_CMD);	//Set Phase Length
	OLED_WR_Byte(0xe2,OLED_DATA);	
	
	OLED_WR_Byte(0xd1,OLED_CMD);	//Enhance Driving Scheme Capability 
	OLED_WR_Byte(0x82,OLED_DATA);	
	OLED_WR_Byte(0x20,OLED_DATA);	

	OLED_WR_Byte(0xbb,OLED_CMD);	//Set Pre-Charge Voltage 
	OLED_WR_Byte(0x1f,OLED_DATA);	

	OLED_WR_Byte(0xb6,OLED_CMD);	//Set Second Pre-Charge Period 
	OLED_WR_Byte(0x08,OLED_DATA);	

	OLED_WR_Byte(0xbe,OLED_CMD);	//Set VCOMH Deselect Level 
	OLED_WR_Byte(0x07,OLED_DATA);		

	OLED_WR_Byte(0xa6,OLED_CMD);	//Set Display Mode
	//Clear Screen

	OLED_WR_Byte(0xaf,OLED_CMD);	//Sleep Out
}




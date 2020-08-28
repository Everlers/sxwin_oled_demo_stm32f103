#include "voice.h"
#include "oneWire.h"
#include "fun.h"
#include "led.h"

void voiceInit(void)
{
	dataForm_t form;
	
	form.impulse.start.tpLevel = 0;
	form.impulse.start.level1Time = 10000;
	form.impulse.start.level2Time = 0;
	
	form.impulse.data0.tpLevel = 1;
	form.impulse.data0.level1Time = 200;
	form.impulse.data0.level2Time = 600;
	
	form.impulse.data1.tpLevel = 1;
	form.impulse.data1.level1Time = 600;
	form.impulse.data1.level2Time = 200;

	form.impulse.series.tpLevel = 1;
	form.impulse.series.level1Time = 2000;
	form.impulse.series.level2Time = 5000;
	
	form.impulse.end.tpLevel = 0;
	form.impulse.end.level1Time = 0;
	form.impulse.end.level2Time = 0;
	
	form.sequence = OW_DATA_SEQUENCE_LOW;
	form.waitTime = 0;
	form.numBer = 0;
	owConfig(&form);//配置单总线格式
}

void voiceRep(u8 dat)
{
	u16 data = 0x5500|dat;
	owSend((u8 *)&data,2);
}

void voiceTask(void)
{
	if(owRecvStateGet() == OW_STA_DONE)
	{
		u16 data;
		if(owRecvDataGet((u8 *)&data,sizeof(data)) == sizeof(data) && (data & 0xFF00) == 0xAA00)
		{
			u8 rsp = data&0x00FF;
			switch((data&0x00FF))
			{
				case 0x55://语音模块唤醒 0:来了 1:在呢
					ledStream();
				break;
				
				case 0xFE://语音模块进入休眠 2:再见
					ledAllClose();
					rsp = 0;
				break;
				
				case 0x03://语音导航	3:请扫描二维码获取帮助
					
				break;
				
				case 0x04://打开暖风机 4:暖风机已打开
					funAcPowerOn();
				break;
				
				case 0x05://关闭暖风机 5:暖风机已关闭
					funAcPowerOff();
				break;
				
				case 0x06://打开电暖器 6:电暖器已打开
					funAcSetMode(AC_MODE_HEAT);
				break;
				
				case 0x07://关闭电暖器 7:电暖器已关闭
					funAcSetMode(AC_MODE_WIND);
				break;
				
				case 0x08://打开热风 8:热风已打开
					funAcSetMode(AC_MODE_HEAT);
				break;
				
				case 0x09://打开冷风 9:冷风已打开
					funAcSetMode(AC_MODE_COLD);
				break;
				
				case 0x0A://打开扫风 10:扫风已打开
					funAcSWind(SET);
				break;
				
				case 0x0B://关闭扫风 11:扫风已关闭
					funAcSWind(RESET);
				break;
				
				case 0x0C://打开摇头 12:摇头已打开
					funAcArefaction(SET);
				break;
				
				case 0x0D://停止摇头 13:摇头已关闭
					funAcArefaction(RESET);
				break;
				
				case 0x0E://一档取暖 14:已调到一档
					funAcSetMode(AC_MODE_HEAT);
					funAcSetWinds(1);
				break;
				
				case 0x0F://二档取暖 15:已调到二档
					funAcSetMode(AC_MODE_HEAT);
					funAcSetWinds(2);
				break;
				
				case 0x10://低风挡 16:已调到低风挡
					funAcSetWinds(2);
				break;
				
				case 0x11://高风挡 17:已调到高风挡
					funAcSetWinds(5);
				break;
				
				case 0x12://太热了 18:温度已降低 19：温度已最低
					funAcSetTemp(0);
				break;
				
				case 0x13://太冷了 20:温度已调高 21：温度已最高
					funAcSetTemp(0xFF);
				break;
				
				case 0x14://定时一小时 22：一小时已定时
					funAcSetTime(60);
				break;
				
				case 0x15://定时半小时 23：半小时已定时
					funAcSetTime(30);
				break;
			}
			voiceRep(rsp);//应答模块
		}
	}
}

#include "destruct.h"
#include "hal_flash.h"
#include "delay.h"
#include "string.h"

//自毁系统
DestructNumOrUUID_t DestructNumOrUUID = DES_UUID;//选择检测方式

//自毁清除的数据
const u8 clear_data[]={
"The program has been destroyed,\
 Please contact Software Engineer"
};

//唯一识别码 （预防程序被下载到别的板子上）
//如果在板子的储存空间内未发现有以下字符则自毁代码
const u8 UUID[DES_UUID_LENGTH]={
"XieYuFeng-GuoDun"
};

void DestructNumSet(u32 num)
{
	u8 flag;
	HalFlashRead(DES_FLAG_SAVE_ADDR,&flag,1);//读取标记
	if(flag != DES_REVISE_FLASG)//如果没有修改过此设备的可以用次数
	{
		flag = DES_REVISE_FLASG;//标记已经修改
		HalFlashWrite(DES_FLAG_SAVE_ADDR,&flag,1);//写入标记
		HalFlashWrite(DES_NUM_SAVE_ADDR,(u8 *)&num,4);//修改可用次数
	}
}

void DestructSystem(void)
{
	Check:
	if(DestructNumOrUUID == DES_NUM)
	{
		u32 num;
		DestructNumSet(DES_NUMBER);//设置使用次数
		HalFlashRead(DES_NUM_SAVE_ADDR,(u8 *)&num,4);
		if(num == 0)//如果次数用完
		{
			if(DES_WHILE_OR_DESTRUCT)//如果成立则死循环
				while(1){}
			else//否则销毁
				HalFlashWrite(DES_CODE_RUN_ADDR,(u8 *)clear_data,sizeof(clear_data));
		}
		num-=1;
		HalFlashWrite(DES_NUM_SAVE_ADDR,(u8 *)&num,4);
	}
	else if(DestructNumOrUUID == DES_UUID)
	{
		u8 uuid[sizeof(UUID)];
		HalFlashRead(DES_UUID_SAVE_ADDR,uuid,sizeof(uuid));
		if(memcmp(uuid,UUID,sizeof(uuid)))//如果UUID错误
		{
			if(DES_WRITE_UUID)//如果是出厂板子则写入UUID
				HalFlashWrite(DES_UUID_SAVE_ADDR,(u8 *)UUID,sizeof(UUID));//写入UUID到Flash
			else{//否则就是更新的代码 且UUID对不上 非原厂PCB
				DestructNumOrUUID = DES_NUM;//设置次数限制
				goto Check;//执行次数限制
			}
		}
	}
}

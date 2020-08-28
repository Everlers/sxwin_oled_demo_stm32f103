#include "encryption.h"
#include "hal_flash.h"
#include "string.h"

#if ID_WRITE_ADDR
static u8 *EncryptionCPUID = (u8 *)ID_WRITE_ADDR;//加密后的CPUID
#else
static const u8 EncryptionCPUID[12] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};//加密后的CPUID
#endif
static const u32 termCountdown = TERM_COUNTDOWN;//期限倒计数(在CPUID错误的情况下开始倒数)

#if EMPOWER_CODE_WRITE
//芯片授权
//将芯片ID通过简单运算写入Flash 
void encryptionEmpower(void)
{
	u8 *CPUID = (u8 *)0x1FFFF7E8;
	u8 tmp[12];
	for(u8 i=0;i<CPUID_LENGTH;i++){//计算CPUID
		tmp[i] = CPUID[i]>>1;
		tmp[i] |= CPUID[i]<<6;
	}
	if(memcmp(EncryptionCPUID,tmp,CPUID_LENGTH))//如果没有储存加密后的ID
		HalFlashWrite((u32)EncryptionCPUID,tmp,CPUID_LENGTH);//保存加密后的CPUID到Flash
}
#endif

//取消加密授权
void encryptionCancelEmpower(void)
{
	u8 Clear[]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	HalFlashWrite((u32)EncryptionCPUID,Clear,CPUID_LENGTH);
}

//授权验证
void encryptionEmpowerCheck(void)
{
	u8 *CPUID = (u8 *)0x1FFFF7E8;
	u8 tmp[12];
	for(u8 i=0;i<CPUID_LENGTH;i++){//计算CPUID
		tmp[i] = CPUID[i]>>1;
		tmp[i] |= CPUID[i]<<6;
	}
	if(memcmp(tmp,EncryptionCPUID,CPUID_LENGTH)){//如果ID不正确(未授权)
		u32 *Countdown = (u32 *)&termCountdown;
		if(*Countdown == 0){//如果计数完成
			char DestructData[] = {"Unauthorized hardware!"};
			HalFlashWrite(0x08000000,(u8 *)DestructData,strlen(DestructData));
		}
		else{
			u32 tmp = *Countdown-1;
			HalFlashWrite((u32)&termCountdown,(u8 *)&tmp,sizeof(termCountdown));
		}
	}
}

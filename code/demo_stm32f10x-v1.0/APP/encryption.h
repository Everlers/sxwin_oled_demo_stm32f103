#ifndef _ENCRYPTION_H_
#define _ENCRYPTION_H_
#include "stm32f10x.h"

//使用特殊方法将CPUID进行加密计算写入到指定Flash地址(或由编译器分配,如果程序带有在线更新则使用固定地址)
//注意！如果加密后的ID写入地址由编译器分配 则每次烧录程序都需要授权一次

#define ID_WRITE_ADDR										ENCRYPTION_SAVE_ADDR//加密后的ID写入到指定位置 如果地址为0则按照编译器分配的地址写入ID (ID长度12个字节)
#define EMPOWER_CODE_WRITE							1//是否写入授权代码(如果不写入则需要另外对CPUID进行加密并写入到指定的Flash地址)
#define CPUID_LENGTH										12//CPUID 长度
#define TERM_COUNTDOWN									10//设置在未授权的情况下使用次数(重新上电或者复位次数)

void encryptionEmpower(void);//将此芯片加密授权(将CPUID加密保存在Flash)
void encryptionCancelEmpower(void);//取消加密授权(擦除在Flash保存的CPUID)
void encryptionEmpowerCheck(void);//授权验证(每次运行程序前调用即可,如果验证有误则到达一定次数后摧毁Flash首地址数据)

#endif

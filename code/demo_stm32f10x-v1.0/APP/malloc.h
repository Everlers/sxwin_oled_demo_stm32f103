#ifndef malloc_h
#define malloc_h
#include "stm32f10x.h"

#define MALLOC
#define MEM_BLOCK_SIZE 								32//分配一块内存最小字节
#define MAX_MEM_RAM_SIZE 							(8 * 1024)//动态内存分配的字节大小
#define MAX_ALLOC_TABLE_SIZE 					(MAX_MEM_RAM_SIZE / MEM_BLOCK_SIZE)//每块的大小

typedef void (*MallocErrorCb) (void);//内存分配错误的回调

typedef struct {
	u8 	Mem_Base[MAX_MEM_RAM_SIZE];//内存池
	u16 Mem_Map[MAX_ALLOC_TABLE_SIZE];//内存管理状态
	u8 	Init_Flag;//初始化状态 
}Malloc_TypeDef;

void malloc_Init(MallocErrorCb ErrorCB);
u8 mem_rate(void);
void *Malloc(u32 len);
void Free(void *ptr);

#endif

#include "malloc.h"
#include "stdio.h"
#include "hal_sys.h"
#include "string.h"

Malloc_TypeDef mem_dev={.Init_Flag = 0};
MallocErrorCb Error = NULL;

void malloc_Init(MallocErrorCb ErrorCB){
	memset(mem_dev.Mem_Map,0,sizeof(mem_dev.Mem_Map));//内存管理表初始化
	memset(mem_dev.Mem_Base,0,sizeof(mem_dev.Mem_Base));//内存池初始化
	mem_dev.Init_Flag = 1;//初始化完成标志
	Error = ErrorCB;
}

//内存使用率
u8 mem_rate(void){
	u32 rate = 0;
	for(u32 i=0;i<MAX_ALLOC_TABLE_SIZE;i++){
		if(mem_dev.Mem_Map[i])rate++;
	}
	return rate*100/MAX_ALLOC_TABLE_SIZE;
}

void *Malloc(u32 len){
	u32 i,offset;
	u32 nmemb; //需要的内存块
	u32 cmemb=0; //找到的内存块
	if(!mem_dev.Init_Flag){malloc_Init(NULL);}//调用初始化
	if(len == 0)return ((void *)0);//不需要分配
	nmemb = len/MEM_BLOCK_SIZE;//计算内存块
	if(len%MEM_BLOCK_SIZE)nmemb++;//如果有余数需要多一块内存
	//INTX_DISABLE();//关闭所有中断
	for(offset=MAX_ALLOC_TABLE_SIZE-1;offset>0;offset--){//从上到下寻找内存
		if(mem_dev.Mem_Map[offset] == 0) cmemb++;//如果本内存块没有被分配
		else cmemb = 0;//连续的内存块不够 清除继续往下找
		if(cmemb == nmemb){//找到连续内存
			for(i=0;i<nmemb;i++){//将这些内存块全部设定为使用中的状态
				mem_dev.Mem_Map[i+offset]=nmemb;//写入分配的块大小
			}
			return ((void *)((offset*MEM_BLOCK_SIZE)+(u32)(mem_dev.Mem_Base)));//返回变量地址
		}
	}
	//INTX_ENABLE();
	if(Error)//不够内存分配
		Error();
	return (void *)0;//没有找到
}

void Free(void *ptr){
	u32 Mem_addr;
	u32 i,offset;
	u32 Bsize;
	offset = (u32)ptr;
	offset -= (u32)&mem_dev.Mem_Base;
	INTX_DISABLE();
	if(offset < MAX_MEM_RAM_SIZE){
		Mem_addr = offset/MEM_BLOCK_SIZE;//块地址
		Bsize = mem_dev.Mem_Map[Mem_addr];//块数量
		for(i=0;i<Bsize;i++){
			mem_dev.Mem_Map[i+Mem_addr] = 0;
		}
	}
	INTX_ENABLE();
}

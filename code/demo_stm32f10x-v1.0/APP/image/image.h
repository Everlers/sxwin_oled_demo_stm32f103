#ifndef _IMAGE_H_
#define _IMAGE_H_
#include "hal_sys.h"

//Image 单色  高位在前
extern const unsigned char gImage_power[1152];//电源
extern const unsigned char gImage_power1[288];//电源
extern const unsigned char gImage_wifi[512];//WiFi
extern const unsigned char gImage_arefaction[512];//除湿
extern const unsigned char gImage_time[512];//定时
extern const unsigned char gImage_windd[512];//风向
extern const unsigned char gImage_sleepMode[2048];//睡眠模式
extern const unsigned char gImage_blowing_inMode [2048];//送风模式
extern const unsigned char gImage_blowing_in1Mode [2048];//送风模式
extern const unsigned char gImage_coldMode[2048];//制冷模式
extern const unsigned char gImage_heatMode[2048];//制热模式
extern const unsigned char gImage_autoMode[2048];//自动模式
extern const unsigned char gimage_temp[72];//摄氏度图标
extern const unsigned char gimage_72x16FontFrame[144];//字框
extern const unsigned char gImage_welcome[5504];//欢迎 单色

u8 *imageTime(u8 width,u8 high,u8 depth,u8 background,u16 time);
u8 *windSpeed (u8 width,u8 high,u8 depth,u8 background,u8 winds);
u8 *imageTempLabel (u8 width,u8 high,u8 depth,u8 background,char *font);
u8 *imageTemp (u8 width,u8 high,u8 depth,u8 background,int temp);
u8 *imageGet(const u8 *image,u16 size,u8 depth,u8 background);
u8 *image16GrayGet(const u8 *image,u32 size);
void imageFree(u8 *image);
#endif

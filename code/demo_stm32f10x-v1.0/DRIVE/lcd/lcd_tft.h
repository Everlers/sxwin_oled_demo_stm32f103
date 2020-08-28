#ifndef _LCD_H_
#define _LCD_H_
#include "stm32f10x.h"

#define LCD_W 240
#define LCD_H 320

#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         		 0x001F  
#define BRED             0XF81F
#define GRED 						 0XFFE0
#define GBLUE			 			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 		 0XBC40 //棕色
#define BRRED 			 		 0XFC07 //棕红色
#define GRAY  			 		 0X8430 //灰色

typedef enum{
POINT_MODE,	//使用画点模式
FILL_MODE,	//使用填充模式
}painting_mode;

void lcd_init(void);
void lcd_clear(u16 color);
void lcd_set_win(u16 x,u16 y,u16 xend,u16 yend);
void lcd_draw_point(u16 x,u16 y,u16 color);
void lcd_fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color);
void lcd_draw_line(u16 x1, u16 y1, u16 x2, u16 y2,u16 color);
void lcd_draw_rectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color);
void lcd_draw_circle(u16 x0,u16 y0,u8 r,u16 color);
void lcd_show_char (u16 x,u16 y,char c,u16 color,painting_mode mode);
void lcd_show_string(u16 x,u16 y,char *c,u16 color,painting_mode mode);
void lcd_show_num_32 (u16 x,u16 y,u8 c,u16 color);
void lcd_show_num_16 (u16 x,u16 y,u8 c,u16 color);
void lcd_show_picture(u16 x,u16 y,u16 xend,u16 yend,u8 *gImage);

u16 RGB888_565(u32 rgb);
u16 RGB565 (u8 r,u8 g,u8 b);

void lcd_printf_init(void);
void lcd_printf_exit(void);
void lcd_printf_task(void);
void lcd_printf(char *fmt,...);
void lcd_log(char *fmt,...);

/*设置缓冲函数*/
void lcd_set_buf_addr(u16 *addr);
void lcd_set_num_32_64 (u16 x,u16 y,u8 c,u16 color);
void lcd_set_num_16_32 (u16 x,u16 y,u8 c,u16 color);
void lcd_set_fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color);
void lcd_show_buf(u16 x,u16 y,u16 xend,u16 yend,u16 *buf);//把缓冲写入显示
#endif

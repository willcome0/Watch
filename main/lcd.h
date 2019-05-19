#ifndef __LCD_H
#define __LCD_H

#include "stdlib.h"
#include "Arduino.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define LCD_WIDTH 	240
#define LCD_HEIGHT 	240

//画笔颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //棕色
#define BRRED 			 0XFC07 //棕红色
#define GRAY  			 0X8430 //灰色
//GUI颜色

#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
//以上三色为PANEL的颜色 
 
#define LIGHTGREEN     	 0X841F //浅绿色
//#define LIGHTGRAY        0XEF5B //浅灰色(PANNEL)
#define LGRAY 			 0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)

typedef signed char        s8;
typedef short              s16;
typedef int                s32;
typedef long long          s64;
typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

void lcd_init(void);
void lcd_clear(uint16_t color);
void lcd_fill(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color);


void lcd_draw_point(uint16_t x, uint16_t y, uint16_t color);
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void lcd_draw_rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);

void lcd_show_char(u16 x, u16 y, u16 fc, u16 bc, u8 chr, u8 size, u8 cover);
void lcd_show_num(u16 x, u16 y, u16 fc, u16 bc, u32 num, u8 len, u8 size, u8 add, u8 cover);
void lcd_show_pic(u16 x, u16 y, u16 width, u16 height, const u8 *p);
void lcd_show_zh(u16 x, u16 y, u16 fc, u16 bc, char *str, u8 size, u8 cover);
void lcd_show_str(u16 x, u16 y, u16 fc, u16 bc, char *str, u8 size, u8 cover);
void lcd_show_img(u16 x, u16 y, u16 width, u16 height, const u8 *p, u8 colorful);
void lcd_show_img_bin(u16 x, u16 y, u16 width, u16 height, u16 fc, u16 bc, const u8 *img, u8 cover);

void lcd_show_img_bat(uint8_t show_flag);
void lcd_show_img_charge(uint8_t show_flag);
void lcd_show_img_wifi(uint8_t show_flag);
void lcd_show_img_device(void);
void lcd_show_img_game(void);
void lcd_show_img_set(void);
void lcd_show_img_help(void);
void lcd_show_img_wait(void);
void lcd_show_img_poweroff(void);
void lcd_show_img_device1(uint16_t x, uint16_t y);
void lcd_show_img_device2(uint16_t x, uint16_t y);

void ui_show_dial(void);
void ui_show_device(void);
void ui_show_game(void);
void ui_show_set(void);
void ui_show_help(void);
void ui_show_device_con(void);

#ifdef __cplusplus
}
#endif

#endif

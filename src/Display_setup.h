# ifndef DISPLAY_SETUP_H
# define  DISPLAY_SETUP_H

# include <Arduino.h>
void display_partialLine(uint8_t line, String zf); ////发送局部刷新的显示信息到屏幕,带居中
void display_partialLine_2(uint8_t line, String zf) ;
boolean sdBeginCheck(); //SD挂载检查
void xiaobian(); //消除黑边（四周的边跟随屏幕刷新，仅全局刷新有效）
void display_Bitmap_Setup(); //显示开机图片
void display_bitmap_bottom(const unsigned char* bitmaps, String z); //显示开机图片,并在底部显示文字
void display_bitmap_sleep(String z); //显示休眠图标，并在底下显示原因
void FixedRefresh(); //定次刷新,每5次刷黑白一次，每15次正式全局刷黑白
uint16_t getCenterX(String z); //计算字符居中的X位置
void BW_refresh(); //黑白刷新一次
void displayShuaBai(); //刷白确认
void display_setup(); //天气开机连接画面
void BWClearScreen();    //黑一下清屏
void display_partialLine_BJZ(uint8_t line, String zf,uint16_t x, uint8_t width); ////发送局部刷新的显示信息到屏幕,不居中
boolean sdBeginCheck(); //SD挂载检查

# endif
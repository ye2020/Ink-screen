/**
  ****************************** Y.Z.T.****************************************
  * @file       menu.c/h
  * @brief      提供键值适配等UI菜单框架数据
  * @note          
  * @history    2022.7.8
  *
  @verbatim     v1.0
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************** Y.Z.T. *****************************************
  */

#ifndef __MENU_H
#define __MENU_H

#include "bsp_button.h"


// 对应数据类型定义
typedef unsigned int   menu_u32;
typedef unsigned short menu_u16;
typedef unsigned char  menu_u8;

typedef int menu_i32 ;
typedef short menu_s16 ;
typedef char menu_s8 ;


// 键值定义枚举
typedef enum 
{
    KEY_none  = 0,
	  KEY_enter,          // 确定
    KEY_esc,            // 返回
    KEY_up,             // 向上
    KEY_dowm,           // 向下
    KEY_home,           // home键
    KEY_setting,        // 设置键
    KEY_game_lift,      // 左转
    KEY_game_right,     // 右转

}key_value_e;

// 各界面索引，子菜单
typedef struct Menu_Key_Index
{
	menu_u8 main_current_index ;
	menu_u8 setting_current_index ; 
	menu_u8 select_current_index ; 

	
	menu_u8 Current_Page ;
}Key_Index;


extern Key_Index sub_index ;

typedef void (*menu_op_func)(button_status_e);

typedef struct OP_STRUCT
{
	int op_menu ; 				//操作菜单
	menu_op_func opfun ;	//操作方法
}OP_MENU_PAGE;

/*当前菜单*/
typedef enum 
{
	MAIN_PAGE = 0,          // 主页面						// 写0的话莫名报错
	CLOCK_PAGE,             // 时钟状态
	WEATHER_PAGE,           // 天气状态
	CONFIGURATION_PAGE,     // 配置状态
	READ_PAGE,              // 阅读状态
	GAME_PAGE,              // 游戏状态
  SETTING_PAGE,           // 设置模式
	SELECT_PAGE,						// 菜单
	
}OP_PAGE;



void Menu_Select_Item(menu_i32 current_index, button_status_e KeyValue);

void select_page_process(button_status_e KeyValue);

void main_page_process(button_status_e KeyValue);
void clock_page_process(button_status_e KeyValue);
void weather_page_process(button_status_e KeyValue);
void configuration_page_process(button_status_e KeyValue);
void read_page_process(button_status_e KeyValue);
void game_page_process(button_status_e KeyValue);
void setting_page_process(button_status_e KeyValue);
extern void Menu_Select_main(button_status_e Key5Value);




#endif
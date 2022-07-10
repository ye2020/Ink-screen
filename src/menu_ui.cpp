/**
  ****************************** Y.Z.T.****************************************
  * @file       menu_ui.c/h
  * @brief      提供菜单框架中UI的处理
  * @note
  * @history    2022.7.8
  *
  @verbatim     v1.0
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************** Y.Z.T. *****************************************
  */
#include "menu_ui.h"

//主菜单初始化
void Menu_Main_Init(void)
{
	Serial.println("home status");

	sub_index.main_current_index = 0;
	sub_index.setting_current_index = 8;
	sub_index.select_current_index = 1;
	sub_index.Current_Page = MAIN_PAGE;
	//.....刷新回主页面的UI和状态(等待添加初始化菜单)
}

//进入相应的页面
void Enter_Page(menu_i32 index, button_status_e Key5Value , button_status_e Key0Value)
{
	
	sub_index.Current_Page = index;
	switch (sub_index.Current_Page)
	{
		//进入主页面
	case MAIN_PAGE:
	{
		Menu_Select_Item(MAIN_PAGE, Key5Value,Key0Value);
		break;
	}
		// 进入时钟页面
	case CLOCK_PAGE:
	{
		Menu_Select_Item(CLOCK_PAGE,  Key5Value,Key0Value);
		break;
	}
		// 	进入天气界面
	case WEATHER_PAGE:
	{
		Menu_Select_Item(WEATHER_PAGE,  Key5Value,Key0Value);
		break;
	}
	//进入配置页面
	case CONFIGURATION_PAGE:
	{
		Menu_Select_Item(CONFIGURATION_PAGE,  Key5Value,Key0Value);
		break;
	}
	//进入阅读页面
	case READ_PAGE:
	{
		Menu_Select_Item(READ_PAGE,  Key5Value,Key0Value);
		break;
	}
	//进入游戏页面
	case GAME_PAGE:
	{
		Menu_Select_Item(GAME_PAGE,  Key5Value,Key0Value);
		break;
	}
	//进入设置页面
	case SETTING_PAGE:
	{
		Menu_Select_Item(SETTING_PAGE,  Key5Value,Key0Value);
		break;
	}
	//进入设置页面
	case SELECT_PAGE:
	{
		Menu_Select_Item(SELECT_PAGE,  Key5Value,Key0Value);
		break;
	}
	// 进入菜单页面
	case LANGUAGE_PAGE:
	{
		Menu_Select_Item(LANGUAGE_PAGE,  Key5Value, Key0Value);
		break;
	}
	// 进入菜单页面
	case WORD_PAGE:
	{
		Menu_Select_Item(WORD_PAGE,  Key5Value, Key0Value);
		break;
	}

	// 否则返回主页面
	default:
	{	Menu_Select_Item(MAIN_PAGE, Key5Value,Key0Value);
		break;
	}
	}
}

//主页面UI处理
void main_page_ui_process(menu_u8 index)
{
	switch (index)
	{
	case 0:
		break;
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	case 4:
		break;
	case 5:
		break;
	case 6:
		break;
	default:
		break;
	}
}

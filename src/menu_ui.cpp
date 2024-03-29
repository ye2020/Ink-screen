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
#include "main.h"
#include "GxEPD2_BW.h"
#include "Display_setup.h"


//主菜单初始化
void Menu_Main_Init(void)
{
	Serial.println("home status");

	sub_index.main_current_index = 0;
	sub_index.setting_current_index = 8;
	sub_index.select_current_index = 1;
	sub_index.language_current_index = 2;
	sub_index.configuration_current_index = 20;
	sub_index.read_current_index = 30;				// 不想跟指针索引重合

	sub_index.Current_Page = MAIN_PAGE;

  	display.fillScreen(baise);  // 填充屏幕
	display.drawInvertedBitmap(50, 3, Bitmap_m, 45, 45, heise);
   BW_refresh();          //黑白刷新一次



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

	case BOOK_PAGE:
	{
		Menu_Select_Item(BOOK_PAGE,  Key5Value, Key0Value);
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
	if(ui_loging_flag == 0)		// 允许ui加载
	{
		//BW_refresh();          //黑白刷新一次
		GetData();		
		BW_refresh();          //黑白刷新一次
		
		display_main_home("单击以进入菜单...","Click to enter the menu...");

		ui_loging_flag = 1;
	}

	if(RTC_get_data_count > 0xFFFFF)		// 更新数据
	{
		RTC_get_data_count = 0;
		RTC_re_count++;
		Get_clock_data();
		display_main_home_dynamic_UI();    // 动态UI更新时间显示
	}
}


//天气页面UI处理
void weather_page_ui_process(void)
{
	if(ui_loging_flag == 0)			// 放里面只获取一次 ，不会动态更新
	{
		GetData();
		//BW_refresh();          //黑白刷新一次
		BW_refresh();
		get_time_weather();  //如果wifi_flag 为1，则可以刷新时间和天气

		ui_loging_flag = 1;
	}

	// if(RTC_get_data_count > 0xFFFF)		// 更新数据
	// {
	// 	RTC_get_data_count = 0;
	// 	GetData();
	// }
}

//时钟页面UI处理
void clock_page_ui_process(void)
{

	if(ui_loging_flag == 0)
	{
		Serial.println("clock status");
	  GetData();
	  BW_refresh();
	  BW_refresh();
      display_clock();        // 时钟显示界面
	  ui_loging_flag = 1;
	}

	if(RTC_get_data_count > 0xFFFFF)		// 更新数据
	{
		RTC_re_count++;					//局刷计算
		RTC_get_data_count = 0;
		Get_clock_data();
		display_clock_dynamic_UI();        // 时钟显示界面
	}
}



// 菜单页面ui进程
void select_page_ui_process(void)
{

	if(ui_loging_flag == 0)		// 允许ui加载
	{
	   Serial.println("select status");
	  //BWClearScreen();   //黑一下刷新屏幕
      BWClearScreen();   //黑一下刷新屏幕
      display_main_select();
      ui_loging_flag = 1;
	}

}


// 设置页面UI进程
void setting_page_ui_process(void)
{
	if(ui_loging_flag == 0)		// 允许ui加载
	{
		BWClearScreen();   //黑一下刷新屏幕
		display_main_setting();
		ui_loging_flag = 1;
	}
}



// 语言页面UI进程
void language_page_ui_process(void)
{
	if(ui_loging_flag == 0)		// 允许ui加载
	{
		BWClearScreen();   //黑一下刷新屏幕
		display_main_language();
		ui_loging_flag = 1;
	}
}

// 字号模式进程
void word_page_ui_process(void)
{
	if(ui_loging_flag == 0)		// 允许ui加载
	{
		BWClearScreen();   //黑一下刷新屏幕
		display_main_word();
		ui_loging_flag = 1;
	}
}

// 配置模式进程
void configuration_page_ui_process(void)
{
	if(ui_loging_flag == 0)		// 允许ui加载
	{
		Serial.println("configuration status");
		BWClearScreen();   //黑一下刷新屏幕
		//display_peiwang();
		//peiwang_mod();
		display_peiwang();			// 静态UI第1部分
		peiwang_mod_tesk();			// 静态UI第2部分

		ui_loging_flag = 1;
	}
}

// 阅读模式进程
void read_page_ui_process(void)
{
	if(ui_loging_flag == 0)		// 允许ui加载
	{
		Serial.println("read status");
		BWClearScreen();   //黑一下刷新屏幕

		display_read_static_1();		//静态UI第一部分
		display_SD_file_ui();			// 读取SD卡内容
		//read_filename();
		ui_loging_flag = 1;
	}
}

void book_page_ui_process(void)
{	
	if(ui_loging_flag == 0)		// 允许ui加载
	{		
			Serial.println("book status"); 

		if(file_list_name[return_flie_current_num()] == "word.txt")		//单词本独立格式显示
		show_type2(file_last_read[return_flie_current_num()][1] ,return_flie_current_num());
		else
		show_type1(file_last_read[return_flie_current_num()][1],return_flie_current_num());
		ui_loging_flag = 1;
	}
}


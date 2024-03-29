/**
  ****************************** Y.Z.T.****************************************
  * @file       menu.c/h
  * @brief      提供键值适配等UI菜单框架数据
  * @note       框架移植自STM32工程，因本墨水屏项目UI子菜单仅2层，有些函数不是很必要，故稍显臃肿和混乱
  * @history    2022.7.8
  *
  @verbatim     v1.0
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************** Y.Z.T. *****************************************
  */
/***************************** 操作说明 ******************************
 * 	
 * 		操作说明：
 * 			左键（key5）：
 * 				单击：向下选择 （在主页进入菜单页面）	
 * 				双击：确认enter
 * 				长按：主页进入设置模式
 * 			
 *			右键（key0）: 
 * 				单击：向上选择
 * 				双击：返回esc
 * 				长按：返回主页home
 * 
 * 		特别注意：两键不可同时按，同时按返回无指令
*******************************************************************/


#include "menu.h"
#include <Arduino.h>
#include "menu_ui.h"
#include "main.h"

// 定义菜单索引变量
Key_Index sub_index;
uint8_t  page_current_sta = 1;		//当前页码
static uint8_t flie_current_num = 0;		// 当前书籍编号

static key_value_e Key5Value_transition_function(button_status_e button5, button_status_e button0);
void Menu_Select_Item(menu_i32 current_index, button_status_e Key5Value, button_status_e Key0Value);
uint8_t ui_loging_flag = 0;				//将ui加载标志位置0，表示允许加载ui


//菜单操作表定义
static OP_MENU_PAGE g_opStruct[] =
	{
		{MAIN_PAGE, main_page_process},
		{CLOCK_PAGE, clock_page_process},
		{WEATHER_PAGE, weather_page_process},
		{CONFIGURATION_PAGE, configuration_page_process},
		{READ_PAGE, read_page_process},
		{GAME_PAGE, game_page_process},
		{SETTING_PAGE, setting_page_process},
		{SELECT_PAGE, select_page_process},
		{LANGUAGE_PAGE, language_page_process},
		{WORD_PAGE, word_page_process},
		{BOOK_PAGE, book_page_process}
	};


//跳转到表所对应的页面
static int JUMP_Table(menu_i32 op, button_status_e Key5Value, button_status_e Key0Value)
{
	if (op >= sizeof(g_opStruct) / sizeof(g_opStruct[0]) || op < 0) // 判断该页面是否在表内（1~ 7 ）
	{
		Serial.println("unknow operate!");
		return -1;
	}
	g_opStruct[op].opfun(Key5Value, Key0Value); // 执行操作
	return 0;
}

//菜单选择项
void Menu_Select_Item(menu_i32 current_index, button_status_e Key5Value, button_status_e Key0Value)
{
	JUMP_Table(current_index, Key5Value, Key0Value);
}

/**
 * @brief
 * @param[in]   Key5Value ： 左键key5键值（左） ； Key0Value（右）
 * @retval      在loop函数调用
 * @attention
 */
void Menu_Select_main(button_status_e Key5Value, button_status_e Key0Value)
{
	Enter_Page(sub_index.Current_Page, Key5Value, Key0Value);
}

/**
 * @brief		返回当前阅读的书籍编号
 * @param[in]   none
 * @retval      none
 * @attention
 */
uint8_t return_flie_current_num(void)
{
	return flie_current_num;
}

/**
 * @brief       左键值转换，从具体的按键如（单击，双击转换成每个页面的对应功能）
 * @param[in]   button5 (左)
 * @retval
 * @attention
 */
static key_value_e Key5Value_transition_function(button_status_e button5, button_status_e button0)
{
	button_status_e button_CS = button_none;

	if		(button5 != button_none && button0 == button_none) 		button_CS = button5;
	else if (button5 == button_none && button0 != button_none)		button_CS = button0;
	else															button_CS = button_none;

	switch (button_CS)
	{

	/**************** 左按键对应键值 *******************/
	// key5 短按一下向下
	case button_click:
	{
		Serial.println("KEY_dowm");		
		return KEY_dowm;
		break;
	}
	// key5 短按2下确定
	case button_doubleclick:
	{
		Serial.println("KEY_enter");		
		return KEY_enter;
		break;
	}

	// key5 长按进入设置模式
	case button_longPressStop:
	{
		Serial.println("KEY_setting");		
		return KEY_setting;
		break;
	}

	/**************** 右按键对应键值 *******************/

	// key0 长按返回主页面
	case button_longPressStart2:
	{
		Serial.println("KEY_home");		
		return KEY_home;
		break;
	}

	// key0 短按向上
	case button_click2:
	{
		Serial.println("KEY_up");		
		return KEY_up;
		break;
	}

	// key0 短按2下返回
	case button_doubleclick2:
	{
		Serial.println("KEY_esc");		
		return KEY_esc;
		break;
	}
	default:
		break;
	}

	return KEY_none;
}


uint8_t return_UI_loging_flag(void)
{
	return ui_loging_flag;
}

/************************************************************************************************************************************/
/*********************************************************** 进程处理 **********************************************************/
/************************************************************************************************************************************/

/**
 * @brief       主页面处理
 * @param[in]   KeyValue ： 键值
 * @retval      none
 * @attention
 */
void main_page_process(button_status_e Key5Value, button_status_e Key0Value)
{
	RTC_get_data_count++;    // 计数增加，超过一定值重新更新数据
	main_page_ui_process(0);
	switch (Key5Value)
	{
	// 单击进入菜单选择
	case button_click:
	{
		// 进入选择界面
		ui_loging_flag = 0;     // 当按键按下时，将ui加载标志位置0，表示允许加载ui
		Enter_Page(SELECT_PAGE, Key5Value, Key0Value);
		break;
	}

	// 长按进入设置模式
	case button_longPressStart:
	{
		ui_loging_flag = 0;     // 当按键按下时，将ui加载标志位置0，表示允许加载ui
		Enter_Page(SETTING_PAGE, Key5Value, Key0Value);
		break;
	}

	default:
		break;
	}

#if 0 // 旧代码

//	//键值转换，从具体的按键如（单击，双击转换成每个页面的对应功能）
//	switch(Key5Value_transition_function(KeyValue))
//	{
//		case KEY_none:
//		{
//			break;
//		}
//    // 向上选择（翻页）
//		case KEY_up:
//      {
//         // 临界条件判断
//				 (sub_index.main_current_index > 0) ? (sub_index.main_current_index--) : (sub_index.main_current_index = 0)  ;
//				 main_page_ui_process(sub_index.main_current_index);    // 具体的UI绘制
//				 break ;
//      }
//	  // 向下翻页
//		case KEY_dowm:
//      {
//        // 临界条件判断
//				 (sub_index.main_current_index < 7 ) ? (sub_index.main_current_index++) : (sub_index.main_current_index = 7)  ;
//				 main_page_ui_process(sub_index.main_current_index);
//				 break ;
//       }
//				 
//		//在主页面的时候，短按ENTER按键进入对应的子页面
//		case KEY_enter:
//				 Enter_Page(sub_index.main_current_index,KeyValue);
//			break ;
//		
//		//在主页面的时候，只要触发了home按键即回到开机对应的页面
//		case KEY_home:
//				 Enter_Page(MAIN_PAGE,KeyValue);
//				 Menu_Main_Init();
//				 break ;
//		default:
//			break ;
//	}
#endif
}

/**
 * @brief       菜单页面处理
 * @param[in]   KeyValue ： 键值
 * @retval      none
 * @attention
 */
void select_page_process(button_status_e Key5Value, button_status_e Key0Value)
{
	select_page_ui_process();

	switch (Key5Value_transition_function(Key5Value, Key0Value))
	{
	case KEY_dowm:
	{
		// 临界条件判断
		(sub_index.select_current_index < 5) ? (sub_index.select_current_index++) : (sub_index.select_current_index = 1);
		display_pninter(sub_index.select_current_index);			// 指针显示
		Serial.println("down to choose");
		Serial.println(sub_index.select_current_index);
		break;
	}

	case KEY_up:
	{
        // 临界条件判断
		(sub_index.select_current_index > 1) ? (sub_index.select_current_index--) : (sub_index.select_current_index = 5)  ;
		display_pninter(sub_index.select_current_index);			// 指针显示
		Serial.println("up to choose");
		Serial.println(sub_index.select_current_index);
		//main_page_ui_process(sub_index.main_current_index);    // 具体的UI绘制
		break ;
	}
	case KEY_enter:
	{
		Serial.println("Enter the choice");
		Serial.println((sub_index.select_current_index));
		ui_loging_flag = 0;     // 当按键按下时，将ui加载标志位置0，表示允许加载ui
		Enter_Page(sub_index.select_current_index, button_none, button_none);
		break;
	}

	case KEY_home:
	{
		ui_loging_flag = 0;
		Enter_Page(MAIN_PAGE,button_none,button_none);
	//	Enter_Page(sub_index.setting_current_index, Key5Value, Key0Value);
		break;
	}

	case KEY_esc:
	{	//返回上一页
		ui_loging_flag = 0;     // 当按键按下时，将ui加载标志位置0，表示允许加载ui
		Enter_Page(MAIN_PAGE,button_none,button_none);
		break;
	}
	default:
		break;
	}
}

/**
 * @brief       设置页面处理
 * @param[in]   KeyValue ： 键值
 * @retval      none
 * @attention
 */
void setting_page_process(button_status_e Key5Value, button_status_e Key0Value)
{
	Serial.println("setting status");
	setting_page_ui_process();
	/*************************** 键值处理 **************************/
	switch (Key5Value_transition_function(Key5Value, Key0Value))
	{
	case KEY_dowm:
	{
		// 临界条件判断
		(sub_index.setting_current_index < 9)  ? (sub_index.setting_current_index++) : (sub_index.setting_current_index = 8);
		display_pninter(sub_index.setting_current_index);
		Serial.println("down to choose");
		Serial.println(sub_index.setting_current_index);
		// main_page_ui_process(sub_index.main_current_index);
		break;
	}

	case KEY_up:
	{
        // 临界条件判断
		(sub_index.setting_current_index > 8) ? (sub_index.setting_current_index--) : (sub_index.setting_current_index = 9)  ;
		display_pninter(sub_index.setting_current_index);
		Serial.println("up to choose");
		Serial.println(sub_index.setting_current_index);
		//main_page_ui_process(sub_index.main_current_index);    // 具体的UI绘制
		break ;
	}
	case KEY_enter:
	{
		Serial.println("Enter the choice");
		Serial.println((sub_index.setting_current_index));
		ui_loging_flag = 0;     // 当按键按下时，将ui加载标志位置0，表示允许加载ui
		Enter_Page((sub_index.setting_current_index), button_none, button_none);
		break;
	}

	case KEY_home:
	{
		ui_loging_flag = 0;
		Enter_Page(MAIN_PAGE,button_none,button_none);
	//	Enter_Page(sub_index.setting_current_index, Key5Value, Key0Value);
		break;
	}

	case KEY_esc:
	{	//返回上一页
		ui_loging_flag = 0;     // 当按键按下时，将ui加载标志位置0，表示允许加载ui
		Enter_Page(MAIN_PAGE,button_none,button_none);
		break;
	}
	default:
		break;
	}
}

/**
 * @brief       天气页面处理
 * @param[in]   KeyValue ： 键值
 * @retval      none
 * @attention
 */
void weather_page_process(button_status_e Key5Value, button_status_e Key0Value)
{
	Serial.println("weather status");
	RTC_get_data_count++;    // 计数增加，超过一定值重新更新数据
	weather_page_ui_process();
	switch (Key5Value_transition_function(Key5Value, Key0Value))
	{

	case KEY_home:
	{
		ui_loging_flag = 0;
		Enter_Page(MAIN_PAGE,button_none,button_none);
	//	Enter_Page(sub_index.setting_current_index, Key5Value, Key0Value);
		break;
	}

	case KEY_esc:
	{	//返回上一页
		ui_loging_flag = 0;     // 当按键按下时，将ui加载标志位置0，表示允许加载ui
		Enter_Page(SELECT_PAGE,button_none,button_none);
		break;
	}
	default:
		break;
	}		
}


/**
 * @brief       时钟页面处理
 * @param[in]   KeyValue ： 键值
 * @retval      none
 * @attention
 */
void clock_page_process(button_status_e Key5Value, button_status_e Key0Value)
{
	clock_page_ui_process();
	RTC_get_data_count++;    // 计数增加，超过一定值重新更新数据
	switch (Key5Value_transition_function(Key5Value, Key0Value))
	{

	case KEY_home:
	{
		ui_loging_flag = 0;
		Enter_Page(MAIN_PAGE,button_none,button_none);
	//	Enter_Page(sub_index.setting_current_index, Key5Value, Key0Value);
		break;
	}

	case KEY_esc:
	{	//返回上一页
		ui_loging_flag = 0;     // 当按键按下时，将ui加载标志位置0，表示允许加载ui
		Enter_Page(SELECT_PAGE,button_none,button_none);
		break;
	}
	default:
		break;
	}		
}

/**
 * @brief      	配置页面处理
 * @param[in]   KeyValue ： 键值
 * @retval      none
 * @attention
 */
void configuration_page_process(button_status_e Key5Value, button_status_e Key0Value)
{
	configuration_page_ui_process();
	switch (Key5Value_transition_function(Key5Value, Key0Value))
	{
	case KEY_dowm:
	{
		// 临界条件判断
		(sub_index.configuration_current_index < (20 + return_wifi_num() - 1)) ? (sub_index.configuration_current_index++) : (sub_index.configuration_current_index = 20 );
		display_pninter(sub_index.configuration_current_index);			// 指针显示
		Serial.println("down to choose");
		Serial.println(sub_index.configuration_current_index);
		break;
	}

	case KEY_up:
	{
        // 临界条件判断
		(sub_index.configuration_current_index > 20) ? (sub_index.configuration_current_index--) : (sub_index.configuration_current_index =  (20 + return_wifi_num() - 1))  ;
		display_pninter(sub_index.configuration_current_index);			// 指针显示
		Serial.println("up to choose");
		Serial.println(sub_index.configuration_current_index);
		break ;
	}

	case KEY_enter:
	{
		Serial.println("Enter the choice");
		Serial.println((sub_index.configuration_current_index));

		ui_loging_flag = 0;     // 当按键按下时，将ui加载标志位置0，表示允许加载ui
		WiFi.disconnect(1);//断开网络
		wifi_flag = 0;
		display_wifi_connect(sub_index.configuration_current_index);  // wifi连接及ui绘制
		Enter_Page(SELECT_PAGE,button_none,button_none);
		break;
	}
	case KEY_home:
	{
		ui_loging_flag = 0;
		Enter_Page(MAIN_PAGE,button_none,button_none);
	//	Enter_Page(sub_index.setting_current_index, Key5Value, Key0Value);
		break;
	}

	case KEY_esc:
	{	//返回上一页
		ui_loging_flag = 0;     // 当按键按下时，将ui加载标志位置0，表示允许加载ui
		Enter_Page(SELECT_PAGE,button_none,button_none);
		break;
	}
	default:
		break;
	}		
}

/**
 * @brief      	阅读页面处理
 * @param[in]   KeyValue ： 键值
 * @retval      none
 * @attention
 */
void read_page_process(button_status_e Key5Value, button_status_e Key0Value)
{
	uint8_t  page_num = ((return_file_num_flag() / 5 ) + 1);	//总页码数
	flie_current_num = (sub_index.read_current_index % 5 + (page_current_sta - 1 )*5);	// 书籍当前编号
	
	read_page_ui_process();
	switch (Key5Value_transition_function(Key5Value, Key0Value))
	{
	case KEY_dowm:
	{
		// 懒得优化，就这么写了，有点憨憨
		if(page_current_sta != page_num)
		{
		// 临界条件判断
			if(sub_index.read_current_index <  (34) )
			(sub_index.read_current_index++) ;
			else
			{
			(sub_index.read_current_index = 30 ); 
			page_current_sta++;
			}
		}
		else 	// 在最后一页，指针索引最大值改变
		{
			if(sub_index.read_current_index <  (30 + (return_file_num_flag() % 5)-1 ) )
			(sub_index.read_current_index++) ;
			else
			{
			(sub_index.read_current_index = 30 ); 
			page_current_sta++;
			}
		}
		if(page_current_sta >  page_num) page_current_sta = 1;  // 重返第一页


		Serial.printf("page=%d",page_current_sta);
		display_SD_file_dynamic_ui((sub_index.read_current_index % 30 ),page_current_sta);
		display_pninter(sub_index.read_current_index);			// 指针显示
		Serial.println("down to choose");
		Serial.println(sub_index.read_current_index);
		break;
	}

	case KEY_up:
	{
			// 临界条件判断
			if(sub_index.read_current_index > 30)
			(sub_index.read_current_index--) ;
			else
			{
			(sub_index.read_current_index =  (34 ) );
			page_current_sta--;
			}

		if(page_current_sta <  1) page_current_sta = page_num;  // 重返最后一页

		display_SD_file_dynamic_ui((sub_index.read_current_index % 30 ),page_current_sta);  
		display_pninter(sub_index.read_current_index);			// 指针显示
		Serial.printf("page=%d",page_current_sta);
		Serial.println("up to choose");
		Serial.println(sub_index.read_current_index);
		break ;
	}

	case KEY_enter:
	{
		Serial.println("Enter the choice");
		Serial.println((sub_index.read_current_index));

		ui_loging_flag = 0;     // 当按键按下时，将ui加载标志位置0，表示允许加载ui
	
		// 加载当前书籍数据进入data （书籍编号 = 当前页面索引 + 当前页面数*5）
		read_file_data(file_list_name[flie_current_num]);
		Enter_Page(BOOK_PAGE,button_none,button_none);
		break;
	}
	case KEY_home:
	{
		ui_loging_flag = 0;
		Enter_Page(MAIN_PAGE,button_none,button_none);
	//	Enter_Page(sub_index.setting_current_index, Key5Value, Key0Value);
		break;
	}

	case KEY_esc:
	{	//返回上一页
		ui_loging_flag = 0;     // 当按键按下时，将ui加载标志位置0，表示允许加载ui
		Enter_Page(SELECT_PAGE,button_none,button_none);
		break;
	}
	default:
		break;
	}		
}

/**
 * @brief      	游戏页面处理
 * @param[in]   KeyValue ： 键值
 * @retval      none
 * @attention
 */
void game_page_process(button_status_e Key5Value, button_status_e Key0Value)
{
	Serial.println("game status");
	word_page_ui_process();
	switch (Key5Value_transition_function(Key5Value, Key0Value))
	{

	case KEY_home:
	{
		ui_loging_flag = 0;
		Enter_Page(MAIN_PAGE,button_none,button_none);
	//	Enter_Page(sub_index.setting_current_index, Key5Value, Key0Value);
		break;
	}

	case KEY_esc:
	{	//返回上一页
		ui_loging_flag = 0;     // 当按键按下时，将ui加载标志位置0，表示允许加载ui
		Enter_Page(SELECT_PAGE,Key5Value,Key0Value);
		break;
	}
	default:
		break;
	}	
}

/**
 * @brief      	语言页面处理
 * @param[in]   KeyValue ： 键值
 * @retval      none
 * @attention
 */
void language_page_process(button_status_e Key5Value, button_status_e Key0Value)
{
	Serial.println("language status");
	language_page_ui_process();			// 静态UI绘制
	switch (Key5Value_transition_function(Key5Value, Key0Value))
	{

	case KEY_dowm:
	{
		// 临界条件判断
		(sub_index.language_current_index < 3) ? (sub_index.language_current_index++) : (sub_index.language_current_index = 2);
		display_pninter(sub_index.language_current_index);			// 指针显示
		Serial.println("down to choose");
		Serial.println(sub_index.language_current_index);
		break;
	}

	case KEY_up:
	{
        // 临界条件判断
		(sub_index.language_current_index > 2) ? (sub_index.language_current_index--) : (sub_index.language_current_index = 3)  ;
		display_pninter(sub_index.language_current_index);			// 指针显示
		Serial.println("up to choose");
		Serial.println(sub_index.language_current_index);
		break ;
	}

	case KEY_enter:
	{
		Serial.println("Enter the choice");
		Serial.println((sub_index.language_current_index));

		if(sub_index.language_current_index == 2)		language_choose_flag = 0; //中文
		else if(sub_index.language_current_index == 3)	language_choose_flag = 1; //英文

		ui_loging_flag = 0;     // 当按键按下时，将ui加载标志位置0，表示允许加载ui
		Enter_Page(MAIN_PAGE,button_none,button_none);;
		break;
	}
	case KEY_home:
	{
		ui_loging_flag = 0;
		Enter_Page(MAIN_PAGE,button_none,button_none);
	//	Enter_Page(sub_index.setting_current_index, Key5Value, Key0Value);
		break;
	}

	case KEY_esc:
	{	//返回上一页
		ui_loging_flag = 0;     // 当按键按下时，将ui加载标志位置0，表示允许加载ui
		Enter_Page(SETTING_PAGE,button_none,button_none);
		break;
	}
	default:
		break;
	}	
}


/**
 * @brief       字号页面处理
 * @param[in]   KeyValue ： 键值
 * @retval      none
 * @attention
 */
void word_page_process(button_status_e Key5Value, button_status_e Key0Value)
{
	Serial.println("word status"); 
	word_page_ui_process();
	switch (Key5Value_transition_function(Key5Value, Key0Value))
	{
	case KEY_home:
	{
		ui_loging_flag = 0;
		Enter_Page(MAIN_PAGE,button_none,button_none);
	//	Enter_Page(sub_index.setting_current_index, Key5Value, Key0Value);
		break;
	}

	case KEY_esc:
	{	//返回上一页
		ui_loging_flag = 0;     // 当按键按下时，将ui加载标志位置0，表示允许加载ui
		Enter_Page(SETTING_PAGE,button_none,button_none);
		break;
	}
	default:
		break;
	}
}

/**
 * @brief       字号页面处理
 * @param[in]   KeyValue ： 键值
 * @retval      none
 * @attention
 */
static uint16_t book_page_current_index = 0;		//
void book_page_process(button_status_e Key5Value, button_status_e Key0Value)
{	
	book_page_ui_process();
	
	switch (Key5Value_transition_function(Key5Value, Key0Value))
	{

	case KEY_dowm:
	{
		if(file_list_name[flie_current_num] == "word.txt")		//单词本独立格式显示
		show_type2(file_last_read[flie_current_num][2] ,flie_current_num);
		else
		show_type1(file_last_read[flie_current_num][2] ,flie_current_num);

		Serial.println("down to choose");
		Serial.println(sub_index.configuration_current_index);
		break;
	}

	case KEY_up:
	{
		if(file_list_name[flie_current_num] == "word.txt")		//单词本独立格式显示
		show_type2(file_last_read[flie_current_num][0] ,flie_current_num);
		else
		show_type1(file_last_read[flie_current_num][0]  ,flie_current_num);
		Serial.println("up to choose");
		Serial.println(sub_index.configuration_current_index);
		break ;
	}		
	case KEY_home:
	{
		ui_loging_flag = 0;
		Enter_Page(MAIN_PAGE,button_none,button_none);
	//	Enter_Page(sub_index.setting_current_index, Key5Value, Key0Value);
		break;
	}

	case KEY_esc:
	{	//返回上一页
		ui_loging_flag = 0;     // 当按键按下时，将ui加载标志位置0，表示允许加载ui
		Enter_Page(READ_PAGE,button_none,button_none);
		break;
	}
	default:
		break;
	}

}


/**
  ****************************** Y.Z.T.****************************************
  * @file       menu.c/h
  * @brief      提供键值适配等UI菜单框架数据
  * @note       框架移植自STM32工程，因本墨水屏项目UI子菜单仅1层，有些函数不是很必要，故稍显臃肿和混乱   
  * @history    2022.7.8
  *
  @verbatim     v1.0
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************** Y.Z.T. *****************************************
  */

#include "menu.h"
#include <Arduino.h>
#include "menu_ui.h"

// 定义菜单索引变量
Key_Index sub_index ;

static key_value_e Key5Value_transition_function(button_status_e button5);
void Menu_Select_Item(menu_i32 current_index, menu_u8 KeyValue);




//菜单操作表定义
static OP_MENU_PAGE g_opStruct[] = 
{
	{MAIN_PAGE          , main_page_process},
	{CLOCK_PAGE         , clock_page_process},
	{WEATHER_PAGE       , weather_page_process},
	{CONFIGURATION_PAGE , configuration_page_process},
	{READ_PAGE          , read_page_process},
	{GAME_PAGE          , game_page_process},
	{SETTING_PAGE		, setting_page_process},
	{SELECT_PAGE        , select_page_process}
};


//跳转到表所对应的页面
static int JUMP_Table(menu_i32 op , button_status_e KeyValue)
{
	if(op >= sizeof(g_opStruct)/sizeof(g_opStruct[0]) || op < 0)   // 判断该页面是否在表内（1~ 7 ）
	{
		 Serial.println("unknow operate!");
		return -1;
	}
	g_opStruct[op].opfun(KeyValue);			// 执行操作
	return 0 ;
}

//菜单选择项
void Menu_Select_Item(menu_i32 current_index, button_status_e KeyValue)
{
	JUMP_Table(current_index , KeyValue);
}

/**
 * @brief       
 * @param[in]   Key5Value ： 左键key5键值（左） ； Key0Value（右）
 * @retval      在loop函数调用
 * @attention
 */
void Menu_Select_main(button_status_e Key5Value, button_status_e Key0Value)
{
	Enter_Page(sub_index.Current_Page ,Key5Value);
}



//选择菜单项
void Select_Main_Menu_Item(button_status_e item)
{


}

/**
 * @brief       键值转换，从具体的按键如（单击，双击转换成每个页面的对应功能）
 * @param[in]   
 * @retval      KeyValue ： 键值
 * @attention
 */
static key_value_e Key5Value_transition_function(button_status_e button5)
{
	switch (button5)
	{
	// key5 短按一下向下	
	case button_click:
		{
			return KEY_dowm;
			break;
		}
	// key5 短按2下确定
	case button_doubleclick:
		{
			return KEY_enter;
			break;
		}

	// key5 长按进入设置模式
	case button_longPressStart:
		{
   
			return  KEY_setting;           
			break;
		}

	// key0 长按返回主页面
	case button_longPressStart2:
		{
   
			return  KEY_home;           
			break;
		}	
	
	// key0 短按向上
	case button_click2:
		{
			return      KEY_up;           
			break;
		}	
	
	//key0 短按2下返回
	case button_doubleclick2:
		{
			return      KEY_esc;           
			break;
		}	
	
	default:
		break;
	}
	return KEY_none;
 }

/**
 * @brief       主页面处理
 * @param[in]   KeyValue ： 键值
 * @retval      none
 * @attention
 */
void main_page_process(button_status_e KeyValue)
{
			switch(KeyValue)
		{
			// 长按进入菜单选择
			case button_longPressStart:
			{
				// 进入选择界面
				Enter_Page(SELECT_PAGE, KeyValue);
				break;
			}
			
			// 单击进入设置模式
			case button_click:
			{
				Enter_Page(SETTING_PAGE,KeyValue);
				break;
			}

			default:
				break;
		}

#if 0		// 旧代码

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
void select_page_process(button_status_e KeyValue)
{
	Serial.println("select status");
	switch (Key5Value_transition_function(KeyValue))
	{
	case KEY_dowm:
      {
        // 临界条件判断
		(sub_index.select_current_index<= 5 ) ? (sub_index.select_current_index++) : (sub_index.select_current_index = 5)  ;
		Serial.println("down to choose");
		Serial.println(sub_index.select_current_index);
		break ;
       }

	case  KEY_enter:
	{
		Enter_Page((sub_index.select_current_index + 1),KeyValue);	
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
void clock_page_process(button_status_e KeyValue)
{
	Serial.println("clock status");

}


/**
 * @brief       设置页面处理
 * @param[in]   KeyValue ： 键值
 * @retval      none
 * @attention
 */
void setting_page_process(button_status_e KeyValue)
{
	Serial.println("setting status");
	switch (Key5Value_transition_function(KeyValue))
	{
	case KEY_dowm:
      {
        // 临界条件判断
		(sub_index.setting_current_index<= 5 ) ? (sub_index.setting_current_index++) : (sub_index.setting_current_index = 5)  ;
		Serial.println("down to choose");
		Serial.println(sub_index.setting_current_index);
		//main_page_ui_process(sub_index.main_current_index);
		break ;
       }

	case  KEY_enter:
	{
		Serial.println("Enter the choice");
		Serial.println((sub_index.setting_current_index + 1));
		Enter_Page((sub_index.setting_current_index + 1),KeyValue);	
	} 

	case KEY_home:
	{
		sub_index.setting_current_index = 0;
		Enter_Page(sub_index.setting_current_index,KeyValue);
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
void weather_page_process(button_status_e KeyValue)
{
	Serial.println("weather status");


}


/**
 * @brief      	配置页面处理
 * @param[in]   KeyValue ： 键值
 * @retval      none
 * @attention
 */
void configuration_page_process(button_status_e KeyValue)
{
	Serial.println("configuration status");

	
}

/**
 * @brief      	阅读页面处理
 * @param[in]   KeyValue ： 键值
 * @retval      none
 * @attention
 */
void read_page_process(button_status_e KeyValue)
{
	Serial.println("read status");


}

/**
 * @brief      	游戏页面处理
 * @param[in]   KeyValue ： 键值
 * @retval      none
 * @attention
 */
void game_page_process(button_status_e KeyValue)
{
	Serial.println("game status");
}
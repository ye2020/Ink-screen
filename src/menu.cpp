/**
  ****************************** Y.Z.T.****************************************
  * @file       menu.c/h
  * @brief      �ṩ��ֵ�����UI�˵��������
  * @note       �����ֲ��STM32���̣���īˮ����ĿUI�Ӳ˵���1�㣬��Щ�������Ǻܱ�Ҫ��������ӷ�׺ͻ���   
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

// ����˵���������
Key_Index sub_index ;

static key_value_e Key5Value_transition_function(button_status_e button5);
void Menu_Select_Item(menu_i32 current_index, menu_u8 KeyValue);




//�˵���������
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


//��ת��������Ӧ��ҳ��
static int JUMP_Table(menu_i32 op , button_status_e KeyValue)
{
	if(op >= sizeof(g_opStruct)/sizeof(g_opStruct[0]) || op < 0)   // �жϸ�ҳ���Ƿ��ڱ��ڣ�1~ 7 ��
	{
		 Serial.println("unknow operate!");
		return -1;
	}
	g_opStruct[op].opfun(KeyValue);			// ִ�в���
	return 0 ;
}

//�˵�ѡ����
void Menu_Select_Item(menu_i32 current_index, button_status_e KeyValue)
{
	JUMP_Table(current_index , KeyValue);
}

/**
 * @brief       
 * @param[in]   Key5Value �� ���key5��ֵ���� �� Key0Value���ң�
 * @retval      ��loop��������
 * @attention
 */
void Menu_Select_main(button_status_e Key5Value, button_status_e Key0Value)
{
	Enter_Page(sub_index.Current_Page ,Key5Value);
}



//ѡ��˵���
void Select_Main_Menu_Item(button_status_e item)
{


}

/**
 * @brief       ��ֵת�����Ӿ���İ����磨������˫��ת����ÿ��ҳ��Ķ�Ӧ���ܣ�
 * @param[in]   
 * @retval      KeyValue �� ��ֵ
 * @attention
 */
static key_value_e Key5Value_transition_function(button_status_e button5)
{
	switch (button5)
	{
	// key5 �̰�һ������	
	case button_click:
		{
			return KEY_dowm;
			break;
		}
	// key5 �̰�2��ȷ��
	case button_doubleclick:
		{
			return KEY_enter;
			break;
		}

	// key5 ������������ģʽ
	case button_longPressStart:
		{
   
			return  KEY_setting;           
			break;
		}

	// key0 ����������ҳ��
	case button_longPressStart2:
		{
   
			return  KEY_home;           
			break;
		}	
	
	// key0 �̰�����
	case button_click2:
		{
			return      KEY_up;           
			break;
		}	
	
	//key0 �̰�2�·���
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
 * @brief       ��ҳ�洦��
 * @param[in]   KeyValue �� ��ֵ
 * @retval      none
 * @attention
 */
void main_page_process(button_status_e KeyValue)
{
			switch(KeyValue)
		{
			// ��������˵�ѡ��
			case button_longPressStart:
			{
				// ����ѡ�����
				Enter_Page(SELECT_PAGE, KeyValue);
				break;
			}
			
			// ������������ģʽ
			case button_click:
			{
				Enter_Page(SETTING_PAGE,KeyValue);
				break;
			}

			default:
				break;
		}

#if 0		// �ɴ���

//	//��ֵת�����Ӿ���İ����磨������˫��ת����ÿ��ҳ��Ķ�Ӧ���ܣ�
//	switch(Key5Value_transition_function(KeyValue))
//	{
//		case KEY_none:
//		{
//			break;
//		}
//    // ����ѡ�񣨷�ҳ��
//		case KEY_up:
//      {
//         // �ٽ������ж�
//				 (sub_index.main_current_index > 0) ? (sub_index.main_current_index--) : (sub_index.main_current_index = 0)  ;
//				 main_page_ui_process(sub_index.main_current_index);    // �����UI����
//				 break ;
//      }
//	  // ���·�ҳ
//		case KEY_dowm:
//      {
//        // �ٽ������ж�
//				 (sub_index.main_current_index < 7 ) ? (sub_index.main_current_index++) : (sub_index.main_current_index = 7)  ;
//				 main_page_ui_process(sub_index.main_current_index);
//				 break ;
//       }
//				 
//		//����ҳ���ʱ�򣬶̰�ENTER���������Ӧ����ҳ��
//		case KEY_enter:
//				 Enter_Page(sub_index.main_current_index,KeyValue);
//			break ;
//		
//		//����ҳ���ʱ��ֻҪ������home�������ص�������Ӧ��ҳ��
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
 * @brief       �˵�ҳ�洦��
 * @param[in]   KeyValue �� ��ֵ
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
        // �ٽ������ж�
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
 * @brief       ʱ��ҳ�洦��
 * @param[in]   KeyValue �� ��ֵ
 * @retval      none
 * @attention
 */
void clock_page_process(button_status_e KeyValue)
{
	Serial.println("clock status");

}


/**
 * @brief       ����ҳ�洦��
 * @param[in]   KeyValue �� ��ֵ
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
        // �ٽ������ж�
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
 * @brief       ����ҳ�洦��
 * @param[in]   KeyValue �� ��ֵ
 * @retval      none
 * @attention
 */
void weather_page_process(button_status_e KeyValue)
{
	Serial.println("weather status");


}


/**
 * @brief      	����ҳ�洦��
 * @param[in]   KeyValue �� ��ֵ
 * @retval      none
 * @attention
 */
void configuration_page_process(button_status_e KeyValue)
{
	Serial.println("configuration status");

	
}

/**
 * @brief      	�Ķ�ҳ�洦��
 * @param[in]   KeyValue �� ��ֵ
 * @retval      none
 * @attention
 */
void read_page_process(button_status_e KeyValue)
{
	Serial.println("read status");


}

/**
 * @brief      	��Ϸҳ�洦��
 * @param[in]   KeyValue �� ��ֵ
 * @retval      none
 * @attention
 */
void game_page_process(button_status_e KeyValue)
{
	Serial.println("game status");
}
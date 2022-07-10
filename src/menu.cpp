/**
  ****************************** Y.Z.T.****************************************
  * @file       menu.c/h
  * @brief      �ṩ��ֵ�����UI�˵��������
  * @note       �����ֲ��STM32���̣���īˮ����ĿUI�Ӳ˵���2�㣬��Щ�������Ǻܱ�Ҫ��������ӷ�׺ͻ���
  * @history    2022.7.8
  *
  @verbatim     v1.0
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************** Y.Z.T. *****************************************
  */
/***************************** ����˵�� ******************************
 * 	
 * 		����˵����
 * 			�����key5����
 * 				����������ѡ�� ������ҳ����˵�ҳ�棩	
 * 				˫����ȷ��enter
 * 				��������ҳ��������ģʽ
 * 			
 *			�Ҽ���key0��: 
 * 				����������ѡ��
 * 				˫��������esc
 * 				������������ҳhome
 * 
 * 		�ر�ע�⣺��������ͬʱ����ͬʱ��������ָ��
*******************************************************************/


#include "menu.h"
#include <Arduino.h>
#include "menu_ui.h"

// ����˵���������
Key_Index sub_index;

static key_value_e Key5Value_transition_function(button_status_e button5, button_status_e button0);
void Menu_Select_Item(menu_i32 current_index, button_status_e Key5Value, button_status_e Key0Value);

//�˵���������
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
		{WORD_PAGE, word_page_process}	
	};


//��ת��������Ӧ��ҳ��
static int JUMP_Table(menu_i32 op, button_status_e Key5Value, button_status_e Key0Value)
{
	if (op >= sizeof(g_opStruct) / sizeof(g_opStruct[0]) || op < 0) // �жϸ�ҳ���Ƿ��ڱ��ڣ�1~ 7 ��
	{
		Serial.println("unknow operate!");
		return -1;
	}
	g_opStruct[op].opfun(Key5Value, Key0Value); // ִ�в���
	return 0;
}

//�˵�ѡ����
void Menu_Select_Item(menu_i32 current_index, button_status_e Key5Value, button_status_e Key0Value)
{
	JUMP_Table(current_index, Key5Value, Key0Value);
}

/**
 * @brief
 * @param[in]   Key5Value �� ���key5��ֵ���� �� Key0Value���ң�
 * @retval      ��loop��������
 * @attention
 */
void Menu_Select_main(button_status_e Key5Value, button_status_e Key0Value)
{
	Enter_Page(sub_index.Current_Page, Key5Value, Key0Value);
}

/**
 * @brief       ���ֵת�����Ӿ���İ����磨������˫��ת����ÿ��ҳ��Ķ�Ӧ���ܣ�
 * @param[in]   button5 (��)
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

	/**************** �󰴼���Ӧ��ֵ *******************/
	// key5 �̰�һ������
	case button_click:
	{
		Serial.println("KEY_dowm");		
		return KEY_dowm;
		break;
	}
	// key5 �̰�2��ȷ��
	case button_doubleclick:
	{
		Serial.println("KEY_enter");		
		return KEY_enter;
		break;
	}

	// key5 ������������ģʽ
	case button_longPressStop:
	{
		Serial.println("KEY_setting");		
		return KEY_setting;
		break;
	}

	/**************** �Ұ�����Ӧ��ֵ *******************/

	// key0 ����������ҳ��
	case button_longPressStop2:
	{
		Serial.println("KEY_home");		
		return KEY_home;
		break;
	}

	// key0 �̰�����
	case button_click2:
	{
		Serial.println("KEY_up");		
		return KEY_up;
		break;
	}

	// key0 �̰�2�·���
	case button_doubleclick2:
	{
		Serial.println("KEY_esc");		
		return KEY_esc;
		break;
	}
	default:
		break;
	}

	/**************** �Ұ�����Ӧ��ֵ *******************/
	// switch(buttton0)
	// {

	// default:
	// 	break;
	// }

	return KEY_none;
}


/************************************************************************************************************************************/
/*********************************************************** ���̴��� **********************************************************/
/************************************************************************************************************************************/

/**
 * @brief       ��ҳ�洦��
 * @param[in]   KeyValue �� ��ֵ
 * @retval      none
 * @attention
 */
void main_page_process(button_status_e Key5Value, button_status_e Key0Value)
{
	switch (Key5Value)
	{
	// ��������˵�ѡ��
	case button_click:
	{
		// ����ѡ�����
		Enter_Page(SELECT_PAGE, Key5Value, Key0Value);
		break;
	}

	// ������������ģʽ
	case button_longPressStart:
	{
		Enter_Page(SETTING_PAGE, Key5Value, Key0Value);
		break;
	}

	default:
		break;
	}

#if 0 // �ɴ���

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
void select_page_process(button_status_e Key5Value, button_status_e Key0Value)
{
	Serial.println("select status");
	switch (Key5Value_transition_function(Key5Value, Key0Value))
	{
	case KEY_dowm:
	{
		// �ٽ������ж�
		(sub_index.select_current_index < 5) ? (sub_index.select_current_index++) : (sub_index.select_current_index = 5);
		Serial.println("down to choose");
		Serial.println(sub_index.select_current_index);
		break;
	}

	case KEY_up:
	{
        // �ٽ������ж�
		(sub_index.select_current_index > 1) ? (sub_index.select_current_index--) : (sub_index.select_current_index = 1)  ;
		Serial.println("up to choose");
		Serial.println(sub_index.select_current_index);
		//main_page_ui_process(sub_index.main_current_index);    // �����UI����
		break ;
	}
	case KEY_enter:
	{
		Serial.println("Enter the choice");
		Serial.println((sub_index.select_current_index));
		Enter_Page(sub_index.select_current_index, Key5Value, Key0Value);
		break;
	}

	case KEY_home:
	{
		Menu_Main_Init();
	//	Enter_Page(sub_index.setting_current_index, Key5Value, Key0Value);
		break;
	}

	case KEY_esc:
	{	//������һҳ
		Enter_Page(MAIN_PAGE,Key5Value,Key0Value);
		break;
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
void setting_page_process(button_status_e Key5Value, button_status_e Key0Value)
{
	//Serial.println("setting status");

	/*************************** ��ֵ���� **************************/
	switch (Key5Value_transition_function(Key5Value, Key0Value))
	{
	case KEY_dowm:
	{
		// �ٽ������ж�
		(sub_index.setting_current_index < 9)  ? (sub_index.setting_current_index++) : (sub_index.setting_current_index = 9);
		Serial.println("down to choose");
		Serial.println(sub_index.setting_current_index);
		// main_page_ui_process(sub_index.main_current_index);
		break;
	}

	case KEY_up:
	{
        // �ٽ������ж�
		(sub_index.setting_current_index > 8) ? (sub_index.setting_current_index--) : (sub_index.setting_current_index = 8)  ;
		Serial.println("up to choose");
		Serial.println(sub_index.setting_current_index);
		//main_page_ui_process(sub_index.main_current_index);    // �����UI����
		break ;
	}
	case KEY_enter:
	{
		Serial.println("Enter the choice");
		Serial.println((sub_index.setting_current_index));
		Enter_Page((sub_index.setting_current_index), Key5Value, Key0Value);
		break;
	}

	case KEY_home:
	{
		Menu_Main_Init();
	//	Enter_Page(sub_index.setting_current_index, Key5Value, Key0Value);
		break;
	}

	case KEY_esc:
	{	//������һҳ
		Enter_Page(MAIN_PAGE,Key5Value,Key0Value);
		break;
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
void weather_page_process(button_status_e Key5Value, button_status_e Key0Value)
{
	Serial.println("weather status");
	switch (Key5Value_transition_function(Key5Value, Key0Value))
	{

	case KEY_home:
	{
		Menu_Main_Init();
	//	Enter_Page(sub_index.setting_current_index, Key5Value, Key0Value);
		break;
	}

	case KEY_esc:
	{	//������һҳ
		Enter_Page(SELECT_PAGE,Key5Value,Key0Value);
		break;
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
void clock_page_process(button_status_e Key5Value, button_status_e Key0Value)
{
	Serial.println("clock status");
	switch (Key5Value_transition_function(Key5Value, Key0Value))
	{

	case KEY_home:
	{
		Menu_Main_Init();
	//	Enter_Page(sub_index.setting_current_index, Key5Value, Key0Value);
		break;
	}

	case KEY_esc:
	{	//������һҳ
		Enter_Page(SELECT_PAGE,Key5Value,Key0Value);
		break;
	}
	default:
		break;
	}		
}

/**
 * @brief      	����ҳ�洦��
 * @param[in]   KeyValue �� ��ֵ
 * @retval      none
 * @attention
 */
void configuration_page_process(button_status_e Key5Value, button_status_e Key0Value)
{
	Serial.println("configuration status");
	switch (Key5Value_transition_function(Key5Value, Key0Value))
	{

	case KEY_home:
	{
		Menu_Main_Init();
	//	Enter_Page(sub_index.setting_current_index, Key5Value, Key0Value);
		break;
	}

	case KEY_esc:
	{	//������һҳ
		Enter_Page(SELECT_PAGE,Key5Value,Key0Value);
		break;
	}
	default:
		break;
	}		
}

/**
 * @brief      	�Ķ�ҳ�洦��
 * @param[in]   KeyValue �� ��ֵ
 * @retval      none
 * @attention
 */
void read_page_process(button_status_e Key5Value, button_status_e Key0Value)
{
	Serial.println("read status");
	switch (Key5Value_transition_function(Key5Value, Key0Value))
	{

	case KEY_home:
	{
		Menu_Main_Init();
	//	Enter_Page(sub_index.setting_current_index, Key5Value, Key0Value);
		break;
	}

	case KEY_esc:
	{	//������һҳ
		Enter_Page(SELECT_PAGE,Key5Value,Key0Value);
		break;
	}
	default:
		break;
	}		
}

/**
 * @brief      	��Ϸҳ�洦��
 * @param[in]   KeyValue �� ��ֵ
 * @retval      none
 * @attention
 */
void game_page_process(button_status_e Key5Value, button_status_e Key0Value)
{
	Serial.println("game status");
	switch (Key5Value_transition_function(Key5Value, Key0Value))
	{

	case KEY_home:
	{
		Menu_Main_Init();
	//	Enter_Page(sub_index.setting_current_index, Key5Value, Key0Value);
		break;
	}

	case KEY_esc:
	{	//������һҳ
		Enter_Page(SELECT_PAGE,Key5Value,Key0Value);
		break;
	}
	default:
		break;
	}	
}

/**
 * @brief      	����ҳ�洦��
 * @param[in]   KeyValue �� ��ֵ
 * @retval      none
 * @attention
 */
void language_page_process(button_status_e Key5Value, button_status_e Key0Value)
{
	Serial.println("language status");

	switch (Key5Value_transition_function(Key5Value, Key0Value))
	{

	case KEY_home:
	{
		Menu_Main_Init();
	//	Enter_Page(sub_index.setting_current_index, Key5Value, Key0Value);
		break;
	}

	case KEY_esc:
	{	//������һҳ
		Enter_Page(SETTING_PAGE,Key5Value,Key0Value);
		break;
	}
	default:
		break;
	}	
}


/**
 * @brief       �ֺ�ҳ�洦��
 * @param[in]   KeyValue �� ��ֵ
 * @retval      none
 * @attention
 */
void word_page_process(button_status_e Key5Value, button_status_e Key0Value)
{
	Serial.println("word status"); 

	switch (Key5Value_transition_function(Key5Value, Key0Value))
	{

	case KEY_home:
	{
		Menu_Main_Init();
	//	Enter_Page(sub_index.setting_current_index, Key5Value, Key0Value);
		break;
	}

	case KEY_esc:
	{	//������һҳ
		Enter_Page(SETTING_PAGE,Key5Value,Key0Value);
		break;
	}
	default:
		break;
	}

}

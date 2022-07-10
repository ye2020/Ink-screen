/**
  ****************************** Y.Z.T.****************************************
  * @file       menu_ui.c/h
  * @brief      �ṩ�˵������UI�Ĵ���
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

//���˵���ʼ��
void Menu_Main_Init(void)
{
	Serial.println("home status");

	sub_index.main_current_index = 0;
	sub_index.setting_current_index = 0;
	sub_index.Current_Page = MAIN_PAGE;
	//.....ˢ�»���ҳ���UI��״̬(�ȴ����ӳ�ʼ���˵�)
}

//������Ӧ��ҳ��
void Enter_Page(menu_i32 index, button_status_e KeyValue)
{
	sub_index.Current_Page = index;
	switch (sub_index.Current_Page)
	{
		//������ҳ��
	case MAIN_PAGE:
	{
		Menu_Select_Item(MAIN_PAGE, KeyValue);
		break;
	}
		// ����ʱ��ҳ��
	case CLOCK_PAGE:
	{
		Menu_Select_Item(CLOCK_PAGE, KeyValue);
		break;
	}
		// 	������������
	case WEATHER_PAGE:
	{
		Menu_Select_Item(WEATHER_PAGE, KeyValue);
		break;
	}
	//��������ҳ��
	case CONFIGURATION_PAGE:
	{
		Menu_Select_Item(CONFIGURATION_PAGE, KeyValue);
		break;
	}
	//�����Ķ�ҳ��
	case READ_PAGE:
	{
		Menu_Select_Item(READ_PAGE, KeyValue);
		break;
	}
	//������Ϸҳ��
	case GAME_PAGE:
	{
		Menu_Select_Item(GAME_PAGE, KeyValue);
		break;
	}
	//��������ҳ��
	case SETTING_PAGE:
	{
		Menu_Select_Item(SETTING_PAGE, KeyValue);
		break;
	}
	// ����˵�ҳ��
	case SELECT_PAGE:
	{
		Menu_Select_Item(SELECT_PAGE, KeyValue);
		break;
	}
	default:
		break;
	}
}

//��ҳ��UI����
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
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
#include "main.h"
#include "GxEPD2_BW.h"
#include "Display_setup.h"


//���˵���ʼ��
void Menu_Main_Init(void)
{
	Serial.println("home status");

	sub_index.main_current_index = 0;
	sub_index.setting_current_index = 8;
	sub_index.select_current_index = 1;
	sub_index.Current_Page = MAIN_PAGE;

  display.fillScreen(baise);  // �����Ļ
	display.drawInvertedBitmap(50, 3, Bitmap_m, 45, 45, heise);
   BW_refresh();          //�ڰ�ˢ��һ��



	//.....ˢ�»���ҳ���UI��״̬(�ȴ���ӳ�ʼ���˵�)
}

//������Ӧ��ҳ��
void Enter_Page(menu_i32 index, button_status_e Key5Value , button_status_e Key0Value)
{
	
	sub_index.Current_Page = index;
	switch (sub_index.Current_Page)
	{
		//������ҳ��
	case MAIN_PAGE:
	{
		Menu_Select_Item(MAIN_PAGE, Key5Value,Key0Value);
		break;
	}
		// ����ʱ��ҳ��
	case CLOCK_PAGE:
	{
		Menu_Select_Item(CLOCK_PAGE,  Key5Value,Key0Value);
		break;
	}
		// 	������������
	case WEATHER_PAGE:
	{
		Menu_Select_Item(WEATHER_PAGE,  Key5Value,Key0Value);
		break;
	}
	//��������ҳ��
	case CONFIGURATION_PAGE:
	{
		Menu_Select_Item(CONFIGURATION_PAGE,  Key5Value,Key0Value);
		break;
	}
	//�����Ķ�ҳ��
	case READ_PAGE:
	{
		Menu_Select_Item(READ_PAGE,  Key5Value,Key0Value);
		break;
	}
	//������Ϸҳ��
	case GAME_PAGE:
	{
		Menu_Select_Item(GAME_PAGE,  Key5Value,Key0Value);
		break;
	}
	//��������ҳ��
	case SETTING_PAGE:
	{
		Menu_Select_Item(SETTING_PAGE,  Key5Value,Key0Value);
		break;
	}
	//��������ҳ��
	case SELECT_PAGE:
	{
		Menu_Select_Item(SELECT_PAGE,  Key5Value,Key0Value);
		break;
	}
	// ����˵�ҳ��
	case LANGUAGE_PAGE:
	{
		Menu_Select_Item(LANGUAGE_PAGE,  Key5Value, Key0Value);
		break;
	}
	// ����˵�ҳ��
	case WORD_PAGE:
	{
		Menu_Select_Item(WORD_PAGE,  Key5Value, Key0Value);
		break;
	}

	// ���򷵻���ҳ��
	default:
	{	Menu_Select_Item(MAIN_PAGE, Key5Value,Key0Value);
		break;
	}
	}
}

//��ҳ��UI����
void main_page_ui_process(menu_u8 index)
{
	if(ui_loging_flag == 0)		// ����ui����
	{
		BW_refresh();          //�ڰ�ˢ��һ��
		BW_refresh();          //�ڰ�ˢ��һ��

		display_main_home();

		ui_loging_flag = 1;
	}
}


//����ҳ��UI����
void weather_page_ui_process(void)
{
	if(ui_loging_flag == 0)
	{
		BW_refresh();          //�ڰ�ˢ��һ��
		BW_refresh();
		get_time_weather();  //���wifi_flag Ϊ1�������ˢ��ʱ�������

		ui_loging_flag = 1;
	}
}



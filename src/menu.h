/**
  ****************************** Y.Z.T.****************************************
  * @file       menu.c/h
  * @brief      �ṩ��ֵ�����UI�˵��������
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


// ��Ӧ�������Ͷ���
typedef unsigned int   menu_u32;
typedef unsigned short menu_u16;
typedef unsigned char  menu_u8;

typedef int menu_i32 ;
typedef short menu_s16 ;
typedef char menu_s8 ;


// ��ֵ����ö��
typedef enum 
{
    KEY_none  = 0,
	  KEY_enter,          // ȷ��
    KEY_esc,            // ����
    KEY_up,             // ����
    KEY_dowm,           // ����
    KEY_home,           // home��
    KEY_setting,        // ���ü�
    KEY_game_lift,      // ��ת
    KEY_game_right,     // ��ת

}key_value_e;

// �������������Ӳ˵�
typedef struct Menu_Key_Index
{
	menu_u8 main_current_index ;
	menu_u8 setting_current_index ; 
	menu_u8 select_current_index ; 

	
	menu_u8 Current_Page ;
}Key_Index;


extern Key_Index sub_index ;

typedef void (*menu_op_func)(button_status_e , button_status_e);

typedef struct OP_STRUCT
{
	int op_menu ; 				//�����˵�
	menu_op_func opfun ;	//��������
}OP_MENU_PAGE;

/*��ǰ�˵�*/
typedef enum 
{
	MAIN_PAGE = 0,          // ��ҳ��						
	CLOCK_PAGE,             // ʱ��״̬ 
	WEATHER_PAGE,           // ����״̬
	CONFIGURATION_PAGE,     // ����״̬
	READ_PAGE,              // �Ķ�״̬
	GAME_PAGE,              // ��Ϸ״̬ ��������select�µ��ӱ���
  SETTING_PAGE,           // ����ģʽ��main�µ��ӱ���
	SELECT_PAGE,						// �˵�    ��main�µ��ӱ���
  LANGUAGE_PAGE,          // �������ã�setting�µ��ӱ���
  WORD_PAGE,              // �ֺ����ã�setting�µ��ӱ���
	
}OP_PAGE;



void Menu_Select_Item(menu_i32 current_index, button_status_e Key5Value , button_status_e Key0Value);

/******************* ���̴��� *********************/

void select_page_process(button_status_e Key5Value , button_status_e Key0Value);
void main_page_process(button_status_e Key5Value , button_status_e Key0Value);
void clock_page_process(button_status_e Key5Value , button_status_e Key0Value);
void weather_page_process(button_status_e Key5Value , button_status_e Key0Value);
void configuration_page_process(button_status_e Key5Value , button_status_e Key0Value);
void read_page_process(button_status_e Key5Value , button_status_e Key0Value);
void game_page_process(button_status_e Key5Value , button_status_e Key0Value);
void setting_page_process(button_status_e Key5Value , button_status_e Key0Value);
void Menu_Select_main(button_status_e Key5Value , button_status_e Key0Value);
void language_page_process(button_status_e Key5Value, button_status_e Key0Value);
void word_page_process(button_status_e Key5Value, button_status_e Key0Value);





#endif
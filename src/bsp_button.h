/**
  ****************************** Y.Z.T.****************************************
  * @file       bsp_button.c/h
  * @brief      ���ڴ�Ű����๦�ܼ�����غ����ͽṹ��
  * @note       ���������Ķ̰���������˫���Ȳ�������   
  * @history    2022.7.8
  *
  @verbatim     v1.0
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************** Y.Z.T. *****************************************
  */
#ifndef _BSP_BUTTON_H
#define _BSP_BUTTON_H


#include "Arduino.h"

// ����ö�ٱ��� ������ǰ״̬
typedef enum
{
    button_none = 0,        // ʲô��û��
	button_click = 1,       // �����̰�1��
    button_doubleclick ,          // �����̰�2��
    button_longPressStart , // ����������ʼ�ж�
    button_longPress,      // ����������
    button_longPressStop,  // �������������ж�
} button_status_e;



void button_init(void);
void button_loop(void) ; 

button_status_e key5_status_return(void);
button_status_e key0_status_return(void);


/*************************** ����5�ص�����  *********************************/

void click1();
void doubleclick1() ;
void longPressStart1() ;
void longPress1() ;
void longPressStop1() ;

/*************************** ����0�ص�����  *********************************/
void click2() ;
void doubleclick2() ;
void longPressStart2();
void longPress2() ;
void longPressStop2() ;



#endif
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

#include "OneButton.h"
#include "bsp_button.h"

/*********************** �������� *************************/
OneButton key5(5, true);           // ���尴ť5��gpio5��  
OneButton key0(0,true);            // ���尴��0��gpio0��

static button_status_e button5_current_state = button_none;      // ����5��ǰ����״̬
static button_status_e button0_current_state = button_none;      // ����0��ǰ����״̬

/**********************  �������� ************************/


/**
 * @brief       ������ʼ������
 * @param[in]   none
 * @retval      none
 * @attention  ��setup��������
 */
 void button_init(void)
 {
    // ����5����ӳ��.
    key5.attachClick(click1);
    key5.attachDoubleClick(doubleclick1);
    key5.attachLongPressStart(longPressStart1);
    key5.attachLongPressStop(longPressStop1);
    key5.attachDuringLongPress(longPress1);

    // ����0����ӳ��
    key0.attachClick(click2);
    key0.attachDoubleClick(doubleclick2);
    key0.attachLongPressStart(longPressStart2);
    key0.attachLongPressStop(longPressStop2);
    key0.attachDuringLongPress(longPress2);

 }


/**
 * @brief       ����ѭ�������������������
 * @param[in]   none
 * @retval      ��setup��������
 * @attention
 */
  void button_loop(void)  
  {
    button5_current_state = button_none;  // Ĭ��ʲô��û�ɣ�ֱ�������ж��¼�
    button0_current_state = button_none;  // Ĭ��ʲô��û�ɣ�ֱ�������ж��¼�
    key5.tick();
    key0.tick();
  }


/**
 * @brief       ����5״̬�ص�����
 * @param[in]   none
 * @retval      
 * @attention   ���ڷ��ذ���5��״ֵ̬
 */
button_status_e key5_status_return(void)
{
    return button5_current_state;
}

/**
 * @brief       ����0״̬�ص�����
 * @param[in]   none
 * @retval      
 * @attention   ���ڷ��ذ���0��״ֵ̬
 */
button_status_e key0_status_return(void)
{
    return button0_current_state;
}

/*************************** ����5�ص�����  *********************************/

// ��key5������1��ʱ����������������á�
void click1()
{
  Serial.println("Button 1 click.");
  button5_current_state = button_click;    // ����״̬
} 


// ��key5��ʱ�䱻����2��ʱ����������������á�
void doubleclick1() 
{
  Serial.println("Button 1 doubleclick.");
  button5_current_state = button_doubleclick;    // ����״̬
} 


// ����ʱ�䰴��key5ʱ�����������������һ�Ρ�
void longPressStart1() 
{
  Serial.println("Button 1 longPress start");
  button5_current_state = button_longPressStart;    // ����״̬
}


// ����ʱ�䰴��key5ʱ����һֱ�������������
void longPress1() 
{
  Serial.println("Button 1 longPress...");
  button5_current_state = button_longPress;    // ����״̬
}


// ����ʱ�䰴��key5�ͷź�,���������������һ��
void longPressStop1() 
{
  Serial.println("Button 1 longPress stop");
  button5_current_state = button_longPressStop;    // ����״̬
}


/*************************** ����0�ص�����  *********************************/

void click2() 
{
  Serial.println("Button 2 click.");
  button0_current_state = button_click2;    // ����״̬
} 


void doubleclick2() 
{
  Serial.println("Button 2 doubleclick.");
  button0_current_state = button_doubleclick2;    // ����״̬
} 


void longPressStart2()
{
  Serial.println("Button 2 longPress start");
  button0_current_state = button_longPressStart2;    // ����״̬
}


void longPress2() 
{
  Serial.println("Button 2 longPress...");
  button0_current_state = button_longPress2;    // ����״̬
} 

void longPressStop2() 
{
  Serial.println("Button 2 longPress stop");
  button0_current_state = button_longPressStop2;    // ����״̬
}


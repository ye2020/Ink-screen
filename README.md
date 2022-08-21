



# 记录一次墨水屏项目的开发过程



> **version :**    *v1.0*      *「2022.7.14」*     *未添加新功能*
>
> **author：**  *Y.Z.T.*
>
> 
>
> **简介：**  *该项目是基于ESP8266开发的一款多功能墨水屏；







---

:star:<font face="华文行楷" color=black size=7> 目录</font>

[TOC]







## 一、开发过程及规划

​	

### 1.1	项目来源

*该项目是基于ESP8266开发的一款多功能墨水屏；*

*最初的项目开发来自笔者的几位同学，后因各种原因，笔者继承了该项目；因原项目已经拥有了相对完善的硬件方案，同时笔者的同学也*

*已经搭建并测试出一套行之有效的开发环境，故笔者自认只是在原有项目基础上进行优化和功能添加。*



***「当前是v1.0版本，仅在原项目基础上进行优化，还并未添加多少新的功能」***



### 1.2	改进需求

*笔者的同学已经做到了实现该墨水屏的基本功能，如天气时钟显示及SD卡读写与文件阅读等；但可能是因为赶进度，笔者在接手后发*

*现，有些地方还存在可以优化的地方。大致如下：*



- **整个UI的设计稍显单调。**
- **整个页面切换是程线性的，不够灵活。**
- **网络连接是在代码层面进行更改，过于繁琐。**



<img src="https://pic.imgdb.cn/item/6301e24a16f2c2beb15305f2.png" alt="image-20220715133600887" style="zoom: 33%;" />

​																										*==（图为原项目UI）==*

​	

### 1.3	方案设计

*针对前面提及的可优化方向，笔者开始着手进行相关的优化；*

***大致可以分成下列三个部分*:**



#### 1.3.1	UI设计

##### 1.3.1.1    优化方向

*笔者发现原UI基本上只有文字的描述，且很多地方留有大片的空白；在感官上，笔者主观地认为稍微显得有些单调。*

**故笔者认为在UI层面可以优化的方向大致有以下两点：**



- *其一： 可以考虑在文字的基础上增加图像符号的描述，可能会比单纯文字表达效果好一些。*

  

- *其二：可以尝试在整个屏幕空白的地方，填充进一些比较小的状态显示；例如：**时间显示**、**网络显示**、**电量显示**等。*





##### 1.3.1.2   UI方案

**在经过一段时间的UI布局尝试后，笔者最终决定将整个UI划分为大致三个板块：**

- 其一：最左边的图标或logo显示，醒目且清楚的表示当前页面的功能
- 其二：是最下面的备注显示，通过小字文字的显示，对每个页面的按键操作进行简单的说明。
- 其三：右边板块作为功能的选择板块，进行功能的选择和进入。



<img src="https://pic.imgdb.cn/item/6301e25716f2c2beb1530c29.png" alt="image-20220715153810006" style="zoom:50%;" />

​																					     *==（图为阅读页面UI）==*



#### 1.3.2	菜单设计



*因为原项目是通过线性的方式进行界面的切换，笔者认为不够灵活；故考虑采用**多级菜单**的方式，进行功能的切换。*



##### 1.3.2.1  设计方向

笔者认为，多级菜单本质就是多线程，多状态；



故设计方向可以分成几个：

1. *采用操作系统的多线程管理。*
2. *采用有限状态机（FSM）进行多状态的切换。*
3. 其他的方式，如遍历的方式等。



(因本项目采用的主控是ESP8266，本身硬件外设条件是比较匮乏的；而且在这种相对较小型的项目中使用==操作系统==，实在是没必要，徒增项目的复杂度，故最终还是采用**==有限状态机==**的方式来对项目进行管理)



##### 1.3.2.2    最终方案



*用状态机来设计多级菜单，也有着多种方式；比如常见的直接定义好菜单和子菜单的索引*

**例如：**

![img](https://pic.imgdb.cn/item/6301e27216f2c2beb1531ce6.jpg)



为了方便后续维护，功能添加等，笔者最终选择了==「基于事件的表驱动菜单框架的设计方法」==

*「具体的代码实现在这里不详叙，详见下面的嵌入式设计部分」*





#### 1.3.3	网络设计

原项目是通过在代码层面，定义好网络的**ssid**和**password**，并使得在开机的时候自动连接。

笔者认为，这样做适用性和泛用性太差，且修改繁琐，故弃用。



##### 1.3.3.1	方案设计

这部分的设计，笔者本想直接套用市面上主流的方案，（即扫描附近网络，选择网络，输入密码以连接）。但最终发现，在本项目只有两

个有效功能按键的情况下，进行按键密码输入实在过于麻烦和繁琐。



因为笔者同时参加过RM的比赛，其中裁判系统的**==主控模块==**也是需要连接网络来与服务器进行通信；同样功能按键不足，DJI最终选择以默认密码来连接网络，（即使用一个专用网络，将密码设置为“12345678”来连接）；



故笔者借鉴这一设计，**最终方案**为：

*（通过扫描附近网络，选择网络以进行连接，通过建立一个==密码库==储存常用密码，只要密码是其中一个即可成功连接）*





![image-20220715144919558](https://pic.imgdb.cn/item/6301e28f16f2c2beb1532b2b.png)

​																				     *==（图为DJI的裁判系统主控模块）==*





### 1.4	开发规划



*由于是第一次接触ESP8266的开发，为减少试错成本，降低开发中无所谓的时间浪费，故做出下列大致的开发流程规划。*



#### 1.4.1 	积累知识，查阅资料。



由于笔者之前并未接触过ESP8266的开发，且关于Arduino平台的开发也只局限在简单的地方，并未进行任何的相关项目开发。

因为在Arduino平台开发有一个很大的好处就是，它有很多官方的开源库可以进行调用，基本上为你做好了了==硬件层==、==驱动层==的相关代码

封装，我们只需要调用对应的==API接口==; 我们只专注于==应用层==的代码开发，能大大提高开发的速度。



故笔者首先就去查阅了相关封装库的对应函数接口说明，如SD卡的==驱动库==、==WiFi支持库==等；并做好了相关笔记，方便后续开发。



<img src="https://pic.imgdb.cn/item/6301e29416f2c2beb1532e19.png" alt="image-20220715150711055" style="zoom:50%;" />



​																				     *==（图为笔者的部分笔记内容）==*



#### 1.4.2	技术验证

​	在笔者的一位同学帮忙焊好PCB板子后，笔者进行各部分外设的调试测试至==稳定可靠且互相解耦==，这样可以确保硬件和软件解耦，避免

PCB和软件一起搞最后bug在哪都不知道。



#### 1.4.3	代码框架设计

​	在完成对硬件的测试之后，就是着手对代码框架的设计，对于一个好的嵌入式项目；一定是要有一个好的==代码框架==，这样会有利于后续

的代码维护，不然代码量一大，很容易就演变为”屎山“。



这里笔者针对新增加的功能，进行==模块化设计==和==分层设计==，封装成不同的文件。

**例如：菜单设计部分主要大致分成2个部分：**

##### menu.c/h

<img src="https://pic.imgdb.cn/item/6301e29816f2c2beb1533041.png" alt="image-20220715155127589" style="zoom: 67%;" />



*（这部分主要是为菜单框架定义了==主要的结构体==，==枚举类型==，和==相关的进程函数==等）*



##### menu_ui.c/h

<img src="https://pic.imgdb.cn/item/6301e29e16f2c2beb1533306.png" alt="image-20220715155254661" style="zoom: 67%;" />



*（这部分主要提供具体的UI绘制，针对==本项目==进行针对特点功能的实现）*



#### 1.4.4	完善代码功能



在完成对代码框架的构建后，就是进行具体==代码的填充==和==测试完善==。

![image-20220715160121682](https://pic.imgdb.cn/item/6301e2a416f2c2beb153367d.png)





#### 1.4.5   实际测试和优化

在完成全部的代码设计后，就是多做测试，测试系统的稳定性。对笔者来说，平时打比赛的时候，一般都是只有30%的时间用于代码，其余时间都是用于上车实际

测试，并不断修复发现的问题和添加需要的其他功能



## 二、嵌入式代码设计

### 2.1	主控选型



​	**本项目由于需要连接网络，且有考虑进行物联网和网页连接开发等功能。**



- *首先考虑支持WIFI功能，可选的大概有STM32系列外带WIFI模块、ESP32系列、ESP8266系列。*

  

- *其次考虑开发难度和开发周期，选择对这方面支持更好的ESP系列。*

  

- *最后选择了更便宜的ESP8266作为该项目的主控。*



*(esp8266是成本极低且具有完整[TCP/IP协议栈](https://zh.m.wikipedia.org/wiki/TCP/IP协议栈)的Wi-Fi控制芯片，能很好的适配项目的开发需求)。*





### 2.2	技术验证部分

这部分主要对应*「1.4.2」*部分内容，测试了==工程开发环境==；和所有要用到的==外设==，例如：墨水屏、按键、SD卡等。



![image-20220715162646473](https://pic.imgdb.cn/item/6301e2a916f2c2beb1533940.png)

​																						   			  *==（图为笔者的开发环境）==*



#### **开发环境**  

**主要选用：**==VScode（代码编辑）== + ==PlatformIO（用于跨平台的开发环境和统一的编译下载）== + ==git（进行代码版本管理）==



### 2.3	实际工程代码



实际工程代码这里，主要介绍笔者在项目中用到的==「基于事件的表驱动菜单框架的设计方法」==，其余部分均为针对该项目的实际应用层代码和具体UI绘制，感觉不

具备适用性，所以不多赘述。



#### 2.3.1	设计目标

在初设计代码框架的时候，要明白自己想要达成什么样的效果； 



**例如笔者的目标如下图所示：**

![img](https://pic.imgdb.cn/item/6301e2cf16f2c2beb1534a98.jpg)



​																						   			  *==（图为菜单框架即状态转换图）==*

*（如上图所示：笔者的需求是，能灵活实现各级菜单的跳转）*



#### 2.3.2   整天软件设计框架



<img src="https://pic.imgdb.cn/item/6301e2ae16f2c2beb1533bad.png" alt="软件框架设计" style="zoom: 80%;" />





#### 2.3.3	代码部分

##### menu.c

**核心代码：**

```c
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

// ......其他进程代码
```



##### menu.h

```c
/**
  ****************************** Y.Z.T.****************************************
  * @file       menu.c/h
  * @brief      提供键值适配等UI菜单框架数据
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

extern uint8_t ui_loging_flag;
// 对应数据类型定义
typedef unsigned int   menu_u32;
typedef unsigned short menu_u16;
typedef unsigned char  menu_u8;

typedef int menu_i32 ;
typedef short menu_s16 ;
typedef char menu_s8 ;


// 键值定义枚举
typedef enum 
{
    KEY_none  = 0,
	  KEY_enter,          // 确定
    KEY_esc,            // 返回
    KEY_up,             // 向上
    KEY_dowm,           // 向下
    KEY_home,           // home键
    KEY_setting,        // 设置键
    KEY_game_lift,      // 左转
    KEY_game_right,     // 右转

}key_value_e;

// 各界面索引，子菜单
typedef struct Menu_Key_Index
{
	menu_u8 main_current_index ;
	menu_u8 setting_current_index ; 
	menu_u8 select_current_index ; 
  menu_u8 language_current_index;
  menu_u8 configuration_current_index;
  menu_u8 read_current_index;
  
	menu_u8 Current_Page ;      // 当前页面
}Key_Index;


extern Key_Index sub_index ;

typedef void (*menu_op_func)(button_status_e , button_status_e);

typedef struct OP_STRUCT
{
	int op_menu ; 				//操作菜单
	menu_op_func opfun ;	//操作方法
}OP_MENU_PAGE;

/*当前菜单*/
typedef enum 
{
	MAIN_PAGE = 0,          // 主页面						
	CLOCK_PAGE,             // 时钟状态 
	WEATHER_PAGE,           // 天气状态
	CONFIGURATION_PAGE,     // 配置状态
	READ_PAGE,              // 阅读状态
	GAME_PAGE,              // 游戏状态 （以上是select下的子表单）
  SETTING_PAGE,           // 设置模式（main下的子表单）
	SELECT_PAGE,						// 菜单    （main下的子表单）
  LANGUAGE_PAGE,          // 语言设置（setting下的子表单）
  WORD_PAGE,              // 字号设置（setting下的子表单）
  BOOK_PAGE,              // 书籍页面（read下的子表单）
	
}OP_PAGE;



void Menu_Select_Item(menu_i32 current_index, button_status_e Key5Value , button_status_e Key0Value);

/******************* 进程处理 *********************/

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
void book_page_process(button_status_e Key5Value, button_status_e Key0Value);

uint8_t return_UI_loging_flag(void);
uint8_t return_flie_current_num(void);





#endif
```



##### menu_ui.c

```c
/**
  ****************************** Y.Z.T.****************************************
  * @file       menu_ui.c/h
  * @brief      提供菜单框架中UI的处理，储存UI绘制的进程函数
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

/********************************************************************/
/**************************** UI 业务代码 ****************************/
/********************************************************************/

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


}


// ....................其他业务代码
```

##### menu_ui.h

```c
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
#ifndef __MENU_UI_H
#define __MENU_UI_H

#include "menu.h"
#include <Arduino.h>

void Menu_Main_Init(void);

void Enter_Page(menu_i32 index, button_status_e Key5Value, button_status_e Key0Value);

void main_page_ui_process(menu_u8 index);
void weather_page_ui_process(void);
void clock_page_ui_process(void);
void select_page_ui_process(void);
void setting_page_ui_process(void);
void language_page_ui_process(void);
void word_page_ui_process(void);
void configuration_page_ui_process(void);
void read_page_ui_process(void);
void book_page_ui_process(void);

```



### 三、硬件PCB设计

*硬件设计部分由笔者的同学之前就完成了，笔者并未特别了解，故不多赘述。仅指出存在的问题。*



**（在此，感谢笔者的同学之前的工作）**

### 3.1原理图





![image-20220715172715788](https://pic.imgdb.cn/item/6301e2b316f2c2beb1533e22.png)



### 3.2	存在的问题



#### 3.2.1   下载电路

该项目的pcb并未设计自动下载电路，只是在IO0上放置一个开关，用于模拟时序。导致在下载的时候，会稍微麻烦和浪费时间一些。



![preview](https://pic.imgdb.cn/item/6301e2da16f2c2beb1534f8e.jpg)

​																				   			  *==（图为下载电路图）==*



#### 3.2.2	引脚共用



因为ESP8266引脚较少，导致以下两个问题：



- **屏幕刷新的DC引脚**与**按键0共用**；（屏幕刷新时，按右键容易导致死机）

  

- **SD卡的SC引脚**与**按键5**共用（调用按键后必须卸载SD卡再挂载SD卡）





## 四、总结与展望

### 4.1	开发总结



在这次项目中，笔者主要收获就是将之前在开发STM32时初构的菜单框架成功用到了项目实践中；  

接触了ESP8266的开发过程，不禁感慨，有完备的驱动库对开发来说确实是方便，不用手撸驱动，只用专注于应用层的代码，大大提高了开发效率。





总体来说，该项目是一个==轻量级的项目==, 而且笔者负责的部分实在不算多。 

 

但前后也花了将近5天的时间，实在是过于墨迹了，以至于笔者到后面开发过程中，变得有些浮躁，也因此浪费了更多时间。

 （  实在是不应该 _(:з」∠)__   ）



### 4.2  	展望



其实在【v1.0】版本开发结束后，笔者有很多功能还没有实现（比如配网进行文件上传，开发一个适合在墨水屏上的小游戏等等）。



因为接下来笔者还要进行备赛，故该项目先搁置，希望能在【v2.0】版本完善当前bug，添加更多功能。



### 4.3	致谢



在这里再次感谢笔者的同学，感谢他们之前的工作，为笔者搭建了好的开发平台，开发工作才能如此顺利完成 。

/*
  适用 8266-4MB(FS3MB OTA~512Kb)
  
  版本介绍
  V001: 初步测试SD的挂载、按键、墨水屏显示列表。
  因为8266引脚不够用，SD卡的CS引脚与按键3共用，
  故每次调用完按键就必须卸载SD卡再挂载SD卡。

  V002: 正在构建，会放出比较完整的SD卡、按键、墨水屏联动显示例程
*/

/*
  1.中文的实现由U8g2_for_Adafruit_GFX提供
  原版对8266大字库支持不好，故使用我修改的过的库
  字库为GB2312+ASCII+ASCII扩展+自添加字

  2.屏幕的驱动由GxEPD2提供
  默认8266接屏幕的引脚定义
  GPIO4-BUSY
  GPIO2-RES
  GPIO0-DC
  GPIO15-CS
  GPIO14-SCLK
  GPIO13-SDI
*/


//基类gxepd2u GFX可用于将引用或指针作为参数传递到显示实例，使用~1.2k更多代码
//启用或禁用GxEPD2_GFX基类
#include <ESP8266WiFi.h>
#define ENABLE_GxEPD2_GFX 1
#include <GxEPD2_BW.h>
//#include <GxEPD2_3C.h>
//#include <GxEPD2_7C.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <SPI.h>
#include <SD.h>
#include "main.h"
# include "sleep.h"
# include "Display_setup.h"
#include "OneButton.h"
#include "bsp_button.h"

#include <Arduino.h>
#include <string>
using namespace std;
#include <ESP8266WiFi.h>          //连网用
#include <ESP8266HTTPClient.h>    //网页用
#include <ArduinoJson.h>         //解析数据用

//声明gb2312.c
#include "gb2312.cpp"
//声明外部字体常量
extern const uint8_t chinese_gb2312[252730] U8G2_FONT_SECTION("chinese_gb2312");
File root;

#define RTCdz_SDInitError   0    // RTC数据-地址 SD挂载错误 0-无 1-错误
#define COLORED     0
#define UNCOLORED   1
SPISettings spi_settings(4000000, MSBFIRST, SPI_MODE0); //SPI设置
GxEPD2_BW<GxEPD2_213, GxEPD2_213::HEIGHT> display(GxEPD2_213(/*CS*/ 15, /*DC*/ 0, /*RST*/ 2, /*BUSY*/ 4));                         // GDEM029A01
//GxEPD2_BW<GxEPD2_290_T94, GxEPD2_290_T94::HEIGHT> display(GxEPD2_290_T94(/*CS=D8*/ 15, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4)); // GDEM029T94
//GxEPD2_BW<GxEPD2_290_T94_V2, GxEPD2_290_T94_V2::HEIGHT> display(GxEPD2_290_T94_V2(/*CS*/ 15, /*DC*/ 0, /*RST*/ 2, /*BUSY*/ 4));    // GDEM029T94, Waveshare 2.9" V2 variant
//GxEPD2_BW<GxEPD2_290_I6FD, GxEPD2_290_I6FD::HEIGHT> display(GxEPD2_290_I6FD(/*CS=15*/ SS, /*DC=4*/ 4, /*RST=2*/ 2, /*BUSY=5*/ 5)); // GDEW029I6FD
//GxEPD2_BW<GxEPD2_290_T5, GxEPD2_290_T5::HEIGHT> display(GxEPD2_290_T5(/*CS=D8*/ 15, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4));    // GDEW029T5
//GxEPD2_BW<GxEPD2_290_T5D, GxEPD2_290_T5D::HEIGHT> display(GxEPD2_290_T5D(/*CS=D8*/ 15, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4));   // GDEW029T5D
//GxEPD2_BW<GxEPD2_290_M06, GxEPD2_290_M06::HEIGHT> display(GxEPD2_290_M06(/*CS=D8*/ 15, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4)); // GDEW029M06
//GxEPD2_3C<GxEPD2_290c, GxEPD2_290c::HEIGHT> display(GxEPD2_290c(/*CS*/ 15, /*DC*/ 0, /*RST*/ 2, /*BUSY*/ 4));                      // GDEW029Z10
U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;
uint32_t RTC_SDInitError = 0;    // SD挂载错误 0-无 1-错误


void printDirectory(File dir, int numTabs);
void read_file_data();   //读取SD卡文件内容，保存与data3 字符串中
void init_ping();      //屏幕初始化
void init_setup();     //原程序的 setup
void init_loop();    //原程序的 loop
void read_filename();    //读取SD卡文件的名字，并且显示
void show_type1(uint32_t now1_i);    //逐行显示    （效果好，速度较快）
void show_type2(uint32_t now2_i);     //显示英文

void get_time_weather();     //读取时间和天气 并且显示
void get_wifi();


void clear_ping();   //刷新整个屏幕
void key0_select();    //在对应File 后面打√
void GetData();
void display_main();
void display_clock(); //时钟显示界面


extern void BWClearScreen();  //黑一下清屏幕


const String language = "zh-Hans";  // 请求语言
const String url_yiyan = "https://v1.hitokoto.cn/?encode=json&min_length=1&max_length=21";//一言获取地址
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp1.aliyun.com", 8 * 3600, 60000); //udp，服务器地址，时间偏移量，更新间隔
boolean sdInitOk = 0; //SD卡挂载状态 0-失败 1-成功
EEStru eepUserSet;
ActualWeather actual;
// #define eeprom_address1 sizeof(EEPROMStruct)    //起始地址
FutureWeather future; //创建结构体变量 未来天气
char *ssid     = "00_mi";//"8B110_2.4G";          //网络
char *password = "yezhaotin";//"DGUT8B110";
ESP8266WebServer server(80); //建立网络服务器对象，该对象用于响应HTTP请求。监听端口（80）
ESP8266HTTPUpdateServer httpUpdater; //建立httpOTA对象
IPAddress local_IP(192, 168, 3, 3);
IPAddress gateway(192, 168, 3, 3);
IPAddress subnet(255, 255, 255, 0);
char sta_ssid[32] = {0};
char sta_password[64] = {0};
IPAddress dns1(114, 114, 114, 114);
IPAddress dns2(114, 114, 115, 115);
// const char *ssid     = "8B110_5G_2";
// const char *password = "DGUT8B110";
unsigned char file_num_flag = 0;
unsigned char page_flag = 1 ;   //界面标志位   123 分别表示对应界面
unsigned char loop_flag ;   //循环显示标志位 
unsigned char wifi_flag = 1 ; //wifi连接标志位  1为成功，0为失败
unsigned char get_wifi_flag = 1;  //
unsigned char key5_flag = 0,key0_flag = 1;  //按键标志位  key0=1默认选择第一个文件夹
String select1="1";
String select2="2";
String select3="3";
char select_file=1;
uint32_t total_page=1;   //记算总页数
unsigned char SD_loop_flag ;   //界面二读SD卡循环标志位
unsigned char to_one_flag  = 0 ;
uint32_t now_read1 = 0;  //用于记录阅读位置
uint32_t now_read2 = 0;  //用于记录阅读位置
String SD_data,SD_data1,test_data;   //存File 内容
String save_data,save; //临时存放单个SD_data内容
String file_name;
String line_data; //用于保存行显示内容
char net_time[] = {'0', '0', '0', '0', '-', '0','0','-','0','0',' ','0', '0', ':', '0', '0', '\0'};




static screen_status_e screen_mode_current_state = SCREEN_MODE_main;    // 屏幕当前模式


uint32_t RTC_minute =0;       // RTC数据-分钟
uint32_t RTC_hour =0;         // RTC数据-小时
uint32_t RTC_seconds =0;      // RTC数据-秒
uint32_t RTC_cq_cuont = 0;       // RTC数据-重启计数
uint32_t RTC_clockTime2 = 0;     // RTC数据-上次的时间
uint32_t RTC_wifi_ljcs = 0;      // RTC数据-wifi连接超时计数
uint32_t RTC_jsjs =0;           // RTC数据-局刷计数
uint32_t RTC_ntpTimeError = 0;   // 获取时间超时 0-无 1-超时 2-wifi连接超时
uint32_t RTC_clock_code = 0;     // 时钟错误代码
uint32_t RTC_8025T_error = 0;    // BL8025T时钟芯片错误检测 0-正常 1-年错 2-月错 3-日错 4-时错 5-分错 6-秒错
uint32_t RTC_albumAutoOldWz = 0; // 随机轮播图片的上一次位置
uint32_t RTC_tqmskjxs =0;     // 天气模式开机显示 0-不显示 其他-显示

 uint8_t  incount = 0;

uint32_t clockTime0, clockTime1;
uint32_t clockTime3 = 0;
#define RTCdz_hour           0                      // RTC数据-地址 小时
#define RTCdz_minute         RTCdz_hour+1           // RTC数据-地址 分钟
#define RTCdz_seconds        RTCdz_minute+1         // RTC数据-地址 秒
#define RTCdz_cq_cuont       RTCdz_seconds+1        // RTC数据-地址 重启计数
#define RTCdz_clockTime2     RTCdz_cq_cuont+1       // RTC数据-地址 time2
#define RTCdz_wifi_ljcs      RTCdz_clockTime2+1     // RTC数据-地址 wifi连接超时计数
#define RTCdz_jsjs           RTCdz_wifi_ljcs+1      // RTC数据-地址 局刷计数
#define RTCdz_ntpTimeError   RTCdz_jsjs+1           // RTC数据-地址 NTP错误类型
#define RTCdz_clock_code     RTCdz_ntpTimeError+1   // RTC数据-地址 时钟错误代码
# define RTCdz_follower     RTCdz_clock_code+1
#define RTCdz_tm_error       RTCdz_follower+1       // RTC数据-地址 BL8025T时钟芯片错误检测
#define RTCdz_albumAutoOldWz RTCdz_tm_error+1       // RTC数据-地址 随机轮播图片的上一次位置
#define RTCdz_tqmskjxs       RTCdz_albumAutoOldWz+1 // RTC数据-地址 天气模式开机显示 0-不显示 其他-显示
//带温湿度的位置
uint16_t day_x0 = 54;        //未来天气详情X坐标

#define day_y0 72  //未来天气详情Y坐标
#define day_y1 day_y0+18
#define day_y2 day_y1+18
#define main_y0 14         //更新时间图标
#define main_y1 main_y0+19   //位置图标
#define main_y2 main_y1+19   //天气状态图标

#define main_x0 1         // 左边小图标X位置
#define main_x1 main_x0+15  // 左边小图标后的文字的X位置

#define main_x2 295-12    // 右边小图标X位置
uint16_t temp_x=10;
#define temp_y0 50
//圈圈位置
uint16_t circle_x;
int dht30_error = 0;            // dht30错误代码
float dht30_temp = 0.0;         // dht30温度
float dht30_humi = 0.0;         // dht30湿度
LifeIndex life_index;
WiFiClient client;
HTTPClient http;
String time_URL = "http://quan.suning.com/getSysTime.do";  //苏宁
String weather_URL = "http://api.seniverse.com/v3/weather/daily.json?key=S1bSNZi-rCMupxE0X&location=Shenzhen&language=zh-Hans&unit=c";  //心知天气
//String weather_URL = "http://api.seniverse.com/v3/weather/daily.json?key=SowoK5KqY7RljBCrs&location=Dongguan&language=zh-Hans&unit=c";
String response;    //存API读出的内容
unsigned char image[1024];
Paint paint(image, 0, 0);
Epd epd;
unsigned long time_start_ms;
unsigned long time_now_s;
void auto_eeprom()
{
  // //自动计算需要用多少eeprom数量
  // EEPROM.begin(sizeof(EEPROMStruct) + sizeof(eepClock));
  // //获取eeprom数据
  // EEPROM.get(eeprom_address0, eepUserSet);
  // EEPROM.get(eeprom_address1, eepUserClock);
  // if (eepUserSet.auto_state != 1) eepUserSet.auto_state = 0;
  // if (eepUserSet.auto_state == 0)
  {
    eepUserSet.auto_state = 1; // 自动刷写eeprom状态 0-需要 1-不需要

    // 为将要存储在EEPROM中的内容设置初始值(默认值)
    strcpy(eepUserSet.city, String("东莞").c_str());             // 城市
    strcpy(eepUserSet.weatherKey, String("SoN0MgqkuI1-gobCf").c_str());          // 天气key
    eepUserSet.nightUpdata = 0;                                 // 夜间更新 1-更新 0-不更新
    strcpy(eepUserSet.inAWord, String("By 甘草酸不酸").c_str());  // 一句话默认显示的内容
    eepUserSet.inAWord_mod = 1;                                 // 自定义一句话的模式 0-联网获取句子 1-自定义句子 2-天数倒计时 3-B站粉丝
    eepUserSet.batDisplayType = 1;                              // 电池显示类型 0-电压 1-百分比
    eepUserSet.runMode = 0;                                     // 0-模式选择页面 1-天气 2-阅读 3-时钟 4-配网
    strcpy(eepUserSet.txtNameLastTime, String("").c_str());     // 上一次打开的txt文件名
    strcpy(eepUserSet.customBmp, String("").c_str());           // 要显示的自定义图片
   // if (ClockChipCheck()) eepUserSet.clockCompensate = 0;       // 有时钟芯片的手动补偿值
    eepUserSet.clockCompensate = -850;    // 无时钟芯片的手动补偿值
    eepUserSet.outputPower = 19.0;                              // 设置发射功率
    eepUserSet.setRotation = 1;                                 // 屏幕旋转方向0-90度（1） 1-270度（3）
    eepUserSet.clock_display_state = 1;                         // 时钟模式是否开启日期显示
    eepUserSet.clock_calibration_state = 1;                     // 时钟模式是否开启强制校准
    eepUserSet.clockJZJG = 60;                                  // 时钟模式校准间隔 分钟
    eepUserSet.albumAuto = 0;                                   // 相册自动动播放 0-关闭 1-开启
    eepUserSet.fastFlip = 1;                                    // 快速翻页 0-关闭 1-开启
    eepUserSet.clockQSJG = 19;                                   // 时钟模式全局刷新间隔
    eepUserSet.sdState = 0;                                     // 启用SD卡 1-启用 0-未启用
    
    // eepUserClock.hour = 90;     // 时
    //   eepUserClock.minute = 90;   // 分
    //   eepUserClock.seconds = 90;  // 秒

    // EEPROM.put(eeprom_address0, eepUserSet);
    // EEPROM.put(eeprom_address1, eepUserClock);
    // EEPROM.commitReset(); // 首次保存覆盖掉旧的数值
    // Serial.println("EEPROM put");
  }
  if (eepUserSet.auto_state == 1)
  {
    // EEPROM.get(eeprom_address0, eepUserSet);
    // EEPROM.get(eeprom_address1, eepUserClock);
    if (eepUserSet.outputPower < 10) eepUserSet.outputPower = 19.0; //发射功率
    if (eepUserSet.clockQSJG < 5)    eepUserSet.clockQSJG = 5;      //时钟模式全局刷新间隔
    if (eepUserSet.clockJZJG < 10)   eepUserSet.clockJZJG = 10;     //时钟模式校准间隔 分钟
    Serial.println("EEPROM get");
  }
  //Serial.print(EEPROM.percentUsed());
  //Serial.println("% 当前使用的ESP闪存空间的数量");

  /*Serial.print("eepUserSet.city[]:"); Serial.println(eepUserSet.city);
    Serial.print("eepUserSet.weatherKey[]:"); Serial.println(eepUserSet.weatherKey);
    Serial.print("eepUserSet.nightUpdata:"); Serial.println(eepUserSet.nightUpdata);
    Serial.print("eepUserSet.inAWord[]:"); Serial.println(eepUserSet.inAWord);
    Serial.print("eepUserSet.screenType:"); Serial.println(eepUserSet.screenType);
    Serial.print("eepUserSet.batDisplayType:"); Serial.println(eepUserSet.batDisplayType);
    Serial.print("eepUserSet.runMode:"); Serial.println(eepUserSet.runMode);
    Serial.print("eepUserSet.txtNameLastTime:"); Serial.println(eepUserSet.txtNameLastTime);
    Serial.print("eepUserSet.clockCompensate:"); Serial.println(eepUserSet.clockCompensate);
    Serial.print("eepUserSet.setRotation:"); Serial.println(eepUserSet.setRotation);*/
  //Serial.print("eepUserSet.clock_calibration_state:"); Serial.println(eepUserSet.clock_calibration_state);
}


void setup()   //上电初始化
{
  //init_setup();
  WiFi.mode(WIFI_OFF); // 关闭wifi
  Serial.begin(115200);
  init_ping();  //屏幕初始化
  BWClearScreen();   //黑一下刷新屏幕
   get_wifi_flag = 1;  //为1 表示去连接wifi
  // wifi_flag  = 1;   //为1表示默认连接成功  连接过程中失败会置0
  //loop_flag = 0;  //循环刷新标志位 0为进入刷新 1为退出
 
  display_partialLine(1,"wifi连接中...");
  display_partialLine(3,"请等待5s");
  auto_eeprom();
  // pinMode(key5, INPUT_PULLUP);     //设置gpio5为输入，按下为0   key0 按下为0
 //pinMode(key5, INPUT_PULLUP);
  get_wifi();                         // 只能连接2.4G频段
  button_init();            // 按键初始化
}

void loop()   //主循环
{
    button_loop();        // 检测按键输入
    
  
}

// void loop()   //主循环
// {
//   /****************** 页面1 （主UI显示）********************/
//   if( page_flag == 1)   //如果连接到wifi,并且在第一界面,就读API和显示时间天气
//   {
   
//     do
//     { Serial.println("interface : 1");    // 界面1
//       get_time_weather();  //如果wifi_flag 为1，则可以刷新时间和天气
//       page_flag = 2;
//     }while(digitalRead(key5) == 1);  //按下key5，才会退出界面1，进入界面2
//    BW_refresh();          //黑白刷新一次
//    BW_refresh();
//   } 


//   /****************** 页面2 （显示时钟）********************/
//   if(page_flag==2)
//   {
    
//     do
//     {Serial.println("interface : 2 ");     
//       if(incount == 0)        //  获取数据（天气实况数据，未来天气数据）
//       {
//        GetData();
//        incount++;
//       }
//       display_clock();        // 时钟显示界面
//     }while(digitalRead(key5) == 1);
//     epd.ClearFrameMemory(0xFF);         // 用指定的颜色清除帧内存，且不更新显示（黑色）
//     epd.DisplayFrame();                 // 更新显示

//       if (epd.Init(lut_partial_update) != 0)  // 调用外围参数失败
//       {
//       Serial.print("e-Paper init failed");
//       return;
//       }
//   epd.ClearFrameMemory(0xFF);   // 用指定的颜色清除帧内存（黑色）
//   epd.DisplayFrame();           // 更新显示
//      BWClearScreen();           // 黑一下清屏
//     //  clear_ping();  //为显示界面2 ，做清屏准备
//     page_flag=3;
//   }


//   /****************** 页面3 （显示时钟）********************/

//    if( page_flag == 3 )   //第一个条件是确保没连上wifi下,能通过key5进入界面2，第二个条件确保能充界面1里进入界面2
//   { 
//     read_filename();    //读SD卡文件   里面有先对SD卡挂载是否成功检查
//     pinMode(key5, INPUT_PULLUP);
//     page_flag =4; 
//     unsigned char flag=1; 
//   do{ 
//     Serial.println("界面3"); 
//     if(select_file == 1)
//     display_partialLine_BJZ(6, select1,150, 1);
//     if(flag == 1) {
//      if(digitalRead(key5) == 0)    //用来选择打开哪个File  //默认为1
//      {
//       do{Serial.println("在select里");
//       if(digitalRead(key5) == 0)
//       {
//         key5_flag+=1;
//         if(key5_flag==1) { Serial.println(1); select_file = 1; display_partialLine_BJZ(6, select1,150, 1);    }
//         if(key5_flag==2) { Serial.println(2); select_file = 2; display_partialLine_BJZ(6, select2,150, 1);  }
//         if(key5_flag>=3) key5_flag = 0; 
//       }
//       }while(digitalRead(key0) == 1);    //在选择file下，按下key0退出
//       flag = 0;
//       Serial.println("退出select"); 
//      } }
//     page_flag = 4;
//     if(digitalRead(key0) == 0)   //返回界面1
//     { 
//       do{
//       page_flag = 1;get_wifi_flag = 1;  //1.是放它放回界面1，2.是让他重新连一次wifi
//       wifi_flag = 1;to_one_flag = 1;  // 1.是为了连接成功用。2.重界面2 返回界面1 ，就让他清一下屏幕的标志  （放置重复刷新屏幕）    
//       Serial.println("在key0的循环里");
//       break;
//       }while(digitalRead(key5) == 1); 
//     }
//     Serial.println(page_flag);
//     }while(digitalRead(key5) == 1 );   //按下则进入界面3 
//   }
//    if(page_flag == 4  )        //进入页面3
//   { char flag=0;
//     sdBeginCheck();   //SD卡挂载检查
//     if(select_file == 2) {  file_name = "word.txt"; }
//     if(select_file == 1) {  file_name = "xs.txt"; }
//      read_file_data(); //读取SD卡文件内容，保存到SD_data中
//     do{
//     Serial.println("界面4");
//     if(digitalRead(key5) == 0)
//       {
//       BWClearScreen();   //黑一下刷新屏幕
//         if( select_file == 1)
//        show_type1(now_read1);  //传当前阅读位置给它   三体
//         if(select_file == 2 )
//        show_type2(now_read2);     //单词
//       }
//     }while(digitalRead(key0) == 1);
//     delay(1000);  //防止直接退出下一个do while
//   do
//   {
//     Serial.println("选择界面1 or 界面2");
//     Serial.println("按下key5则直接返回界面3");
//     Serial.println("按下key0则返回界面1");
//     page_flag = 3; get_wifi_flag =0;
//     if(digitalRead(key0) == 0) 
//     do
//     {
//       Serial.println("返回第一界面中");
//       page_flag = 1;get_wifi_flag ==1; flag=1; break;
//     }while(1);
//     if(flag==1) { flag=0;break; }
//   } while (digitalRead(key5) == 1); //按下key5退出这个
//   BWClearScreen();
//   }
//   Serial.println("在loop里"); Serial.println(page_flag);  Serial.println();
  
//   if( get_wifi_flag ==1 )   //目的是为了重界面2返回界面1 ，重新连接一次wifi
//     { get_wifi_flag ==0 ;  //清0 放置 重复去连wifi
//       BWClearScreen();   //黑一下刷新屏幕
//       display_partialLine(0,".");display_partialLine(1,"wifi连接中...");     
//       display_partialLine(2,".");display_partialLine(3,"请等待5s");     
//       display_partialLine(4,".");display_partialLine(5,".");display_partialLine(6,".");
//       get_wifi();  
//     }
// }
//     //BWClearScreen();   //黑一下刷新屏幕  display_partialLine_BJZ(uint8_t line, String zf,uint16_t x, uint8_t width); //不居中


   


// void key0_select()
// { key0_flag ++;
//   if(key0_flag == 0 ){
//     display_partialLine_BJZ(6,"←",230, 1);
//     display_partialLine_BJZ(0," ",230, 1);
//     }
//   else {
//     if(key0_flag >=7 ) {key0_flag == 0; }
//     display_partialLine_BJZ(key0_flag-1," ",230, 1);
//     display_partialLine_BJZ(key0_flag,"←",230, 1);    
//   }
// }

// void get_time_weather()  { 
//   if( wifi_flag == 1 ) {
//   GetData();
//   display_main();
//       RTC_tqmskjxs = 0;         // 写0表示下次不需要开机显示
//     ESP.rtcUserMemoryWrite(RTCdz_tqmskjxs, &RTC_tqmskjxs, sizeof(RTC_tqmskjxs));//天气模式开机显示
//     // esp_sleep(3600000);       // 休眠1个钟
//   }  //获得天气,在里面调用处理天气的函数，处理完后显示
// }




// /**
//  * @brief       屏幕模式选择
//  * @param[in]   dat: 要写入的数据
//  * @retval      void
//  * @attention
//  */
// screen_status_e Screen_Mode_Changed(void)
// {
//    if(digitalRead(key5) == 1)   // 检测到按键5按下，进行模式变换
//    {
//       switch (screen_mode_current_state)
//       {
//       case /* constant-expression */:
//         /* code */
//         break;
      
//       default:
//         break;
//       }

//    }


// }


void GetData()
{
  
    String url_ActualWeather;    //天气实况地址
    String url_FutureWeather;    //未来天气地址
    String url_LifeIndex;        //生活指数地址
    String weatherKey_s = eepUserSet.weatherKey; //秘钥
    String city_s = eepUserSet.city;             //城市
    //"http://api.seniverse.com/v3/weather/now.json?key=S6pG_Q54kjfnBAi6i&location=深圳&language=zh-Hans&unit=c"
    //拼装天气实况API地址
    url_ActualWeather = "http://api.seniverse.com/v3/weather/now.json";
    url_ActualWeather += "?key=" + weatherKey_s + "&location=" + city_s + "&language=" + language + "&unit=c";

    //https://api.seniverse.com/v3/weather/daily.json?key=S6pG_Q54kjfnBAi6i&location=深圳&language=zh-Hans&unit=c&start=0&days=3
    //拼装实况未来API地址
    url_FutureWeather = "http://api.seniverse.com/v3/weather/daily.json";
    url_FutureWeather += "?key=" + weatherKey_s + "&location=" + city_s + "&language=" + language + "&start=0" + "&days=3";

    //https://api.seniverse.com/v3/life/suggestion.json?key=S6pG_Q54kjfnBAi6i&location=shanghai&language=zh-Hans
    //拼装生活指数API地址
    url_LifeIndex = "http://api.seniverse.com/v3/life/suggestion.json";
    url_LifeIndex += "?key=" + weatherKey_s + "&location=" + city_s;

    uint8_t cs_count = 0; //重试计数
    uint8_t cs_max = 2;   //重试次数
    // if (RTC_tqmskjxs == 0) display_Bitmap_Setup(); //开机壁纸
    // display_partialLine(7, "获取生活指数");
    // while (ParseLifeIndex(callHttp(url_LifeIndex), &life_index) == 0 && cs_count < cs_max) //获取生活指数
    // {
    //   cs_count++;
    // }

    cs_count = 0;
    //display_bitmap_bottom(Bitmap_xiaohei, "获取天气实况数据中");
    while (ParseActualWeather(callHttp(url_ActualWeather), &actual) == 0 && cs_count < cs_max)
    {
      cs_count++;
    }

    cs_count = 0;
    //display_bitmap_bottom(Bitmap_byx, "获取未来天气数据中");
    while (ParseFutureWeather(callHttp(url_FutureWeather), &future) == 0 && cs_count < cs_max)
    {
      cs_count++;
    }

  
   uint8_t update_count = 0;
  while (timeClient.update() == 0 && update_count < 6)
  {
    Serial.print("NTP超时计数:"); Serial.println(update_count);
    delay(100);
    if (update_count == 2) timeClient.setPoolServerName("s2k.time.edu.cn");
    else if (update_count == 3) timeClient.setPoolServerName("1d.time.edu.cn");
    else if (update_count == 4) timeClient.setPoolServerName("s1c.time.edu.cn");
    else if (update_count == 5) timeClient.setPoolServerName("ntp.sjtu.edu.cn");
        update_count++;
  }

  if (update_count < 6 )
  {
   
    RTC_hour = timeClient.getHours();
    RTC_minute = timeClient.getMinutes();
    RTC_seconds = timeClient.getSeconds();
    time_start_ms = millis();
    ESP.rtcUserMemoryWrite(RTC_hour, &RTC_hour, sizeof(RTC_hour));
    ESP.rtcUserMemoryWrite(RTC_minute, &RTC_minute, sizeof(RTC_minute));
   ESP.rtcUserMemoryWrite(RTC_seconds, &RTC_seconds, sizeof(RTC_seconds));
    //Serial.println(timeClient.getFormattedTime());
    Serial.print("get the time :"); Serial.println(RTC_hour);   // 获取时间
    timeClient.end();
    // display_partialLine(7, "NTP，OK!");
  }
  else
  {
    String a; String b;
    a = actual.last_update[11]; b = actual.last_update[12]; //String转char
    RTC_hour = atoi(a.c_str()) * 10; //char转int
    RTC_hour += atoi(b.c_str()) + 1;
    ESP.rtcUserMemoryWrite(RTCdz_hour, &RTC_hour, sizeof(RTC_hour));
    Serial.print("get the time :"); Serial.println(RTC_hour);
    display_partialLine(7, "Failed to obtain NTP time. Use weather time instead");     // 获取NTP时间失败,改用天气时间
  } 
}

void display_tbpd() //天气图标显示
{
  uint16_t x = circle_x + 20; //计算图标的位置
  uint16_t y = 5;
  String code;
  code = actual.weather_code;
  // if (code == "0")      display.drawInvertedBitmap(x, y + 5, Bitmap_qt, 45, 45, heise);
  // else if (code == "1") display.drawInvertedBitmap(x, y, Bitmap_qt_ws, 45, 45, heise);
  // else if (code == "2") display.drawInvertedBitmap(x, y + 5, Bitmap_qt, 45, 45, heise);
  // else if (code == "3") display.drawInvertedBitmap(x, y, Bitmap_qt_ws, 45, 45, heise);
  // else if (code == "4") display.drawInvertedBitmap(x, y, Bitmap_dy, 45, 45, heise);
  // else if (code == "5") display.drawInvertedBitmap(x, y, Bitmap_dy, 45, 45, heise);
  // else if (code == "6") display.drawInvertedBitmap(x, y, Bitmap_dy_ws, 45, 45, heise);
  // else if (code == "7") display.drawInvertedBitmap(x, y, Bitmap_dy, 45, 45, heise);
  // else if (code == "8") display.drawInvertedBitmap(x, y, Bitmap_dy_ws, 45, 45, heise);
  // else if (code == "9") display.drawInvertedBitmap(x, y + 3, Bitmap_yt, 45, 45, heise);
  // else if (code == "10") display.drawInvertedBitmap(x, y, Bitmap_zheny, 45, 45, heise);
  // else if (code == "11") display.drawInvertedBitmap(x, y, Bitmap_lzy, 45, 45, heise);
  // else if (code == "12") display.drawInvertedBitmap(x, y, Bitmap_lzybbb, 45, 45, heise);
  // else if (code == "13") display.drawInvertedBitmap(x, y, Bitmap_xy, 45, 45, heise);
  // else if (code == "14") display.drawInvertedBitmap(x, y, Bitmap_zhongy, 45, 45, heise);
  // else if (code == "15") display.drawInvertedBitmap(x, y, Bitmap_dayu, 45, 45, heise);
  // else if (code == "16") display.drawInvertedBitmap(x, y, Bitmap_by, 45, 45, heise);
  // else if (code == "17") display.drawInvertedBitmap(x, y, Bitmap_dby, 45, 45, heise);
  // else if (code == "18") display.drawInvertedBitmap(x, y, Bitmap_tdby, 45, 45, heise);
  // else if (code == "19") display.drawInvertedBitmap(x, y, Bitmap_dongy, 45, 45, heise);
  // else if (code == "20") display.drawInvertedBitmap(x, y, Bitmap_yjx, 45, 45, heise);
  // else if (code == "21") display.drawInvertedBitmap(x, y, Bitmap_zhenx, 45, 45, heise);
  // else if (code == "22") display.drawInvertedBitmap(x, y, Bitmap_xx, 45, 45, heise);
  // else if (code == "23") display.drawInvertedBitmap(x, y, Bitmap_zhongy, 45, 45, heise);
  // else if (code == "24") display.drawInvertedBitmap(x, y, Bitmap_dx, 45, 45, heise);
  // else if (code == "25") display.drawInvertedBitmap(x, y, Bitmap_bx, 45, 45, heise);
  // else if (code == "26") display.drawInvertedBitmap(x, y, Bitmap_fc, 45, 45, heise);
  // else if (code == "27") display.drawInvertedBitmap(x, y, Bitmap_ys, 45, 45, heise);
  // else if (code == "28") display.drawInvertedBitmap(x, y, Bitmap_scb, 45, 45, heise);
  // else if (code == "29") display.drawInvertedBitmap(x, y, Bitmap_scb, 45, 45, heise);
  // else if (code == "30") display.drawInvertedBitmap(x, y + 5, Bitmap_w, 45, 45, heise);
  // else if (code == "31") display.drawInvertedBitmap(x, y, Bitmap_m, 45, 45, heise);
  // else if (code == "32") display.drawInvertedBitmap(x, y, Bitmap_f, 45, 45, heise);
  // else if (code == "33") display.drawInvertedBitmap(x, y, Bitmap_f, 45, 45, heise);
  // else if (code == "34") display.drawInvertedBitmap(x, y, Bitmap_jf, 45, 45, heise);
  // else if (code == "35") display.drawInvertedBitmap(x, y, Bitmap_rdfb, 45, 45, heise);
  // //else if (code == 37) display.drawInvertedBitmap(x,y, Bitmap_dy, 45, 45, heise);
  // //else if (code == 38) display.drawInvertedBitmap(x,y, Bitmap_dy, 45, 45, heise);
  // else if (code == "99") display.drawInvertedBitmap(x, y, Bitmap_wz, 45, 45, heise);
}

void swap(int *t1, int *t2)
{
  int temp;
  temp = *t1;
  *t1 = *t2;
  *t2 = temp;
}
//****** 冒泡算法从小到大排序 ******
int get_max_num(int arr[], int len)
{
  int i, j;

  for (i = 0; i < len - 1; i++)
  {
    for (j = 0; j < len - 1 - i; j++)
    {
      if (arr[j] > arr[j + 1])
      {
        swap(&arr[j], &arr[j + 1]);
      }
    }
  }
  return arr[len - 1]; //获取最大值
}
int get_min_num(int arr[], int len)
{
  int i, j;

  for (i = 0; i < len - 1; i++)
  {
    for (j = 0; j < len - 1 - i; j++)
    {
      if (arr[j] > arr[j + 1])
      {
        swap(&arr[j], &arr[j + 1]);
      }
    }
  }
  return arr[0]; //获取最小值
}

String week_calculate(int y, int m, int d)
{
  if (m == 1 || m == 2)  //一二月换算
  {
    m += 12;
    y--;
  }
  int week = (d + 2 * m + 3 * (m + 1) / 5 + y + y / 4 - y / 100 + y / 400 + 1) % 7;
  if (week == 1) return"周一";
  else if (week == 2 ) return"周二";
  else if (week == 3 ) return"周三";
  else if (week == 4 ) return"周四";
  else if (week == 5 ) return"周五";
  else if (week == 6 ) return"周六";
  else if (week == 0 ) return"周日";
  else return "计算出错";
  //其中1~6表示周一到周六 0表示星期天
}
void display_clock() //时钟显示界面
{
  static uint32_t tep,tep1;
  if(incount==1)
  {
   tep = RTC_seconds+RTC_minute*60+RTC_hour*3600;
   tep1 = RTC_minute*60+RTC_seconds;
   incount++;
  }

    time_now_s = (millis() - time_start_ms) / 1000;

  char time_string[] = {'0','0',':','0','0', ':', '0', '0', '\0'};
  time_string[0] = (time_now_s+tep) / 3600 / 10 + '0';
  time_string[1] = (time_now_s+tep) / 3600 % 10 + '0';
  time_string[3] = (time_now_s+tep1) / 60 / 10 + '0';
  time_string[4] = (time_now_s+tep1) / 60 % 10 + '0';
  time_string[6] = (time_now_s+RTC_seconds) % 60 / 10 + '0';
  time_string[7] = (time_now_s+RTC_seconds) % 60 % 10 + '0';

  paint.SetWidth(70);   //70大字    12
  paint.SetHeight(100);   //160       65
   paint.SetRotate(ROTATE_270);
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(0,0, time_string, &myFont, COLORED);     //&Font12
  epd.SetFrameMemory(paint.GetImage(), 25 , 85, paint.GetWidth(), paint.GetHeight());
  epd.DisplayFrame();
    epd.SetFrameMemory(paint.GetImage(), 25 , 85, paint.GetWidth(), paint.GetHeight());
  epd.DisplayFrame();
  // set_flag = 1;


  // //0-正常 1-首次获取网络时间失败 2-每10分钟的wifi连接超时 3-每10分钟的获取网络时间失败
  // delay(500);
  // if (RTC_ntpTimeError == 1) {
  //   display_partialLine(6, "NTP服务器连接失败");
  //   display_bitmap_sleep(" ");
  //   esp_sleep(0);
  // }

  // //每xx次局刷全刷一次
  // if (RTC_jsjs == 0) display.init(0, 1, 10, 0);
  // else               display.init(0, 0, 10, 0);
  // RTC_jsjs++; //局刷计数
  // ESP.rtcUserMemoryWrite(RTCdz_jsjs, &RTC_jsjs, sizeof(RTC_jsjs));
  // display.setPartialWindow(0, 0, display.width(), display.height()); //设置局部刷新窗口
  // display.firstPage();
  // do
  // {
  //   //************************ 时间显示 ************************
  //   if (eepUserSet.inAWord_mod == 2 || eepUserSet.inAWord_mod == 3)
  //   {
  //     u8g2Fonts.setFont(u8g2_font_logisoso78_tn);
  //     //拼装时间 小时和分,不够两位数需要补0
  //     String hour, minute, assembleTime;
  //     if (RTC_hour < 10)   hour = "0" + String(RTC_hour);
  //     else                 hour = String(RTC_hour);
  //     if (RTC_minute < 10) minute = "0" + String(RTC_minute);
  //     else                 minute = String(RTC_minute);
  //     assembleTime = hour + ":" + minute;

  //     int8_t sz_x = 0; //显示位置X轴的偏移量
  //     if (RTC_hour >= 10 && RTC_hour <= 19) sz_x = -3; //10-19点
  //     else sz_x = 2;
  //     //显示时间
  //     u8g2Fonts.setCursor(sz_x, 95);
  //     u8g2Fonts.print(assembleTime);
  //     //画一条垂直线
  //     display.drawLine(230, 7, 230, 103, 0);
  //   }
  //   if (eepUserSet.inAWord_mod == 2 && RTC_clock_code == 200) //需要显示天数倒计
  //   {
    

  //  }

    
  //    else if (eepUserSet.inAWord_mod == 3) //b站粉丝
  //    {

  //   }
  //   else //不需要天数倒计时和B粉显示
  //   {
  //     u8g2Fonts.setFont(u8g2_font_logisoso92_tn); //时钟用的超大字体 u8g2_font_logisoso78_tn
  //     //拼装时间 小时和分,不够两位数需要补0
  //     String hour, minute, assembleTime;
  //     if (RTC_hour < 10)   hour = "0" + String(RTC_hour);
  //     else                 hour = String(RTC_hour);
  //     if (RTC_minute < 10) minute = "0" + String(RTC_minute);
  //     else                 minute = String(RTC_minute);
  //     assembleTime = hour + ":" + minute;

  //     uint8_t sz_x = 0; //显示位置X轴的偏移量
  //     if (RTC_hour >= 10 && RTC_hour <= 19) sz_x = 6; //10-19点
  //     else sz_x = 14;
  //     //显示时间
  //     u8g2Fonts.setCursor(sz_x, 100);
  //     u8g2Fonts.print(assembleTime);
  //     //Serial.print("RTC_hour:"); Serial.println(RTC_hour);
  //     //Serial.print("RTC_minute:"); Serial.println(RTC_minute);
  //     //Serial.print("RTC_seconds:"); Serial.println(RTC_seconds);
  //   }
  //   //************************ 电池电量显示 ************************ 电池电量0-电压 1-百分比


  // } while (display.nextPage());

  // //首次秒数对齐、计算休眠时间
  // int32_t sleep_num = 0;
  // clockTime1 = millis();
  // // if (RTC_8025T_error != 0) //时钟芯片不存在
  // // {
  // //   if (RTC_cq_cuont >= eepUserSet.clockJZJG)    sleep_num = eepUserSet.clockCompensate + 60000 - (RTC_seconds * 1000) - ((clockTime1 - clockTime0) + RTC_clockTime2);
  // //   else if (RTC_seconds != 0) sleep_num = 60000 - (RTC_seconds * 1000) - ((clockTime1 - clockTime0) + RTC_clockTime2);
  // //   else if (RTC_jsjs == eepUserSet.clockQSJG + 1)    sleep_num = 60000 - ((clockTime1 - clockTime0) + RTC_clockTime2);
  // //   else sleep_num = 60000 + eepUserSet.clockCompensate;
  // //   //如果计算出来的休眠值小于0
  // //   if (sleep_num <= 0)
  // //   {
  // //     clockTime3 = abs(sleep_num);
  // //     sleep_num = 100;
  // //   }
  // // }

  // /*Serial.print("sleep_num:"); Serial.println(sleep_num);
  //   Serial.print("RTC_ntpTimeError:"); Serial.println(RTC_ntpTimeError);
  //   Serial.print("RTC_cq_cuont:"); Serial.println(RTC_cq_cuont);
  //   Serial.print("RTC_jsjs:"); Serial.println(RTC_jsjs);
  //   Serial.print("RTC_clockTime2:"); Serial.println(RTC_clockTime2);*/
  // //Serial.print("sleep_num:"); Serial.println(sleep_num);
  // //Serial.print("sleep_num:"); Serial.println(sleep_num);
  // //Serial.print("sleep_num:"); Serial.println(sleep_num);
  // //  esp_sleep(sleep_num);
}
void display_main()
{
   //u8g2Fonts.setFontMode(1);
    display.setFullWindow(); //设置全屏刷新 height
    display.firstPage();


    //display.fillScreen(heise);  // 填充屏幕
    //display.display(1);         // 显示缓冲内容到屏幕，用于全屏缓冲
    //display.fillScreen(baise);  // 填充屏幕
    //display.display(1);         // 显示缓冲内容到屏幕，用于全屏缓冲

  do
  {  //****** 显示实况温度和天气图标 ******
    //提取最后更新时间的 仅提取 小时:分钟
         u8g2Fonts.setCursor(210, 16); //实时时间-小时
      u8g2Fonts.print(RTC_hour);
      u8g2Fonts.setCursor(230, 16); //实时时间-小时
      u8g2Fonts.print(RTC_minute);
    String minutes_hours;
    for (uint8_t i = 11; i < 16; i++) minutes_hours += actual.last_update[i];

    const char* minutes_hours_c = minutes_hours.c_str();                         //String转换char
    uint16_t minutes_hours_length = u8g2Fonts.getUTF8Width(minutes_hours_c);     //获取最后更新时间的长度
    uint16_t city_length = u8g2Fonts.getUTF8Width(actual.city);                  //获取城市的字符长度
    uint16_t weather_name_length = u8g2Fonts.getUTF8Width(actual.weather_name);  //获取天气现象的字符长度
    uint16_t uvi_x = 284 - (strlen(life_index.uvi) * 26 / 6 + 3);                //UVI字符的位置

    //计算这三个数值谁最大
    int num[3] = {minutes_hours_length, city_length, weather_name_length};
    int len = sizeof(num) / sizeof(*num);
    int sy_length = uvi_x + (get_max_num(num, len) + main_x1); //剩余长度

    u8g2Fonts.setFont(u8g2_font_fub42_tn); //实况温度用字体

    int temp_x_length = u8g2Fonts.getUTF8Width(actual.temp) + 20 + 58;
    temp_x = (sy_length / 2) - (temp_x_length / 2);
    u8g2Fonts.setCursor(temp_x, temp_y0);  //显示实况温度
    u8g2Fonts.print(actual.temp);

    //画圆圈
    circle_x = temp_x + u8g2Fonts.getUTF8Width(actual.temp) + 8; //计算圈圈的位置
    display.drawCircle(circle_x, temp_y0 - 34, 3, 0);
    display.drawCircle(circle_x, temp_y0 - 34, 4, 0);
    //显示天气图标
    display_tbpd();

    //****** 显示小图标和详情信息 ******
    u8g2Fonts.setFont(chinese_gb2312);
    //最后更新时间
    // display.drawInvertedBitmap(main_x0, main_y0 - 12, Bitmap_gengxing, 13, 13, heise); //画最后更新时间小图标 图片
    // u8g2Fonts.setCursor(main_x1, main_y0);
    // u8g2Fonts.print(minutes_hours);
    // display.drawInvertedBitmap(main_x0, main_y1 - 12, Bitmap_weizhi, 13, 13, heise); //画位置小图标  图片
    u8g2Fonts.setCursor(main_x1, main_y1);
    u8g2Fonts.print(actual.city);
    //天气实况状态
    // display.drawInvertedBitmap(main_x0, main_y2 - 12, Bitmap_zhuangtai, 13, 13, heise); //画天气状态小图标  图片
    u8g2Fonts.setCursor(main_x1, main_y2);
    u8g2Fonts.print(actual.weather_name);
    //紫外线指数
    u8g2Fonts.setFont(u8g2_font_u8glib_4_tf);
    u8g2Fonts.setCursor(284, main_y0 - 3);
    u8g2Fonts.print("UVI");
    u8g2Fonts.setFont(chinese_gb2312);
    u8g2Fonts.setCursor(uvi_x, main_y0);
    u8g2Fonts.print(life_index.uvi);
    //Serial.print("life_index.uvi长度："); Serial.println(strlen(life_index.uvi));
    //Serial.print("life_index.uvi："); Serial.println(life_index.uvi);
    //湿度
    // display.drawInvertedBitmap(main_x2, main_y1 - 12, Bitmap_humidity, 13, 13, heise);
    u8g2Fonts.setCursor(main_x2 - (strlen(future.date0_humidity) * 6 + 3), main_y1);
    u8g2Fonts.print(future.date0_humidity);
    //风力等级
    // display.drawInvertedBitmap(main_x2, main_y2 - 12, Bitmap_fx, 13, 13, heise);
    u8g2Fonts.setCursor(main_x2 - (strlen(future.date0_wind_scale) * 6 + 3), main_y2);
    u8g2Fonts.print(future.date0_wind_scale);

    //************ 显示未来天气 ************
    //拼装月日字符串 格式02-02
    String day0, day1, day2;
    for (uint8_t i = 5; i < 10; i++)
    {
      day0 += future.date0[i];
      day1 += future.date1[i];
      day2 += future.date2[i];
    }

    //拼装星期几
    //提取年月日并转换成int
    String nian0, nian1, nian2, yue0, yue1, yue2, ri0, ri1, ri2;
    int nian0_i, nian1_i, nian2_i, yue0_i, yue1_i, yue2_i, ri0_i, ri1_i, ri2_i;
    for (uint8_t i = 0; i <= 9; i++)
    {
      if (i <= 3)
      {
        nian0 += future.date0[i];
        nian1 += future.date1[i];
        nian2 += future.date2[i];
      }
      else if (i == 5 || i == 6)
      {
        yue0 += future.date0[i];
        yue1 += future.date1[i];
        yue2 += future.date2[i];
      }
      else if (i == 8 || i == 9)
      {
        ri0 += future.date0[i];
        ri1 += future.date1[i];
        ri2 += future.date2[i];
      }
    }
    nian0_i = atoi(nian0.c_str()); yue0_i = atoi(yue0.c_str()); ri0_i = atoi(ri0.c_str());
    nian1_i = atoi(nian1.c_str()); yue1_i = atoi(yue1.c_str()); ri1_i = atoi(ri1.c_str());
    nian2_i = atoi(nian2.c_str()); yue2_i = atoi(yue2.c_str()); ri2_i = atoi(ri2.c_str());

    //Serial.print("年:"); Serial.print(nian0_i); Serial.print(" "); Serial.print(nian1_i); Serial.print(" "); Serial.println(nian2_i);
    //Serial.print("月:"); Serial.print(yue0_i); Serial.print(" "); Serial.print(yue1_i); Serial.print(" "); Serial.println(yue2_i);
    //Serial.print("日:"); Serial.print(ri0_i); Serial.print(" "); Serial.print(ri1_i); Serial.print(" "); Serial.println(ri2_i);
    //Serial.println(week_calculate(nian, yue, ri));

    //拼装白天和晚上的天气现象
    String text_day0, text_night0, dn0_s;
    String text_day1, text_night1, dn1_s;
    String text_day2, text_night2, dn2_s;
    /*strcmp(const char s1,const char s2)
      当 str1 < str2 时，返回为负数(-1)；
      当 str1 == str2 时，返回值= 0；
      当 str1 > str2 时，返回正数(1)。*/

    if (strcmp(future.date0_text_day, future.date0_text_night) != 0) //今天
    {
      text_day0 = future.date0_text_day;
      text_night0 = future.date0_text_night;
      dn0_s = text_day0 + "转" + text_night0;
    }
    else dn0_s = future.date0_text_night;

    if (strcmp(future.date1_text_day, future.date1_text_night) != 0) //明天
    {
      text_day1 = future.date1_text_day;
      text_night1 = future.date1_text_night;
      dn1_s = text_day1 + "转" + text_night1;
    }
    else dn1_s = future.date1_text_night;

    if (strcmp(future.date2_text_day, future.date2_text_night) != 0) //后天
    {
      text_day2 = future.date2_text_day;
      text_night2 = future.date2_text_night;
      dn2_s = text_day2 + "转" + text_night2;
    }
    else dn2_s = future.date2_text_night;

    //拼装高低温
    String  high0, high1, high2, low0, low1, low2, hl0_s, hl1_s, hl2_s;
    high0 = future.date0_high; high1 = future.date1_high; high2 = future.date2_high;
    low0 = future.date0_low; low1 = future.date1_low; low2 = future.date2_low;
    hl0_s = high0 + "/" + low0;
    hl1_s = high1 + "/" + low1;
    hl2_s = high2 + "/" + low2;

    //拼装未来天气详情并显示
    String wltqxq0 = "今 " + day0 + " " + week_calculate(nian0_i, yue0_i, ri0_i) + " " + dn0_s + " " + hl0_s;
    String wltqxq1 = "明 " + day1  + " " + week_calculate(nian1_i, yue1_i, ri1_i) + " " + dn1_s + " " + hl1_s;
    String wltqxq2 = "后 " + day2  + " " + week_calculate(nian2_i, yue2_i, ri2_i)  + " " + dn2_s + " " + hl2_s;
    //计数长度
    uint16_t wltqxq0_length = u8g2Fonts.getUTF8Width(wltqxq0.c_str());
    uint16_t wltqxq1_length = u8g2Fonts.getUTF8Width(wltqxq1.c_str());
    uint16_t wltqxq2_length = u8g2Fonts.getUTF8Width(wltqxq2.c_str());
    //自动居中的X坐标
    int wltqxq0_x, wltqxq1_x, wltqxq2_x;
    wltqxq0_x = (display.width() / 2) - (wltqxq0_length / 2);
    wltqxq1_x = (display.width() / 2) - (wltqxq1_length / 2);
    wltqxq2_x = (display.width() / 2) - (wltqxq2_length / 2);
    //拼装数据 分成2段（今 06-21）&（周一 阴转阵雨）
    String data0 = "今 " + day0; //今 06-21
    String data1 = week_calculate(nian0_i, yue0_i, ri0_i) + " " + dn0_s; //周一 阴转阵雨
    String data2 = "明 " + day1; //明 06-22
    String data3 = week_calculate(nian1_i, yue1_i, ri1_i) + " " + dn1_s; //周二 阵雨转大雨
    String data4 = "后 " + day2; //后 06-23
    String data5 = week_calculate(nian2_i, yue2_i, ri2_i) + " " + dn2_s; //周三 大雨转中雨

    uint16_t data0_length = u8g2Fonts.getUTF8Width(data0.c_str());
    uint16_t data1_length = u8g2Fonts.getUTF8Width(data1.c_str());
    uint16_t data2_length = u8g2Fonts.getUTF8Width(data2.c_str());
    uint16_t data3_length = u8g2Fonts.getUTF8Width(data3.c_str());
    uint16_t data4_length = u8g2Fonts.getUTF8Width(data4.c_str());
    uint16_t data5_length = u8g2Fonts.getUTF8Width(data5.c_str());
    //对比三个数谁大
    int data_x0_num[3] = {data0_length, data2_length, data4_length};
    int data_x0_len = sizeof(data_x0_num) / sizeof(*data_x0_num);
    int data_x0_max = get_max_num(data_x0_num, data_x0_len);
    //对比三个数谁大
    int data_x1_num[3] = {data1_length, data3_length, data5_length};
    int data_x1_len = sizeof(data_x1_num) / sizeof(*data_x1_num);
    int data_x1_max = get_max_num(data_x1_num, data_x1_len);
    //对比三个数谁小
    int wltqxq_num[3] = {wltqxq0_x, wltqxq1_x, wltqxq2_x};
    int wltqxq_len = sizeof(wltqxq_num) / sizeof(*wltqxq_num);
    int wltqxq_x_min = get_min_num(wltqxq_num, wltqxq_len) + 2;
    //数据之间的间隔 5个像素
    uint8_t jianGe_x = 5;

    u8g2Fonts.setCursor(wltqxq_x_min, day_y0);
    u8g2Fonts.print(data0);
    u8g2Fonts.setCursor(wltqxq_x_min + data_x0_max + jianGe_x, day_y0);
    u8g2Fonts.print(data1);
    u8g2Fonts.setCursor(wltqxq_x_min + data_x0_max + jianGe_x + data_x1_max + jianGe_x, day_y0);
    u8g2Fonts.print(hl0_s);

    u8g2Fonts.setCursor(wltqxq_x_min, day_y1);
    u8g2Fonts.print(data2);
    u8g2Fonts.setCursor(wltqxq_x_min + data_x0_max + jianGe_x, day_y1);
    u8g2Fonts.print(data3);
    u8g2Fonts.setCursor(wltqxq_x_min + data_x0_max + jianGe_x + data_x1_max + jianGe_x, day_y1);
    u8g2Fonts.print(hl1_s);

    u8g2Fonts.setCursor(wltqxq_x_min, day_y2);
    u8g2Fonts.print(data4);
    u8g2Fonts.setCursor(wltqxq_x_min + data_x0_max + jianGe_x, day_y2);
    u8g2Fonts.print(data5);
    u8g2Fonts.setCursor(wltqxq_x_min + data_x0_max + jianGe_x + data_x1_max + jianGe_x, day_y2);
    u8g2Fonts.print(hl2_s);

    //display.drawLine(275, 0, 275, 128, 0);
    //****************************************************************************************************

    //****** 显示屏幕中间的一句话 ******
    // display.drawLine(0, 56, 295, 56, 0); //画水平线
    // display.drawLine(0, 74, 295, 74, 0); //画水平线
    // if (eepUserSet.inAWord_mod == 0)    //显示随机句子
    // {
    //   u8g2Fonts.setCursor(getCenterX(yiyan.hitokoto), 71);
    //   u8g2Fonts.print(yiyan.hitokoto);
    // }
    // else if (eepUserSet.inAWord_mod == 1)// 自定义一句话
    // {
    //   u8g2Fonts.setCursor(getCenterX(eepUserSet.inAWord), 71);
    //   u8g2Fonts.print(eepUserSet.inAWord);
    // }
    // else if (eepUserSet.inAWord_mod == 2)// 天数倒计时
    // {
    //   //提取目标月、日
    //   String myStr1 = String(eepUserSet.inAWord);
    //   String shijian_shijian = "";
    //   uint8_t shijian_yue = (myStr1[3] - 48) * 10 + (myStr1[4] - 48);
    //   uint8_t shijian_ri = (myStr1[5] - 48) * 10 + (myStr1[6] - 48);
    //   shijian_shijian = String(shijian_yue) + "月" + String(shijian_ri) + "日";
    //   //提取XX天后天前+事件
    //   String myStr = daysCalculation(eepUserSet.inAWord, nian0_i, yue0_i, ri0_i) + "（" + shijian_shijian + "）";
    //   u8g2Fonts.setCursor(getCenterX(myStr), 71);
    //   u8g2Fonts.print(myStr);
    // }
    // else if (eepUserSet.inAWord_mod == 3)// B站粉丝
    // {
    //   String myStr = "";
    //   if (RTC_Bfen_code == 0)
    //   {
    //     if (RTC_follower < 9999) myStr = "BiliBili: " + String(RTC_follower);
    //     else if (RTC_follower < 99999999)
    //     {
    //       myStr = "BiliBili: " + String(float(RTC_follower) / 10000.0, 1) + "W";
    //     }
    //     u8g2Fonts.setFont(u8g2_font_cupcakemetoyourleader_tr);
    //   }
    //   else myStr = "B粉获取错误：" + String(RTC_Bfen_code);
    //   u8g2Fonts.setCursor(getCenterX(myStr), 71);
    //   u8g2Fonts.print(myStr);
    //   u8g2Fonts.setFont(chinese_gb2312);
    // }
    //************* 显示温湿度-sht30 *************
    if (dht30_temp != 0.0 && dht30_humi != 0.0)
    {
      u8g2Fonts.setFont(u8g2_font_ncenB08_te);
      // display.drawInvertedBitmap(277, 76, Bitmap_tempSHT30, 16, 16, heise);
      // display.drawInvertedBitmap(277, 103, Bitmap_humiditySHT30, 16, 16, heise);
      //dht30_temp = -2.5;
      //dht30_humi = 5.3;

      //以下计算用于数值和图标居中
      String zf_temp = (String)fabs(dht30_temp);  // float转换成String 取绝对值
      const char* zf_t = zf_temp.c_str();         // String转换char
      uint16_t zf_t_Width = u8g2Fonts.getUTF8Width(zf_t);

      String zf_humi = (String)dht30_humi;
      const char* zf_h = zf_humi.c_str();//String转换char
      uint16_t zf_h_Width = u8g2Fonts.getUTF8Width(zf_h);

      u8g2Fonts.setCursor(285 - zf_t_Width / 2 + 3, 101);
      u8g2Fonts.print(dht30_temp, 1);
      u8g2Fonts.setCursor(285 - zf_h_Width / 2 + 3, 128);
      u8g2Fonts.print(dht30_humi, 1);
      //display.drawLine(285, 0, 285, 128, 0);
    }
    
    //电量显示 及 低压提示
    //u8g2Fonts.setFont(chinese_gb2312);
    // u8g2Fonts.setFont(u8g2_font_u8glib_4_tf); //u8g2_font_ncenB08_te
    // if (eepUserSet.batDisplayType == 0)  //显示电压数值
    // {
    //   String batVcc_s = String(bat_vcc) + "V";
    //   u8g2Fonts.setCursor(1, 113);
    //   u8g2Fonts.print(batVcc_s);
    //   //电量格数
    //   // if (bat_vcc > 3.7)display.drawInvertedBitmap(0, 115, Bitmap_bat3, 21, 12, heise);
    //   // else if (bat_vcc > 3.5)display.drawInvertedBitmap(0, 115, Bitmap_bat2, 21, 12, heise);
    //   // else display.drawInvertedBitmap(6, 115, Bitmap_bat1, 21, 12, heise);
    //   //display.drawInvertedBitmap(2, 78, Bitmap_wifidk, 26, 22, heise); //wifi断开
    //   //display.drawInvertedBitmap(7, 79, Bitmap_dlsd, 19, 31, heise);  //闪电图标
    // }
  //   else  //显示电压百分比
  //   {
  //     String batVcc_s = String(uint8_t(getBatVolBfb(bat_vcc))) + "%";
  //     u8g2Fonts.setCursor(4, 113);
  //     u8g2Fonts.print(batVcc_s);
  //     //电量格数
  //     // if (bat_vcc > 3.7)display.drawInvertedBitmap(0, 115, Bitmap_bat3, 21, 12, heise);
  //     // else if (bat_vcc > 3.5)display.drawInvertedBitmap(0, 115, Bitmap_bat2, 21, 12, heise);
  //     // else display.drawInvertedBitmap(2, 115, Bitmap_bat1, 21, 12, heise);
  //     //display.drawInvertedBitmap(0, 80, Bitmap_wifidk, 26, 22, heise); //wifi断开
  //     //display.drawInvertedBitmap(2, 79, Bitmap_dlsd, 19, 31, heise); //闪电图标
  //     //Serial.print("zf_length:"); Serial.println(zf_length);
  //   }
  //   if (bat_vcc <= BatLow) //电池电压大于阈值 显示闪电图标和电量
  //   {
  //     if (dht30_temp != 0.0 && dht30_humi != 0.0) //电池电压小于阈值并 温湿度传感器存在 则在屏幕最右侧显示低压图标
  //     {
  //      // display.drawInvertedBitmap(1, 92, Bitmap_batlow, 24, 35, heise);图片
  //     }
  //     else //电池电压小于阈值并 温湿度传感器不存在 则在屏幕最左侧显示低压图标
  //     {
  // //      display.drawInvertedBitmap(270, 92, Bitmap_batlow, 24, 35, heise);  图片
  //     }
  //     display_bitmap_sleep("* 电量过低 *");
  //   }

  }while(display.nextPage());
    //display.nextPage()
 while(digitalRead(key5)==1)
 {
   delay(1000);
 }
      
  // while (display.nextPage());
}


// void parse_data_time(String content)    //处理得到的时间数据
// {       
//       const size_t capacity = JSON_OBJECT_SIZE(2) + 60;
//       DynamicJsonBuffer JsonBuffer(capacity);
//       JsonObject& root = JsonBuffer.parseObject(content);
//       //const char* sysTime2 = root["sysTime2"]; // "2021-08-15 11:34:54"  //19个  日期时间[11]~[15]
//       const char* sysTime1 = root["sysTime1"]; // "20210815113454"   //14个  时间[8]~[11]
//       net_time[0] = sysTime1[0];net_time[1] = sysTime1[1];net_time[2] = sysTime1[2];net_time[3] = sysTime1[3];
//       net_time[5] = sysTime1[4];net_time[6] = sysTime1[5];net_time[8] = sysTime1[6];net_time[9] = sysTime1[7];
//       net_time[11] = sysTime1[8];net_time[12] = sysTime1[9];net_time[14] = sysTime1[10];net_time[15] = sysTime1[11];
//       /*  下面是显示内容  */
//       display_partialLine(1,net_time);  //在第一行显示时间
// }

void get_wifi()     //连接 wifi
{ uint8_t i = 0;  //用来判断半分钟内是否联网成功 , 否则提示
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {   //检测 wifi 连接状态
    delay(500);
    i++;
    Serial.print(".");
    if(i>=20)
    { wifi_flag = 0;   //表示wifi连接失败
      Serial.println("Failed to connect wifi within 5s ");    // 5s内未成功连接wifi （编码显示问题 用串口输出的话 utf-8 会乱码 ，改GBK的话 显示器乱码 + 注释乱码）
      Serial.println("Press key5 to enter reading mode");   // 按下key5键，进入阅读模式
      display_partialLine(1,"5s内未成功连接wifi");
      display_partialLine(3,"按下key5键,进入阅读模式");
      break;
    }
  }

  if( wifi_flag == 1 ) {   //表示wifi连接成功
  display_partialLine(1,"wifi连接成功");
  display_partialLine(3,"获取时间和天气中...");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  http.setTimeout(5000);

  }

}


void read_file_data()   //读取SD卡文件内容，保存与data3 字符串中
{ total_page = 1;
  unsigned char flag=0;
  uint32_t i,total_xs=0;  //总像素
  if(sdInitOk == 1)
  { 
    File myFile = SD.open(file_name);
    if (myFile)  //如果打开了file
    {  
        SD_data = myFile.readString();
        myFile.close();
    }
    else    //如果file没有打开
    { Serial.println("error opening test.txt"); }
  }

  if(select_file==2){     //小说
  for(i = 0; i < SD_data.length(); i++)
    {
      if (SD_data[i] & 0x80) {        //计算页数
     	  	i+=2;
        total_xs+=14;
    	} 
      else
       {  test_data += SD_data[i];
          const char *character = test_data.c_str();             //String转换char
          uint16_t zf_width = u8g2Fonts.getUTF8Width(character) ;       //获取英文的 像素大小 
          total_xs+=zf_width;
          test_data.clear();
        }         
    }
    total_page = (total_xs/1400)-1;
    total_xs =0;

}
    if(select_file==1)    //单词
    {
      for(i = 0; i < SD_data.length(); i++)
      {
          if(SD_data[i]=='/')  { flag++; }  if(flag>=6) {flag=0;total_page++;}
      }
    }
    Serial.print("总页数---------------");
    Serial.println(total_page);
}

void read_filename()    //读取SD卡文件的名字，并且显示
{
  boolean sw = 0;
while( sw ==0 )
{
sw = 1;  //用于不输出TF卡自带的系统文件
      sdBeginCheck();   //SD卡挂载检查
      display_partialLine(1, "文件列表");    //显示  “文件列表”
      uint8_t count = 2;
      root = SD.open("/");   // 打开SD卡根目录
      while (1) //输出列表
      {
        //ESP.wdtFeed();//喂狗 
        File entry = root.openNextFile();
        // Serial.print("entry:"); Serial.println(entry);
        if (! entry) break;
        String fileName = entry.name(); //文件名
        size_t fileSize = entry.size(); //文件大小
        //Serial.print(fileName); Serial.print(" "); Serial.println(fileSize);
        String xiaoxi = String(count - 2) + ". " + fileName + "  " + String(fileSize) + "字节";
        if(file_num_flag != 0)
        display_partialLine(count, xiaoxi);
        count++;file_num_flag++;
        entry.close();
      }
      // Serial.println("file_num_flag==");
      // Serial.println(file_num_flag);
      file_num_flag = 0;
}
}

void show_type2(uint32_t now2_i)
 {  int line_flag =0 ,flag1=0,flag2=0,flag3=0;    //在哪行显示的标志
    uint16_t x=5;  //用于记录X坐标
    uint16_t zf_width;  //记录字符占的像素
    uint32_t i,last_i;
    unsigned char space_flag = 0;
    for (i = now2_i; i < SD_data.length(); i++) 
      { last_i = i; 
        if (SD_data[i] & 0x80) 
        {
     	  	save_data += SD_data[i];save_data += SD_data[++i];save_data += SD_data[++i];

    	  } 
        else
        {
          if(SD_data[i] == '/')  { SD_data[i]=' ';x=300; }
          save_data += SD_data[i];  
        } 
        //SD_data[i]        
        line_data += save_data;           //将每个字符拼接起来 组成一行的内容
        if( i==last_i )  //用于判断是英文还是中文，进而确定占的X像素大小，来确定是否换行
          { const char *character = save_data.c_str();             //String转换char
            zf_width = u8g2Fonts.getUTF8Width(character) ;       //获取英文的 像素大小
            x += zf_width;
          } else {  x += 14;  }   
          save_data.clear();            //save_data用完就清空，为下次准备  
          
        if(x >= 225 ||i == (SD_data.length()-1))  //该行读满了，就显示改行，没有读满就代表file读完了
          { display_partialLine_2(line_flag , line_data);  //每保存完一行数据就显示
            line_data.clear();
            if(x >= 225)
            {
            line_flag++;
            x = 5;Serial.println(1);    }
            else { Serial.println(2);line_flag = 0;now_read2 =0; display_partialLine_BJZ(6,"(阅读完毕)",180, 4);break;}//退出，等待按键 }
          }  
        if(line_flag >= 7)      //可以通过记录i的值来确定 阅读到哪里
          { Serial.println(3);
            line_flag = 0;
            now_read2 = i+1;  //  记录当前阅读位置  重要  要+1
            break; //退出，等待按键
          }
          
      }

}

void show_type1(uint32_t now1_i)     //逐行显示    （效果好，速度较快）  1为中文  0为单词
{   uint8_t line_flag =0 ;
    uint16_t x=5;  //用于记录X坐标
    uint16_t zf_width;
    uint32_t i,last_i;
    for (i = now1_i; i < SD_data.length(); i++) 
      { last_i = i;  
        if (SD_data[i] & 0x80) {
     	  		save_data += SD_data[i];save_data += SD_data[++i];save_data += SD_data[++i];
    	  	} else {  
          save_data += SD_data[i]; }         
          line_data += save_data;           //将每个字符拼接起来 组成一行的内容
          if( i==last_i )  //用于判断是英文还是中文，进而确定占的X像素大小，来确定是否换行
          {
            const char *character = save_data.c_str();             //String转换char
            zf_width = u8g2Fonts.getUTF8Width(character) ;       //获取英文的 像素大小
            x += zf_width;
          } else {  x += 14;  }       
          save_data.clear();            //save_data用完就清空，为下次准备
          if(x >= 225 ||i == (SD_data.length()-1))  //该行读满了，就显示改行，没有读满就代表file读完了
          { display_partialLine(line_flag , line_data);  //每保存完一行数据就显示
            line_data.clear();
            if(x >= 225)
            {
            line_flag++;
            x = 5;Serial.println(1);    }
            else { Serial.println(2);line_flag = 0;now_read1 =0; display_partialLine_BJZ(6,"(阅读完毕)",180, 4);break;}//退出，等待按键 }
          }
          if(line_flag >= 7)      //可以通过记录i的值来确定 阅读到哪里
          { Serial.println(3);
            line_flag = 0;
            now_read1 = i+1;  //  记录当前阅读位置  重要  要+1
            break; //退出，等待按键
          }
      }
}

void clear_ping()   //刷新整个屏幕
{
  display_partialLine(0," ");
  display_partialLine(1," ");
  display_partialLine(2," ");
  display_partialLine(3," ");
  display_partialLine(4," ");
  display_partialLine(5," ");
  display_partialLine(6," ");
  display_partialLine(7," ");
}
/*** 初始化AP配置 ***/
void initApSTA()
{
  //WiFi.persistent(true);
  WiFi.mode(WIFI_AP_STA); //设置工作模式
  WiFi.softAPConfig(local_IP, gateway, subnet); //ap的网络参数
  WiFi.softAP(ssid, password, random(1, 14), 0, 1);  //ap的名称和密码
  WiFi.begin();
}

void initAp()
{
  //WiFi.persistent(true);
  //WiFi.mode(WIFI_OFF);   //设置工作模式
  WiFi.softAPConfig(local_IP, gateway, subnet); //ap的网络参数
  WiFi.softAP(ssid, password, random(1, 14), 0, 1); //ap的名称、密码、信道、是否隐藏、同时连接数
  WiFi.mode(WIFI_AP); //设置工作模式
}
boolean sdgzState = 1;
void display_peiwang()
{
  uint16_t y0 = 15;
  uint16_t y1 = y0 + 23;
  uint16_t y2 = y1 + 23;
  uint16_t x0 = 100;

  display.setPartialWindow(0, 0, display.width(), display.height()); //设置局部刷新窗口
  u8g2Fonts.setFont(chinese_gb2312);
  display.firstPage();
  do
  {
    display.fillScreen(heise);  // 填充屏幕
    display.display(1);         // 显示缓冲内容到屏幕，用于全屏缓冲
    display.fillScreen(baise);  // 填充屏幕
    display.display(1);         // 显示缓冲内容到屏幕，用于全屏缓冲

    //display.drawInvertedBitmap(15, 15, Bitmap_fashe, 48, 41, heise); //发射站图标 图片
    //display.drawInvertedBitmap(5, 0, Bitmap_pwewm, 64, 64, heise); //二维码图标

    u8g2Fonts.setCursor(x0, y0);
    u8g2Fonts.print("热点名称:");
    u8g2Fonts.setCursor(x0 + 65, y0);
    u8g2Fonts.print(ssid);

    u8g2Fonts.setCursor(x0, y1);
    u8g2Fonts.print("热点密码:");
    u8g2Fonts.setCursor(x0 + 65, y1);
    u8g2Fonts.print(String(password) + "(9个3)");

    u8g2Fonts.setCursor(x0, y2);
    u8g2Fonts.print("热点地址:");
    u8g2Fonts.setCursor(x0 + 65, y2);
    u8g2Fonts.print(WiFi.softAPIP());
  }
  while (display.nextPage());
}

boolean qqtq_state1 = 1;      // 请求天气状态位
uint32_t sta_count = 0;       // sta连接计数
uint32_t getvcc_time = 0;     // 获取电池电压时间
int8_t ap_state = 0;          // 启动ap模式状态 -1扫描到网络，尝试连接 0-无 1-WiFi未配置 2-配置的WiFi扫描不到 3-连接失败 4-连接成功 5-更换wifi
boolean peiwangInitStete = 0; // 配网初始化 0-未初始化 1-已初始化

void peiwang_mod()
{
  if (millis() - overtime > 300000) display_bitmap_sleep("* 操作超时 *");
  if (peiwangInitStete == 0)
  {
    peiwangInitStete = 1;
    u8g2Fonts.setFont(chinese_gb2312);
    //display_batvcc();
    if (ap_state <= 2)
    {
      //先检查wifi是否有配置
      if (WiFi.SSID().length() == 0) ap_state = 1;  // wifi名称为空开启ap模式
      else if (WiFi.SSID().length() > 0)  // wifi名称存在，尝试连接
      {
        display.firstPage();
        do {
          display.setPartialWindow(0, 0, display.width(), 56); //设置局部刷新窗口  图片
        //  display.drawInvertedBitmap(124, 14, Bitmap_fashe, 48, 41, heise); //发射站图标
        } while (display.nextPage());
        ap_state = -1;
      }
    }
    if (ap_state > 0)
    {
      if (ap_state == 4) initApSTA();
      else  initAp();

      if (webDisplayQh_state == 0)
      {
        initWebServer();     // 初始化WEB服务器、webServerOTA服务
      }

      display_peiwang();   // 显示热点信息
      // display_batvcc();    // 获取电池电压

      if (ap_state == 1) {
        sta_count = 22;
        display_partialLine(6, "WiFi未配置，已启动热点");
      }
      /*else if (ap_state == 2) {
        sta_count = 21;
        display_partialLine(6, "扫描不到已配置的WiFi，已启动热点");
        }*/
      else if (ap_state == 3) display_partialLine(6, WiFi.SSID() + " 连接失败，已启动热点");
      else if (ap_state == 4) display_partialLine(6, WiFi.SSID() + " 连接成功(" + WiFi.localIP().toString() + ")");
    }
  }

  if (ap_state == -1 || ap_state == 5) //扫描到网络，设置工作模式
  {
    //WiFi.setAutoConnect(1);   //设置自动连接wifi
    if (ap_state == -1)
    {
      WiFi.mode(WIFI_STA);
      WiFi.begin();
    }
    else if (ap_state == 5)
    {
      WiFi.persistent(true);  //需要保存
      WiFi.mode(WIFI_AP_STA); //设置工作模式
      WiFi.begin(sta_ssid, sta_password);
    }

    display_partialLine(6, "正在尝试连接：" + WiFi.SSID());

    sta_count = 0;
    while (WiFi.isConnected() == 0 && sta_count < 15) //尝试连接
    {
      ESP.wdtFeed();           // 喂狗
      server.handleClient();
      delay(900);
      sta_count++;
      if (sta_count >= 15)
      {
        peiwangInitStete = 0;
        ap_state = 3; //wifi连接失败
      }
    }

    if (WiFi.isConnected()) //连接成功，尝试获取天气参数
    {
      peiwangInitStete = 0;
      if (ap_state == -1) //仅首次连接获取天气数据，https不能获取两次以上 http可以
      {
        String weatherKey_s = eepUserSet.weatherKey;
        String city_s = eepUserSet.city;
        String url_ActualWeather;
        //拼装天气实况API地址
        url_ActualWeather = "http://api.seniverse.com/v3/weather/now.json";
        url_ActualWeather += "?key=" + weatherKey_s + "&location=" + city_s + "&language=" + language + "&unit=c";
        ParseActualWeather(callHttp(url_ActualWeather), &actual);
      }

      if (ap_state == 5) {
        webDisplayQh_state = 0;
      }
      ap_state = 4; //wifi连接成功
    }
  }
  else if (ap_state > 0)
  {
    server.handleClient();  //处理http请求
    // if (millis() - getvcc_time > 10000) //定时任务
    // {
    //   getvcc_time = millis();
    //   // display_batvcc(); //更新电池电压
    //   yield();
    // }
  }
}
void init_ping()      //屏幕初始化
{
  //display.init();
  display.init(0, 1, 10, 1); // 串口使能 初始化完全刷新使能 复位时间 ret上拉使能
  xiaobian();                  // 消除黑边
  display.setRotation(3);      // 设置旋转方向 0-0° 1-90° 2-180° 3-270°
  u8g2Fonts.begin(display);             // 将u8g2过程连接到Adafruit GFX
  u8g2Fonts.setFontMode(1);             // 使用u8g2透明模式（这是默认设置）
  u8g2Fonts.setFontDirection(0);        // 从左到右（这是默认设置）
  u8g2Fonts.setForegroundColor(heise);  // 设置前景色
  u8g2Fonts.setBackgroundColor(baise);  // 设置背景色
  u8g2Fonts.setFont(chinese_gb2312);    // 设置字体
}

// void init_setup()     //原程序的 setup
// {
//   WiFi.mode(WIFI_OFF); // 关闭wifi
//   Serial.begin(74880);
//   ESP.wdtEnable(5000);     //使能软件看门狗的触发间隔
//   //Serial.print("上次重启原因："); Serial.println(ESP.getResetReason());
//   init_ping();  //屏幕初始化
//   display_partialLine(0, "因SD卡挂载超时导致软看门狗重启");
//   if (ESP.getResetReason() == "Software Watchdog")
//   {
//     ESP.rtcUserMemoryRead(RTCdz_SDInitError, &RTC_SDInitError, sizeof(RTC_SDInitError));
//     if (RTC_SDInitError == 1)
//     {
//       Serial.println("因SD卡挂载超时导致软看门狗重启");
//       Serial.println("请检查SD卡或电路");
//       Serial.println("已进入休眠状态");
//       display_partialLine(0, "aaaa");
//       display_partialLine(1, "bbbb");
//       display_partialLine(2, "cccc");
//       esp_sleep(0);
//     }
//   }
// }

// void init_loop()    //原程序的 loop
// {
//   pinMode(key5, INPUT_PULLUP);
//   Serial.println("等待按下按键3");
//   display_partialLine(0, "");
//   display_partialLine(7, "电池电压" + String(getBatVolNew()) + "V");
//   boolean sw = 0;
//   while (sw == 0)
//   {
//     ESP.wdtFeed();//喂狗
//     if (digitalRead(key5) == 0)
//     {
//       sw = 1;
//       sdBeginCheck();
//       if (sdInitOk)
//       {
//         root = SD.open("/");
//         printDirectory(root, 0); //打印目录
//         Serial.println("文件列表输出完毕");
//         Serial.println(" ");
//         root = SD.open("/");
//         File myFile = SD.open("test.txt",FILE_WRITE); // 打开test.txt文件
//         if (myFile)  
//         {
//           Serial.print("Writing to test.txt...");
//           myFile.println("aaa");   //往SD写
//           Serial.println("test.txt:");
//           myFile.close();
//         }
//         else Serial.println("打开失败 测试.txt");
//         Serial.println(" "); Serial.println(" ");
//       }
//       File myFile = SD.open("test.txt");
//       if (myFile) {
//         Serial.println("test.txt:");
//         // read from the file until there's nothing else in it:
//         while (myFile.available()) {
//           Serial.write(myFile.read());
//         }
//         // close the file:
//         myFile.close();
//       } else {
//         // if the file didn't open, print an error:
//         Serial.println("error opening test.txt");
//       }
//     }
//   }
//   //BWClearScreen();//黑白清屏
//   display_partialLine(1, "按下按键3输出文件列表");
//   pinMode(key5, INPUT_PULLUP);
//   sw = 0;
//   while (sw == 0)
//   {
//     ESP.wdtFeed();//喂狗
//    if (digitalRead(key5) == 0)
//     {
//       sw = 1;
//       sdBeginCheck();
//       display_partialLine(1, "文件列表");
//       uint8_t count = 2;
//       root = SD.open("/");
//       while (1) //输出列表
//       {
//         ESP.wdtFeed();//喂狗
//         File entry = root.openNextFile();
//         // Serial.print("entry:"); Serial.println(entry);
//         if (! entry) break;
//         String fileName = entry.name(); //文件名
//         size_t fileSize = entry.size(); //文件大小
//         Serial.print(fileName); Serial.print(" "); Serial.println(fileSize);
//         String xiaoxi = String(count - 1) + ". " + fileName + "  " + String(fileSize) + "字节";
//         display_partialLine(count, xiaoxi);
//         count++;
//         entry.close();
//       }
//     }
//   }
//   display_partialLine(7, "2电池电压:" + String(getBatVolNew()) + "V");
//   esp_sleep(0);
// }

// void printDirectory(File dir, int numTabs)    //打印目录
// {
//   while (true)
//   {
//     File entry =  dir.openNextFile();
//     if (! entry) {
//       // no more files
//       break;
//     }
//     for (uint8_t i = 0; i < numTabs; i++) {
//       Serial.print('\t');
//     }
//     Serial.print(entry.name());
//     if (entry.isDirectory())
//     {
//       Serial.println("/");
//       printDirectory(entry, numTabs + 1);
//     }
//     else
//     {
//       // files have sizes, directories do not
//       Serial.print("\t\t");
//       Serial.print(entry.size(), DEC);
//       time_t cr = entry.getCreationTime();
//       time_t lw = entry.getLastWrite();
//       struct tm * tmstruct = localtime(&cr);
//       Serial.printf("\tCREATION: %d-%02d-%02d %02d:%02d:%02d", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
//       tmstruct = localtime(&lw);
//       Serial.printf("\tLAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
//     }
//     entry.close();
//   }
// }

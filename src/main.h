# ifndef _MAIN_H
# define _MAIN_H

#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266HTTPClient.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <SPI.h>
#include <SD.h>
#include <ESP8266WiFi.h>
# include <stdint.h>
#include <LittleFS.h> // 官方要求的新文件系统库  #include "FS.h"未来将不会得官方支持，已弃用

# include "sleep.h"
# include "callhttps.h"
# include "jsonweather.h"
#include "NTPClient.h"
#include "epd2in13.h"
#include "epdpaint.h"
# include "fonts.h"
# include  "webserver.h"

#define ENABLE_GxEPD2_GFX 1
#define baise  GxEPD_WHITE  //白色
#define heise  GxEPD_BLACK  //黑色
#define key0 0   // GPIO0  FLASH 键
#define key5 5   // GPIO5   
#define key13 13   // GPIO13    MOSI键
#define SD_CS 5  // SD卡选择引脚，可改
#define SCK 14   // 硬件SPI不可改
#define MOSI 13  // 硬件SPI不可改
#define MISO 12  // 硬件SPI不可改
#define BAT_SWITCH 12 //控制电池电压测量的开关，可改

#define SPI_SPEED SD_SCK_MHZ(20)    // SD卡频率
#define RTCdz_SDInitError   0    // RTC数据-地址 SD挂载错误 0-无 1-错误


#define BatLow          3.3         // 低压提示和永不唤醒电压
#define noUpdateTime    5           // 夜间不更新时间截止时间
#define FileLimit       32          // 文件系统文件数量上限
#define jiaoZhunTime    20          // 校准间隔，已停用

#include <GxEPD2_BW.h>


extern const String language;  // 请求语言
extern char *ssid     ;          //网络
extern char *password ;

extern uint32_t RTC_SDInitError;    // SD挂载错误 0-无 1-错误


extern U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;
extern SPISettings spi_settings; //SPI设置
extern GxEPD2_BW<GxEPD2_213, GxEPD2_213::HEIGHT> display;   
extern boolean sdInitOk ; //SD卡挂载状态 0-失败 1-成功

extern uint8_t webDisplayQh_state;

extern size_t overtime ;
extern File fsUploadFile;           // 建立文件对象用于闪存文件上传
extern ESP8266WebServer server; //建立网络服务器对象，该对象用于响应HTTP请求。监听端口（80）
extern ESP8266HTTPUpdateServer httpUpdater; //建立httpOTA对象


struct ActualWeather
{
  char status_code[64];  // 错误代码
  char city[16];         // 城市名称
  char weather_name[16]; // 天气现象名称
  char weather_code[4];  // 天气现象代码
  char temp[5];          // 温度
  char last_update[25];  // 最后更新时间
};
extern ActualWeather actual;   // 创建结构体变量 目前的天气

struct EEStru
{
  uint8_t auto_state;        // 自动刷写eeprom状态 0-需要 1-不需要
  char  city[30];            // 城市
  char  weatherKey[24];      // 天气KEY
  boolean nightUpdata;       // 夜间更新 1-更新 0-不更新
  char  inAWord[67];         // 自定义一句话
  uint8_t inAWord_mod;       // 自定义一句话的模式 0-联网获取句子 1-自定义句子 2-天数倒计时 3-B站粉丝
  boolean batDisplayType;    // 电量显示类型 0-电压 1-百分比
  uint8_t runMode;           // 0-模式选择页面 1-天气 2-阅读 3-时钟 4-配网
  char txtNameLastTime[32];  // 上一次打开的txt文件名
  char customBmp[32];        // 要显示的自定义图片
  int16_t clockCompensate;   // 时钟手动补偿值
  float outputPower;         // 设置发射功率
  boolean setRotation;       // 屏幕旋转方向0-90度（1） 1-270度（3）
  boolean clock_display_state;     // 时钟模式是否开启日期显示
  boolean clock_calibration_state; // 时钟模式是否开启强制校准
  uint8_t clockJZJG;               // 时钟模式校准间隔 分钟
  boolean albumAuto;               // 相册自动动播放 0-关闭 1-开启
  boolean fastFlip;                // 快速翻页 0-关闭 1-开启
  uint8_t clockQSJG;               // 时钟模式全局刷新间隔
  boolean sdState;                 // 启用SD卡 1-启用 0-未启用
  boolean type_8025T;              // 8025t 的类型 0-BL(按顺序读取) 1-RX(读取的数据会偏移8位)
};
extern EEStru eepUserSet;

struct FutureWeather
{
  char status_code[64];       // 错误代码

  char date0[14];             // 今天日期
  char date0_text_day[20];    // 白天天气现象名称
  char date0_code_day[4];     // 白天天气现象代码
  char date0_text_night[16];  // 晚上天气现象名称
  char date0_code_night[4];   // 晚上天气现象代码
  char date0_high[5];         // 最高温度
  char date0_low[5];          // 最低温度
  char date0_humidity[5];     // 相对湿度
  char date0_wind_scale[5];   // 风力等级

  char date1[14];             // 明天日期
  char date1_text_day[20];    // 白天天气现象名称
  char date1_code_day[4];     // 白天天气现象代码
  char date1_text_night[16];  // 晚上天气现象名称
  char date1_code_night[4];   // 晚上天气现象代码
  char date1_high[5];         // 最高温度
  char date1_low[5];          // 最低温度

  char date2[14];             // 后天日期
  char date2_text_day[20];    // 白天天气现象名称
  char date2_code_day[4];     // 白天天气现象代码
  char date2_text_night[16];  // 晚上天气现象名称
  char date2_code_night[4];   // 晚上天气现象代码
  char date2_high[5];         // 最高温度
  char date2_low[5];          // 最低温度
};
extern FutureWeather future; //创建结构体变量 未来天气
struct LifeIndex // 生活指数
{
  char status_code[64];  // 错误代码
  char uvi[10];          // 紫外线指数
};
extern LifeIndex life_index; // 创建结构体变量 生活指数
extern boolean qqtq_state1 ;      // 请求天气状态位
extern uint32_t sta_count ;       // sta连接计数
extern uint32_t getvcc_time ;     // 获取电池电压时间
extern int8_t ap_state ;          // 启动ap模式状态 -1扫描到网络，尝试连接 0-无 1-WiFi未配置 2-配置的WiFi扫描不到 3-连接失败 4-连接成功 5-更换wifi
extern boolean peiwangInitStete ; // 配网初始化 0-未初始化 1-已初始化



 void replyOK();
 void handleFileDelete();
 void replyServerError(String msg);
 void handleStatus();
 bool handleFileRead(String path);
 void handleFileList();
 void handleGetEdit();
 void handleFileCreate();
 void handleFileUpload();
 String byteConversion(size_t zijie); //字节换算

# endif
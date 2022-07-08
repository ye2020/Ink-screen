//系统休眠
/*#define SD_CS 5
  #define SCK 14
  #define MOSI 13
  #define MISO 12*/
# include "sleep.h"
# include "main.h"
# include <stdint.h>
void esp_sleep(uint32_t minutes)
{
  //digitalWrite(BAT_SWITCH, 0); //关闭电池测量
  //pinMode(BAT_SWITCH, INPUT_PULLUP);  //改为输入状态避免漏电

  /*digitalWrite(BAT_SWITCH, 0);
  pinMode(BAT_SWITCH, INPUT_PULLUP);

  digitalWrite(MOSI, 0);
  pinMode(MOSI, INPUT_PULLUP);

  digitalWrite(SCK, 0);
  pinMode(SCK, INPUT_PULLUP);
  
  digitalWrite(SD_CS, 0);
  pinMode(SD_CS, INPUT_PULLUP);*/
   
  display.hibernate(); //关闭电源屏幕并进入深度睡眠 display.powerOff()为仅关闭电源
  //查看堆碎片
  Serial.print("堆碎片度量："); Serial.println(ESP.getHeapFragmentation());
  Serial.print("可用堆大小："); Serial.println(ESP.getFreeHeap());
  ESP.deepSleep(minutes * 1000, WAKE_RF_DEFAULT); //WAKE_RF_DEFAULT  WAKE_RFCAL  WAKE_NO_RFCAL  WAKE_RF_DISABLED RF_DISABLED
}


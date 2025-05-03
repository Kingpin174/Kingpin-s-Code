#include <Arduino.h>
#include <Ticker.h>
#include <ESP8266WiFiMulti.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "WiFi.h"
#include "OLED.h"

// 函数声明
void OLED_Proc(void);
void WiFi_Proc(void);
void Carton_Proc(void);
void Timer1_Server(void);

// 全局变量
Ticker Timer1;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
unsigned char Flag = 0;
unsigned char Number_Picture = 0;
unsigned char OLED_Slow = 1;
unsigned int WiFi_Slow = 0;
unsigned char Carton_Slow = 0;

void setup() {
  // 初始化代码
  Timer1.attach_ms(1, Timer1_Server);

  OLED_Init();
  OLED_Clear();

  WiFi_Init();
}

void loop() {
  // 主循环代码
  OLED_Proc();
  WiFi_Proc();
  Carton_Proc();
}

void OLED_Proc(void) {
  if (OLED_Slow) return;
  OLED_Slow = 1;

  if (results_0_last_update_str.isEmpty())
    OLED_ShowString(1, 1, "Waiting...");
  else {
    if (Flag == 0) {
      Flag = 1;
      OLED_Clear();
    }

    // 显示天气
    if (results_0_now_code_int == 0 || results_0_now_code_int == 1 || results_0_now_code_int == 2 || results_0_now_code_int == 3) {
      OLED_ShowAPicTure(0, 4, 31, 7, 0);
      OLED_ShowAHanzi(4, 3, 7);
      OLED_ShowAHanzi(4, 5, 8); // 晴天
    } else if (results_0_now_code_int == 4 || results_0_now_code_int == 5 || results_0_now_code_int == 6 || results_0_now_code_int == 7 || results_0_now_code_int == 8) {
      OLED_ShowAPicTure(0, 4, 31, 7, 1);
      OLED_ShowAHanzi(4, 3, 9);
      OLED_ShowAHanzi(4, 5, 10); // 多云
    } else if (results_0_now_code_int == 9) {
      OLED_ShowAPicTure(0, 4, 31, 7, 2);
      OLED_ShowAHanzi(4, 3, 11);
      OLED_ShowAHanzi(4, 5, 8); // 阴天
    } else if (results_0_now_code_int == 10) {
      OLED_ShowAPicTure(0, 4, 31, 7, 3);
      OLED_ShowAHanzi(4, 3, 12);
      OLED_ShowAHanzi(4, 5, 13); // 阵雨
    } else if (results_0_now_code_int == 11) {
      OLED_ShowAPicTure(0, 4, 31, 7, 4);
      OLED_ShowAHanzi(4, 3, 14);
      OLED_ShowAHanzi(4, 4, 12);
      OLED_ShowAHanzi(4, 5, 13); // 雷阵雨
    } else if (results_0_now_code_int == 12) {
      OLED_ShowAPicTure(0, 4, 31, 7, 5);
      OLED_ShowAHanzi(4, 3, 15);
      OLED_ShowAHanzi(4, 5, 16); // 雷阵雨伴有冰雹
    } else if (results_0_now_code_int == 13) {
      OLED_ShowAPicTure(0, 4, 31, 7, 6);
      OLED_ShowAHanzi(4, 3, 17);
      OLED_ShowAHanzi(4, 5, 13); // 小雨
    } else if (results_0_now_code_int == 14) {
      OLED_ShowAPicTure(0, 4, 31, 7, 7);
      OLED_ShowAHanzi(4, 3, 18);
      OLED_ShowAHanzi(4, 5, 13); // 中雨
    } else if (results_0_now_code_int == 15) {
      OLED_ShowAPicTure(0, 4, 31, 7, 8);
      OLED_ShowAHanzi(4, 3, 19);
      OLED_ShowAHanzi(4, 5, 13); // 大雨
    } else if (results_0_now_code_int == 16) {
      OLED_ShowAPicTure(0, 4, 31, 7, 9);
      OLED_ShowAHanzi(4, 3, 20);
      OLED_ShowAHanzi(4, 5, 13); // 暴雨
    }

    // 显示温度
    OLED_ShowString(3, 5, "T:");
    OLED_ShowNum(3, 7, results_0_now_temperature_int, 2);
    OLED_ShowAHanzi(3, 5, 6);

    // 显示实时时间
    timeClient.update();
    timeClient.setTimeOffset(28800);
    OLED_ShowTime(1, 1, timeClient.getFormattedTime().c_str());
  }
}

void WiFi_Proc(void) {
  if (WiFi_Slow) return;
  WiFi_Slow = 1;

  Client_Request();
}

void Carton_Proc(void) {
  if (Carton_Slow) return;
  Carton_Slow = 1;

  // 显示旋转太空人
  OLED_ShowCarton(87, 4, 127, 7, Number_Picture);
  Number_Picture++;
  Number_Picture %= 13;
}

void Timer1_Server(void) {
  if (OLED_Slow != 0)
    OLED_Slow++;
  if (OLED_Slow == 100)
    OLED_Slow = 0;

  if (Carton_Slow != 0)
    Carton_Slow++;
  if (Carton_Slow == 100)
    Carton_Slow = 0;

  if (WiFi_Slow != 0)
    WiFi_Slow++;
  if (WiFi_Slow == 10000)
    WiFi_Slow = 0;
}
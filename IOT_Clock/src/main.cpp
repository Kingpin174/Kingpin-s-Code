#include <Arduino.h>
#include <Ticker.h>
#include <ESP8266WiFiMulti.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "WiFi.h"
#include "OLED.h"

void OLED_Proc(void);
void WiFi_Proc(void);
void Carton_Proc(void);
void Timer1_Server(void);

Ticker Timer1;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
unsigned char Flag = 0;
unsigned char Number_Picture = 0;
unsigned char OLED_Slow = 1;
unsigned int WiFi_Slow = 0;
unsigned char Carton_Slow = 0;

//请在心知天气注册账号，并把"密钥"和"城市"补充完整
const char* host = "api.seniverse.com"; // 删除 extern
const char* Privat_Key = "XXXXXXXXX";   // 删除 extern
const char* City = "XXX";               // 修改为 const char*，删除 extern

String results_0_now_text_str = ""; 
int results_0_now_code_int = 0; 
int results_0_now_temperature_int = 0; 
String results_0_last_update_str = "";   


void setup() {
  // put your setup code here, to run once:
  Timer1.attach_ms(1, Timer1_Server);

  OLED_Init();
  OLED_Clear();

  WiFi_Init();

  timeClient.setTimeOffset(28800); // 设置时区偏移，只需设置一次
}

void loop() {
  // put your main code here, to run repeatedly:
  OLED_Proc();
  WiFi_Proc();
  Carton_Proc();
}

void OLED_Proc(void)
{
  if(OLED_Slow) return;
  OLED_Slow = 1;
  
  if(results_0_last_update_str.isEmpty())
    OLED_ShowString(1, 1, "Waiting...");
  else 
  {
    if(Flag == 0)
    {
      Flag = 1;
      OLED_Clear();
    }
    
    // 显示天气
    switch(results_0_now_code_int) {
      case 0: case 1: case 2: case 3:
        OLED_ShowAPicTure(0, 4, 31, 7, 0);
        OLED_ShowAHanzi(4, 3, 7); OLED_ShowAHanzi(4, 5, 8); break; // 晴天
      case 4: case 5: case 6: case 7: case 8:
        OLED_ShowAPicTure(0, 4, 31, 7, 1);
        OLED_ShowAHanzi(4, 3, 9); OLED_ShowAHanzi(4, 5, 10); break; // 多云
      case 9:
        OLED_ShowAPicTure(0, 4, 31, 7, 2);
        OLED_ShowAHanzi(4, 3, 11); OLED_ShowAHanzi(4, 5, 8); break; // 阴天
      default:
        OLED_ShowString(4, 1, "Unknown Weather");
    }

    // 显示温度
    OLED_ShowString(3, 5, "T:");
    OLED_ShowNum(3, 7, results_0_now_temperature_int, 2); OLED_ShowAHanzi(3, 5, 6);
      
    // 显示实时时间
    timeClient.update();
    OLED_ShowTime(1, 1, timeClient.getFormattedTime().c_str());
  }
  
}

void WiFi_Proc(void)
{
    if (WiFi_Slow) return;
    WiFi_Slow = 1;

    if (WiFi.status() != WL_CONNECTED) {
        WiFi_Init(); // 如果断开连接，重新初始化 WiFi
    }

    Client_Request();
}

void Carton_Proc(void)
{
  if(Carton_Slow) return;
  Carton_Slow = 1;
  
  //显示旋转太空人
  OLED_ShowCarton(87, 4, 127, 7 ,Number_Picture);
  Number_Picture++;
  Number_Picture %= 13;
}

void Timer1_Server(void)
{
  if(OLED_Slow != 0)
    OLED_Slow ++;
  if(OLED_Slow == 100)
    OLED_Slow = 0;
    
  if(Carton_Slow != 0)
    Carton_Slow ++;
  if(Carton_Slow == 100)
    Carton_Slow = 0;

  if(WiFi_Slow != 0)
    WiFi_Slow++;
  if(WiFi_Slow == 10000)
    WiFi_Slow = 0;
}

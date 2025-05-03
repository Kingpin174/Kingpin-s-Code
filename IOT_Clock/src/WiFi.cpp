#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <WiFiManager.h>        
#include "OLED.h"
#include "WiFi.h"
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h> // 确保包含 WiFiClient 头文件

void Client_Request(void);
void Json_Parse(WiFiClient Client);

ESP8266WebServer WebServer;

const char* host = "api.seniverse.com";
const char* Privat_Key = "XXXXXXXXX"; // 输入你的密钥
const char* City = "XXX";            // 目标城市拼音

String results_0_now_text_str = "";
int results_0_now_code_int = 0;
int results_0_now_temperature_int = 0;
String results_0_last_update_str = "";

void WiFi_Init(void)
{
//串口初始化
  Serial.begin(9600);
  Serial.println("");

//串口和OLED显示连接WiFi
  Serial.println("连接WiFi中");
  OLED_ShowString(1, 1, "Connecting...");

//连接WiFi
  WiFiManager wifiManager;
  // wifiManager.resetSettings(); //清除esp8266所存储的wifi信息//如需要，可取消注释
  wifiManager.autoConnect("ESP8266", "123456");

//串口输出WiFi信息
  Serial.println("");
  Serial.println("WiFi已连接");
  Serial.print("WiFi SSID: "); Serial.println(WiFi.SSID());
  Serial.print("IP Address: "); Serial.println(WiFi.localIP());
//OLED显示WiFi信息
  OLED_Clear();
  OLED_ShowString(1, 1, "Success");
  OLED_ShowString(3, 1, "SSID:");
  OLED_ShowString(3, 6, WiFi.SSID().c_str());
  OLED_ShowString(4, 1, "IP:");
  OLED_ShowString(4, 4, WiFi.localIP().toString().c_str());
  delay(3000);
  OLED_Clear();

  Serial.println("开始访问服务器");
}

void Client_Request(void)
{
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client; // 创建 WiFiClient 实例
    String url = String("http://") + host + "/v3/weather/now.json?key=" + Privat_Key + "&location=" + City + "&language=zh-Hans&unit=c";
    http.begin(client, url); // 使用新的 API
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        // 解析 JSON 数据并更新全局变量
    }
    http.end();
  }
}

void Json_Parse(WiFiClient Client)
{
  StaticJsonDocument<768> doc;

  DeserializationError error = deserializeJson(doc, Client);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    OLED_Clear();
    OLED_ShowString(1, 1, "JSON Parse Err");
    return;
  }

  JsonObject results_0 = doc["results"][0];

  JsonObject results_0_now = results_0["now"];
  results_0_now_text_str = results_0_now["text"].as<String>(); 
  results_0_now_code_int = results_0_now["code"].as<int>(); 
  results_0_now_temperature_int = results_0_now["temperature"].as<int>(); 
  results_0_last_update_str = results_0["last_update"].as<String>();   

  Serial.println(F("======Weather Now======="));
  Serial.print(F("Weather Now: "));
  Serial.print(results_0_now_text_str);
  Serial.print(F(" "));
  Serial.println(results_0_now_code_int);
  Serial.print(F("Temperature: "));
  Serial.println(results_0_now_temperature_int);
  Serial.print(F("Last Update: "));
  Serial.println(results_0_last_update_str);
}

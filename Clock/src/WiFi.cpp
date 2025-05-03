#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <WiFiManager.h>        
#include "OLED.h"

void Client_Request(void);
void Json_Parse(WiFiClient Client);

ESP8266WebServer WebServer;

//请在心知天气注册账号，并把"密钥"和"城市"补充完整
extern const char* host = "api.seniverse.com";
extern const char* Privat_Key = "SPYZgwXgJxZhYZHZG";     //输入你的密钥
extern char* City = "Nanjing";                       //目标城市的拼英，例：Beijing

extern String results_0_now_text_str = ""; 
extern int results_0_now_code_int = 0; 
extern int results_0_now_temperature_int = 0; 
extern String results_0_last_update_str = "";   

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
  WiFiClient Client;

  String URL = String("/v3/weather/now.json?key=") + Privat_Key + "&location=" + City + "&language=en&unit=c";

  String HttpRequest = String("GET ") + URL + " HTTP/1.1\r\n" +
                        "Host: " + host + "\r\n" +
                        "Connection: close \r\n\r\n";

  if(Client.connect(host, 80))
  {
    Serial.println("服务器连接成功");

    Client.print(HttpRequest);
    String Response_Status = Client.readStringUntil('\n');
    Serial.print("响应状态行: "); Serial.println(Response_Status); Serial.println("");

    if(Response_Status.charAt(9) != '2')
    {
      OLED_Clear();
      OLED_ShowString(1, 1, "Access failed");
      Client.stop();
      while(1)
      {
        delay(1000);
      }
    }

    if(Client.find("\r\n\r\n"))
    {
      Serial.println("已找到响应体");
    }

    Json_Parse(Client);
    Client.stop();
  }
  else
  {
    Serial.println("连接失败");
    OLED_Clear();
    OLED_ShowString(1, 1, "Client.connect"); 
    OLED_ShowString(2, 1, "          failed"); 
    Client.stop();
    while(1)
    {
      delay(1000);
    }
  }
}

void Json_Parse(WiFiClient Client)
{
  StaticJsonDocument<768> doc;

  DeserializationError error = deserializeJson(doc, Client);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  JsonObject results_0 = doc["results"][0];

  JsonObject results_0_now = results_0["now"];
  const char* results_0_now_text = results_0_now["text"]; // "多云"
  const char* results_0_now_code = results_0_now["code"]; // "4"
  const char* results_0_now_temperature = results_0_now["temperature"]; // "14"

  const char* results_0_last_update = results_0["last_update"]; // "2015-09-25T22:45:00-07:00"

 
  // 通过串口监视器显示以上信息
  results_0_now_text_str = results_0_now["text"].as<String>(); 
  results_0_now_code_int = results_0_now["code"].as<int>(); 
  results_0_now_temperature_int = results_0_now["temperature"].as<int>(); 
  
  results_0_last_update_str = results_0["last_update"].as<String>();   
 
  Serial.println(F("======Weahter Now======="));
  Serial.print(F("Weather Now: "));
  Serial.print(results_0_now_text_str);
  Serial.print(F(" "));
  Serial.println(results_0_now_code_int);
  Serial.print(F("Temperature: "));
  Serial.println(results_0_now_temperature_int);
  Serial.print(F("Last Update: "));
  Serial.println(results_0_last_update_str);
}

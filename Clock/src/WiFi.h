#include <Arduino.h> // Include the header that defines String

#ifndef __WIFI_H__
#define __WIFI_H__

extern const char* host;
extern const char* Privat_Key;
extern char* City;
extern String results_0_now_text_str; 
extern int results_0_now_code_int; 
extern int results_0_now_temperature_int; 
extern String results_0_last_update_str;

void WiFi_Init(void);
void Client_Request(void);
#endif
#ifndef WIFI_H
#define WIFI_H

#include <Arduino.h>

extern const char* host;
extern const char* Privat_Key;
extern const char* City;
extern String results_0_now_text_str; 
extern int results_0_now_code_int; 
extern int results_0_now_temperature_int; 
extern String results_0_last_update_str;

void WiFi_Init();
void Client_Request();

#endif
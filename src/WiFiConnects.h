#ifndef WiFiConnects_H
#define WiFiConnects_H

#include "ExchangeWith1C.h"
#include <ESP8266WiFi.h>

Ticker WifiWinker;

void run_WiFi_AP() {

  DEBUG_MSGF("SSID: '%s'\n", SSID_DEFAULT);
  DEBUG_MSGF("Pass: '%s'\n", PASS_DEFAULT);

  //Создадим точку доступа
  WiFi.mode(WIFI_AP);
  WiFi.softAP(SSID_DEFAULT, PASS_DEFAULT, 1, false, 1);
  
  LedPrint("Для настройки подклю-чения, откройте в\nбраузере страницу\nhttp://192.168.4.1");
}

void winkWifi() {

  if (stateWiFiBlink) {
    stateWiFiBlink = false;
  } else
    stateWiFiBlink = true;

}

void run_WiFi_STA() {

  WifiWinker.detach();
  updateDate.detach();
  
  //Подключимся к указанной ранее точке доступа
  
  WifiWinker.attach(0.1, winkWifi);
  LedPrint("Подключение к сети\nWi-Fi");
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(config.ssid, config.pass);
  delay(500);
  WiFi.config(0U, 0U, 0U);
  delay(500);

  DEBUG_MSGF("Connecting to '%s'", config.ssid);
  String dots = "";
  while (WiFi.status() != WL_CONNECTED) {

    dots = dots + ".";
    LedPrint("Подключение к сети\nWi-Fi" + dots);
    
    DEBUG_MSGF(".");
    
    delay(500);
  }
  
  WifiWinker.detach();
  
  DEBUG_MSGF("\nConnected, IP address: ");
  DEBUG_MSG(WiFi.localIP());
  DEBUG_MSG("\n");
  
  updateDateFrom1C();
  
  stateWiFiBlink = true;
  updateDate.attach(3600, updateDateFrom1C);

  LedPrint("Подключено: " + WiFi.localIP().toString());
  delay(1000);
  LedClear();
  
}

#endif

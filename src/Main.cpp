#define DEBUG_ESP_PORT Serial

#ifdef DEBUG_ESP_PORT
#define DEBUG_BEGIN DEBUG_ESP_PORT.begin(115200);
#define DEBUG_MSGF(...) DEBUG_ESP_PORT.printf(__VA_ARGS__)
#define DEBUG_MSG(...) DEBUG_ESP_PORT.print(__VA_ARGS__)
#else
#define DEBUG_BEGIN
#define DEBUG_MSGF(...)
#define DEBUG_MSG(...)
#endif

#include <ESP8266WebServer.h>
#include <Ticker.h>

#define USEBUZZER

#define PIN_SETTINGS_MODE D3
#define PIN_BUZZER D6
#define PIN_FINGERPRINT_STATE D7

#define SSID_DEFAULT "FingerprintAP"
#define PASS_DEFAULT ""

#define ISR_PREFIX IRAM_ATTR

ESP8266WebServer webServer(80);
Ticker ticker;
Ticker fingerprintStateWaiting;

boolean settingsMode = false;
boolean runWiFiAP = false;
boolean stateWiFiBlink = false;

volatile boolean fingerprintRead = false;
volatile boolean makeRequestIn1C = false;
volatile boolean makeRequestOnSheduleIn1C = false;

#include "Other.h"
#include "DateTime.h"
#include "LED.h"
#include "Config.h"
#include "FirmwareReset.h"
#include "AdminPage.h"
#include "ExchangeWith1C.h"
#include "WiFiConnects.h"
#include "Fingerprint.h"
#include "Adafruit_I2CDevice.h"

void fingerprintStateWaitingEnd();

ISR_PREFIX void onPressedbuttonGetAction()
{
  if (settingsMode == false)
    makeRequestIn1C = true;
}

ISR_PREFIX void fingerprintState()
{

  if (!fingerprintStateWaiting.active()) {

    fingerprintRead = true;
    fingerprintStateWaiting.detach();
    fingerprintStateWaiting.attach(2, fingerprintStateWaitingEnd);
  }
  DEBUG_MSGF("fingerprintState. fingerprintRead: %d\n", fingerprintRead);
}

void setup()
{

  DEBUG_BEGIN
  DEBUG_MSG("\n");

  finger.begin(57600);
  delay(1000);
  
  finger.getParameters();
  DEBUG_MSGF("setup. finger.security_level: '%d'\n", finger.security_level);
  DEBUG_MSGF("setup. finger.capacity: '%d'\n", finger.capacity);

  String dots = "";

  beeper(2);

  InitConfig();
  LoadConfig();
  PrintConfig();

  InitLED();

  LedPrint("Загрузка");

  DEBUG_MSGF("setup. settingsMode: '%d'\n", settingsMode);

  //Если сделали сброс настроек тогда включим режим AP, чтобы можно было подключиться и указать параметры подключения к точке доступа
  if (checkResetFlag() || runWiFiAP)
  {
    
    settingsMode = true;
    DEBUG_MSGF("setup. settingsMode: '%d'\n", settingsMode);

    //Do the firmware reset here
    DEBUG_MSGF("setup. Reset Firmware\n");

    run_WiFi_AP();

    //Server a very basic page on the root url
    webServer.on("/", std::bind(serverAdmin, &webServer));
    webServer.begin();

    LedPrint("Подключитесь к Wi-Fi\n'" SSID_DEFAULT "' и\nзайдите на страницу\nhttp://192.168.4.1");
  }
  else
  {

    while (ticker.active())
    {

      delay(1000);

      dots = dots + ".";
      LedPrint("Загрузка" + dots);
    }

    settingsMode = false;
    DEBUG_MSGF("setup. settingsMode: '%d'\n", settingsMode);

    //Включим точку доступа
    run_WiFi_STA();

    delay(100);

    //Подключим прерывание, срабатывающее при прикосновении пальца к сканеру
    pinMode(PIN_FINGERPRINT_STATE, INPUT);
    attachInterrupt(digitalPinToInterrupt(PIN_FINGERPRINT_STATE), fingerprintState, FALLING);

    //Подключим прерывание, срабатывающее при нажатии кнопки "Получить данные из 1С"
    pinMode(PIN_SETTINGS_MODE, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_SETTINGS_MODE), onPressedbuttonGetAction, RISING);
  }
}

void loop()
{

  if (settingsMode == true)
  {

    webServer.handleClient();
  }
  else
  {
    if (makeRequestIn1C == true)
    {
      getActionFrom1C(true);
      makeRequestIn1C = false;
    }
    else if (makeRequestOnSheduleIn1C == true)
    {
      getActionFrom1C(false);
      makeRequestOnSheduleIn1C = false;

      delay(1000);
      fingerprintRead = false;
    }
    else if (fingerprintRead == true)
    {
      DEBUG_MSGF("loop. fingerprintRead: %d\n", fingerprintRead);

      int fingerprintID = readFingerprintIDez();

      if (fingerprintID < 0)
      {
        if (fingerprintID == -1) {

          LedPrint("Отказ", 35, 0, 2);
        } else{
          LedPrint("Отказ (" + String(-1 * fingerprintID) + "%)", 35, 0, 2);
        }

        beeper(2);
        delay(1000);
      }
      else
      {
        beeper(1);
        delay(1000);
        sendFingerprintIDTo1C(fingerprintID);
      }
      fingerprintRead = false;
    }

    if (WiFi.status() != WL_CONNECTED)
    {
      run_WiFi_STA();
    }
    LedPrint();
  }
  delay(100);
}

void fingerprintStateWaitingEnd(){

  fingerprintStateWaiting.detach();
}

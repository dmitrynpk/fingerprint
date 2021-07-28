#ifndef ExchangeWith1C_H
#define ExchangeWith1C_H

#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include "Fingerprint.h"
#include "Other.h"
#include "LED.h"

WiFiClient client;
HTTPClient http;
Ticker updateDate;

const String web_address = "http://fingerprint:687@192.168.253.6:90/perm-ut/hs/fingerprint/";

String connectTo1C() {

  const char * headerKeys[] = {"ScannerID"};
  const size_t numberOfHeaders = 1;
  String scannerID;

DEBUG_MSG("1.1.1\n");

  LedPrint("",0,0,1,true);
  
  String macAddress = WiFi.macAddress();

  DEBUG_MSG("[HTTP] begin connectTo1C...\n" + macAddress + "\n");

  boolean answer = http.begin(client, web_address + "connect");
  http.addHeader("Content-Type", "text/plain");
  http.addHeader("MAC-address", macAddress);
  http.collectHeaders(headerKeys, numberOfHeaders);
  http.setTimeout(10000);

  if (answer == true) {

    int httpCode = http.GET();

    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      DEBUG_MSGF("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {

        scannerID     = http.header("ScannerID");
        String answer = http.getString();

        DEBUG_MSG("ScannerID: " + scannerID + "\n");
        DEBUG_MSG("Answer: " + answer + "\n");
DEBUG_MSG("1.1.3\n");
      }
    } else {
      DEBUG_MSGF("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      
      LedPrint("Ошибка отправки данных");
      beeper(2);
      delay(1000);
    }

DEBUG_MSG("1.1.4\n");
    http.end();
DEBUG_MSG("1.1.5\n");
  }

DEBUG_MSG("1.1.6\n");
  LedPrint();
  
DEBUG_MSG("1.1.7\n");

  return scannerID;
}

void disconnectFrom1C(String scannerID) {

  DEBUG_BEGIN

  DEBUG_MSG("[HTTP] begin disconnectFrom1C...\n");

  LedPrint("",0,0,1,true);
  
  boolean answer = http.begin(client, web_address + "disconnect");
  http.addHeader("Content-Type", "text/plain");
  http.addHeader("ScannerID", scannerID);
  http.setTimeout(10000);
  
  dt.loadInit(http);
    
  if (answer == true) {

    int httpCode = http.GET();

    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      DEBUG_MSGF("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        
        dt.load(http);
        
        String answer  = http.getString();
        
        DEBUG_MSG("Answer: " + answer + "\n");
      }
    } else {
      DEBUG_MSGF("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      
      LedPrint("Ошибка отправки данных");
      beeper(2);
      delay(1000);
    }
    http.end();
  }
  LedPrint();
}

void sendFingerprintIDTo1C(int fingerprintID) {

  const char * headerKeys[] = {"Mark", "Employee"};
  const size_t numberOfHeaders = 2;
  String scannerID;
  
  scannerID = connectTo1C();

  if (scannerID == "") {

    scannerID = connectTo1C();
  }

  if (scannerID != "") {

    DEBUG_BEGIN
    delay(100);

    DEBUG_MSGF("[HTTP] begin sendFingerprint...\n");

    LedPrint("",0,0,1,true);
    
    http.begin(client, web_address + "sendFingerprintID");
    http.addHeader("Content-Type", "text/plain");
    http.addHeader("ScannerID", scannerID);
    http.addHeader("FingerprintID", String(fingerprintID));
    http.collectHeaders(headerKeys, numberOfHeaders);
    http.setTimeout(10000);

    int httpCode = http.GET();

    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      DEBUG_MSGF("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {

        String mark     = urlDecode(http.header("Mark"));
        String employee = urlDecode(http.header("Employee"));
        String answer   = http.getString();
        DEBUG_MSG("Mark: " + mark + "\n");
        DEBUG_MSG("Employee: " + employee + "\n");
        DEBUG_MSG("Answer: " + answer + "\n");
        
        beeper(1);
        
        if (mark == "Начало смены") {
          
          LedPrint("Начало\n   смены",30,0,2);
        }else {
          
          LedPrint("Окончание\n   смены",10,0,2);
        }
        
        delay(1000);
        LedPrint(employee,0,0,2);
        delay(1000);
        LedClear();
      }
    } else {
      DEBUG_MSGF("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      
      LedPrint("Ошибка отправки данных");
      beeper(2);
      delay(1000);
    }

    http.end();

    LedPrint();
    
    disconnectFrom1C(scannerID);
  }
}

int getActionQuantityFrom1C(String scannerID, boolean allData) {

  const char * headerKeys[] = {"ActionQuantity"};
  const size_t numberOfHeaders = 1;
  String actionQuantity;

  DEBUG_BEGIN
  delay(100);

  DEBUG_MSGF("[HTTP] begin getActionQuantity...\n");

  LedPrint("",0,0,1,true);
  
  boolean answer = http.begin(client, web_address + "getActionQuantity");
  http.addHeader("Content-Type", "text/plain");
  http.addHeader("ScannerID", scannerID);
  http.addHeader("allData", allData == true ? "true" : "false");
  http.collectHeaders(headerKeys, numberOfHeaders);
  http.setTimeout(10000);

  if (answer == true) {

    int httpCode = http.GET();

    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      DEBUG_MSGF("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {

        actionQuantity = http.header("ActionQuantity");
        String answer  = http.getString();

        DEBUG_MSG("ActionQuantity: " + actionQuantity + "\n");
        DEBUG_MSG("Answer: " + answer + "\n");
      }
    } else {
      DEBUG_MSGF("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      
      LedPrint("Ошибка отправки данных");
      beeper(2);
      delay(1000);
    }

    http.end();
  }

  LedPrint();
  
  return actionQuantity.toInt();
}

void sendFingerprintTo1C(String scannerID, String employee, uint8_t fingerprintID, uint8_t* fingerTemplate, boolean success) {

  DEBUG_BEGIN
  delay(100);

  DEBUG_MSGF("[HTTP] begin sendFingerprint...\n");

  LedPrint("",0,0,1,true);
  
  http.begin(client, web_address + "sendFingerprint");
  http.addHeader("Content-Type", "application/octet-stream");
  http.addHeader("Content-transfer-encoding", "binary");
  http.addHeader("Content-Length", "256");
  http.addHeader("ScannerID", scannerID);
  http.addHeader("Employee", employee);
  http.addHeader("FingerprintID", String(fingerprintID));
  http.addHeader("Success", success == true ? "true" : "false");
  http.setTimeout(10000);

  int httpCode = http.POST(fingerTemplate, 256);

  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    DEBUG_MSGF("[HTTP] POST... code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK) {

      String answer  = http.getString();
      DEBUG_MSG("Answer: " + answer + "\n");
    }
  } else {
    DEBUG_MSGF("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    
    LedPrint("Ошибка отправки данных");
    beeper(2);
    delay(1000);
  }
  
  http.end();

  LedPrint();
}

void sendFingerprintDeleted(String scannerID, String employee, String fingerprintID) {

  DEBUG_MSGF("[HTTP] begin sendFingerprintDeleted...\n");

  LedPrint("",0,0,1,true);
  
  http.begin(client, web_address + "sendFingerprintDeleted");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("ScannerID", scannerID);
  http.addHeader("Employee", employee);
  http.addHeader("FingerprintID", String(fingerprintID));
  http.setTimeout(10000);

  int httpCode = http.GET();

  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    DEBUG_MSGF("[HTTP] GET... code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK) {

      String answer  = http.getString();
      DEBUG_MSG("Answer: " + answer + "\n");
    }
  } else {
    DEBUG_MSGF("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();

  LedPrint();
}

void sendFingerprintScannerCleaned(String scannerID) {

  DEBUG_MSGF("[HTTP] begin sendFingerprintScannerCleaned...\n");

  LedPrint("",0,0,1,true);
  
  http.begin(client, web_address + "sendFingerprintScannerCleaned");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("ScannerID", scannerID);
  http.setTimeout(10000);

  int httpCode = http.GET();

  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    DEBUG_MSGF("[HTTP] GET... code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK) {

      String answer  = http.getString();
      DEBUG_MSG("Answer: " + answer + "\n");
    }
  } else {
    DEBUG_MSGF("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();

  LedPrint();
}

void getNextActionFrom1C(String scannerID, boolean allData) {

  const char * headerKeys[] = {"Action", "Employee", "EmployeeID", "FingerprintID"};
  const size_t numberOfHeaders = 4;

  String action = "";
  String employee;
  String employeeID;
  String fingerprintID;

  DEBUG_BEGIN

  DEBUG_MSGF("[HTTP] begin getNextAction...\n");

  LedPrint("",0,0,1,true);
  
  boolean answer = http.begin(client, web_address + "getNextAction");
  http.addHeader("Content-Type", "text/plain");
  http.addHeader("ScannerID", scannerID);
  http.addHeader("allData", allData == true ? "true" : "false");
  http.collectHeaders(headerKeys, numberOfHeaders);
  http.setTimeout(10000);

  uint8_t fingerTemplate[256];

  if (answer == true) {

    int httpCode = http.GET();

    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      DEBUG_MSGF("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {

        action        = http.header("Action");
        employee      = urlDecode(http.header("Employee"));
        employeeID    = http.header("EmployeeID");
        fingerprintID = http.header("FingerprintID");

        DEBUG_MSG("Action: " + action + "\n");
        DEBUG_MSG("Employee: " + employee + "\n");
        DEBUG_MSG("FingerprintID: " + fingerprintID + "\n");

        if (action == "Add") {

          WiFiClient data = http.getStream();
          data.readBytes(fingerTemplate, 256);

          int m = -1;
          for (int i = 0; i < 256; ++i) {
            m++;
            if (m == 16) {
              m = 0;
              DEBUG_MSG("\n");
            }
            printHex(fingerTemplate[i], 2);
          }
          DEBUG_MSG("\ndone.\n");
        }

      }
    } else {
      DEBUG_MSGF("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();

    delay(100);

    if (action != "") {
      uint8_t answerID;

      if (action == "New") {

        LedPrint("Сканирование\nотпечатка пальца:\n" + employee, 0, 0, 1);
        delay(1000);

        uint8_t fingerTemplate[256];

        boolean success = readFingerprintEnroll(fingerprintID.toInt(), fingerTemplate);

        if (success == true){
          LedPrint("Отпечаток успешно\nзаписан");
        }else{
          LedPrint("Запись отпечатка\nотменена");
        }
        delay(1000);
        
        sendFingerprintTo1C(scannerID, employeeID, fingerprintID.toInt(), fingerTemplate, success);

        LedClear();

      } else if (action == "Add") {

        answer = writeFingerprint(fingerprintID.toInt(), fingerTemplate);

        DEBUG_BEGIN
        delay(100);

        if (answer != FINGERPRINT_OK) {
          DEBUG_MSGF("\nError #%d\n", answer);
        } else {
          DEBUG_MSG("\nOK\n");
        }

      } else if (action == "Del") {

        answerID = deleteFingerprint(fingerprintID.toInt());

        if (answerID == FINGERPRINT_OK) {
          sendFingerprintDeleted(scannerID, employeeID, fingerprintID);
        }
      } else if (action == "Clear") {

        answerID = clearFingerprintScanner();

        if (answerID == FINGERPRINT_OK) {
          sendFingerprintScannerCleaned(scannerID);
        }
      }
    }
  }

  LedPrint();
}

void getActionFrom1C(boolean allData) {

  String scannerID;
  int actionQuantity;
  
DEBUG_MSG("1.1'\n");
  scannerID = connectTo1C();
DEBUG_MSG("1.2'\n");

  if (scannerID == "") {

DEBUG_MSG("1.3'\n");
    scannerID = connectTo1C();
DEBUG_MSG("1.4'\n");
  }

  if (scannerID != "") {

    actionQuantity = getActionQuantityFrom1C(scannerID, allData);

    for (int n = 0; n < actionQuantity; n++) {

      getNextActionFrom1C(scannerID, allData);
    }

    disconnectFrom1C(scannerID);
  }
}

void updateDateFrom1C() {

  makeRequestOnSheduleIn1C = true;

}

#endif

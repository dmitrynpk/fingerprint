#ifndef Fingerprint_H
#define Fingerprint_H

#include <Adafruit_Fingerprint.h>

#include "Other.h"
#include "ExchangeWith1C.h"

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial);

uint8_t getFingerprintIDez() {

  finger.begin(57600);
  
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;
  
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -2;
  
  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -3;
  
  return finger.fingerID; 
}

uint8_t getFingerprintEnroll(int fingerprintID) {
  
  uint8_t p;
  uint32_t starttime;

  DEBUG_BEGIN
  DEBUG_MSG("fingerprintRead: ");
  DEBUG_MSG(fingerprintRead);
  DEBUG_MSG("\n");
  
  finger.begin(57600);
  
  //Считываем палец

  LedPrint("Приложите палец");
  
  p         = -1;
  starttime = millis();

  while (p != FINGERPRINT_OK && (millis() - starttime) < 10000) {
    if (fingerprintRead == true) {
      p = finger.getImage();
      fingerprintRead = false;
    } else {
      yield();
    }
  }
  
  if (p != FINGERPRINT_OK) {
    return p;
  }

  beeper(1);

  LedPrint("Уберите палец");
  
  //Преобразуем изображение в шаблон
  
  p = finger.image2Tz(1);
  
  if (p != FINGERPRINT_OK) {
      return p;
  }

  //Считываем пустое изображение
  
  p         = -1;
  starttime = millis();
  
  while (p != FINGERPRINT_NOFINGER && (millis() - starttime) < 10000) {
    p = finger.getImage();
  }
  
  if (p != FINGERPRINT_NOFINGER) {
    return p;
  }
  
  //Считываем изображение повторно

  LedPrint("Повторно\nприложите палец");

  p         = -1;
  starttime = millis();
  
  while (p != FINGERPRINT_OK && (millis() - starttime) < 10000) {
    if (fingerprintRead == true) {
      p = finger.getImage();
      fingerprintRead = false;
    } else {
      yield();
    }
  }

  if (p != FINGERPRINT_OK) {
    return p;
  }
  
  beeper(1);

  LedPrint("Уберите палец");
  
  //Преобразуем изображение во 2-й шаблон
  
  p = finger.image2Tz(2);
  
  if (p != FINGERPRINT_OK) {
    return p;
  }
  
  //Создание модели из 2-х шаблонов
  
  p = finger.createModel();
  
  if (p != FINGERPRINT_OK) {
    return p;
  }   
  
  //Сохранение модели в сканере отпечатков пальцев
  
  p = finger.storeModel(fingerprintID);
  
  if (p != FINGERPRINT_OK) {
    return p;
  }
  
  beeper(2);
  
  return p;
}

uint8_t downloadFingerprintTemplate(uint16_t fingerprintID) {

  uint8_t p;

  finger.begin(57600);
  
  p = finger.loadModel(fingerprintID);
  if (p != FINGERPRINT_OK) {
    return p;
  }
  
  p = finger.getModel();
  if (p != FINGERPRINT_OK) {
    return p;
  }

  return p;
}

uint8_t writeFingerprint(uint16_t fingerprintID, uint8_t* fingerTemplate) {

  finger.begin(57600);
  
  uint8_t p;
  
  p = finger.sendModel();
  if (p != FINGERPRINT_OK) {
    return p;
  }
  
  uint32_t starttime = millis();
  int i = 0;
  while (i < 512 && (millis() - starttime) < 20000) {
      if (Serial.available()) {
          Serial.write(fingerTemplate[i++]);
      }
  }
  
  DEBUG_BEGIN
  delay(100);

  int m = -1;
  for (int i = 0; i < 512; ++i) {
    m++;
    if (m==16) {
      m=0;
      DEBUG_MSG("\n");
    }
    printHex(fingerTemplate[i], 2);
  }
  DEBUG_MSG("\ndone.\n");
  delay(100);

  return p;
  
}

boolean readFingerprintEnroll(uint16_t fingerprintID, uint8_t* fingerTemplate) {//Прочитать новый отпечаток пальца в сканер
  
  uint8_t answerID = 0;
  
  while (true) {
    
    answerID = getFingerprintEnroll(fingerprintID);
    
    if (answerID == FINGERPRINT_OK || answerID == FINGERPRINT_TIMEOUT) {
      break;
    }
  }
  
  if (answerID != FINGERPRINT_OK) {
     return false;
  }
  
  answerID = downloadFingerprintTemplate(fingerprintID);
  
  // one data packet is 267 bytes. in one data packet, 11 bytes are 'usesless' :D
  uint8_t bytesReceived[534]; // 2 data packets
  
  uint32_t starttime = millis();
  int i = 0;
  while (i < 534 && (millis() - starttime) < 20000) {
      if (Serial.available()) {
          bytesReceived[i++] = Serial.read();
      }
  }
  
  // filtering only the data packets
  int uindx = 9, index = 0;
  while (index < 534) {
      while (index < uindx) ++index;
      uindx += 256;
      while (index < uindx) {
          fingerTemplate[index++] = bytesReceived[index];
      }
      uindx += 2;
      while (index < uindx) ++index;
      uindx = index + 9;
  }
  
  DEBUG_BEGIN
  delay(100);

  int m = -1;
  for (int i = 0; i < 512; ++i) {
    m++;
    if (m==16) {
      m=0;
      DEBUG_MSG("\n");
    }
    printHex(fingerTemplate[i], 2);
  }
  DEBUG_MSG("\ndone.\n");
  delay(100);

  return true;
}

int readFingerprintIDez() {//Чтение идентификатора отпечатка пальца
  
  int fingerprintID = getFingerprintIDez();
  int confidence    = finger.confidence;

  if (confidence < 100 || fingerprintID > FINGERPRINT_SCANNER_CAPACITY) {

    fingerprintID = -1;
  }
  
  return fingerprintID;
}

uint8_t deleteFingerprint(uint8_t fingerprintID) {//Удалить отпечаток пальца из сканера

  finger.begin(57600);
  
  if (! finger.verifyPassword()) {
    return FINGERPRINT_PACKETRECIEVEERR;
  }
  
  uint8_t answerID = finger.deleteModel(fingerprintID);

  return answerID;
}

uint8_t clearFingerprintScanner() {//Очистка сканера

  finger.begin(57600);
  
  if (! finger.verifyPassword()) {
    return FINGERPRINT_PACKETRECIEVEERR;
  }
  
  uint8_t answerID = finger.emptyDatabase();

  return answerID;
}

ISR_PREFIX void fingerprintState() {
  fingerprintRead = true;
//  Serial.print("fingerprintRead: ");
//  Serial.println(fingerprintRead);
}
#endif

#ifndef Fingerprint_H
#define Fingerprint_H

#include <Adafruit_Fingerprint.h>

#include "Other.h"
#include "ExchangeWith1C.h"

SoftwareSerial mySerial(2, 3);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

uint8_t getFingerprintID() {

  finger.begin(57600);
  
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;
  
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -2;
  
  p = finger.fingerSearch();
  if (p != FINGERPRINT_OK)  return -3;
  
  return finger.fingerID; 
}

uint8_t getFingerprintEnroll(int fingerprintID) {
  
  uint8_t p;
  uint32_t starttime;

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

  LedPrint("Повторно приложите\nпалец");

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

boolean readFingerprintEnroll(uint16_t fingerprintID) {//Прочитать новый отпечаток пальца в сканер
  
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
  
  return true;
}

int readFingerprintIDez() {//Чтение идентификатора отпечатка пальца
  
  int fingerprintID = getFingerprintID();
  int confidence    = finger.confidence;

  DEBUG_MSG("readFingerprintIDez:\n");
  DEBUG_MSGF("  fingerprintID: %d\n", fingerprintID);
  DEBUG_MSGF("  confidence: %d\n", confidence);


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
#endif

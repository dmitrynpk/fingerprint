#ifndef Fingerprint_H
#define Fingerprint_H

#include <Adafruit_Fingerprint.h>

#include "Other.h"
#include "ExchangeWith1C.h"

SoftwareSerial mySerial(2, 3);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);


uint8_t getFingerprintID() {
  
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

  DEBUG_MSGF("getFingerprintEnroll. fingerprintRead: %d\n", fingerprintRead);
  
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

  delay(1000);
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
  
  delay(1000);

  //Считываем изображение повторно
  
  fingerprintStateWaiting.detach();

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
  
  delay(1000);
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

  DEBUG_MSGF("readFingerprintIDez. finger capacity: %d\n", finger.capacity);
  DEBUG_MSGF("readFingerprintIDez. fingerprintID: %d\n", fingerprintID);
  DEBUG_MSGF("readFingerprintIDez. confidence: %d\n", confidence);

  if (fingerprintID >= finger.capacity) {
    
    fingerprintID = -1;

  } else if (confidence < 25) {

    fingerprintID = -1 * confidence;
  }
  
  return fingerprintID;
}

uint8_t deleteFingerprint(uint8_t fingerprintID) {//Удалить отпечаток пальца из сканера
  
  if (! finger.verifyPassword()) {
    return FINGERPRINT_PACKETRECIEVEERR;
  }
  
  uint8_t answerID = finger.deleteModel(fingerprintID);

  return answerID;
}

uint8_t clearFingerprintScanner() {//Очистка сканера
  
  if (! finger.verifyPassword()) {
    return FINGERPRINT_PACKETRECIEVEERR;
  }
  
  uint8_t answerID = finger.emptyDatabase();

  return answerID;
}
#endif

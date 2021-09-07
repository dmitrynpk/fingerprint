#ifndef LED_h
#define LED_h

#include <SPI.h>
#include <Wire.h>

#include <SSD1306Wire.h>
#include "fontsRus.h"

SSD1306Wire display(0x3c, D2, D1);

char FontUtf8Rus(const byte ch);

void InitLED() {
  
  display.init();
  display.setFontTableLookupFunction(FontUtf8Rus);
  display.flipScreenVertically();
}

void LedPrint(String S = "", int x = 0, int y = 0, boolean stateUpdateFrom1C = false) {
  
  display.clear();

  // Вывести строку статуса

  display.setFont(ArialRus_Plain_10);
  
  if (stateWiFiBlink) {
    display.drawString(0, 0, "!");
  }else{
    display.drawString(0, 0, " ");
  }

  if (stateUpdateFrom1C) {
    display.drawString(2, 0, "*");
  }else{
    display.drawString(2, 0, " ");
  }
  
  String sDate = dt.getDate();
  
  display.drawString(20, 0, sDate);    

  // Вывести данные
  
  display.setFont(ArialRus_Plain_10);
  
  display.drawString(0, 10, S);
  display.display();
}

void LedClear() {
  
  LedPrint();
}

char FontUtf8Rus(const byte ch) { 
    static uint8_t LASTCHAR;

    if ((LASTCHAR == 0) && (ch < 0xC0)) {
      return ch;
    }

    if (LASTCHAR == 0) {
        LASTCHAR = ch;
        return 0;
    }

    uint8_t last = LASTCHAR;
    LASTCHAR = 0;
    
    switch (last) {
        case 0xD0:
            if (ch == 0x81) return 0xA8;
            if (ch >= 0x90 && ch <= 0xBF) return ch + 0x30;
            break;
        case 0xD1:
            if (ch == 0x91) return 0xB8;
            if (ch >= 0x80 && ch <= 0x8F) return ch + 0x70;
            break;
    }

    return (uint8_t) 0;
}
#endif

#ifndef LED_h
#define LED_h

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

char FontUtf8Rus(const byte ch);

void InitLED() {
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  delay(1000);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  
  display.cp437(true);
  display.clearDisplay(); 
  display.setTextColor(WHITE);
  display.display();
}

void LedPrint(String S = "", int x = 0, int y = 0, int s = 1, boolean stateUpdateFrom1C = false) {
  
  display.clearDisplay();

  // Вывести строку статуса

  display.setTextSize(1);
  
  display.setCursor(0, 0);

  if (stateWiFiBlink) {
    display.print("\xAD ");
  }else{
    display.print("  ");
  }

  if (stateUpdateFrom1C) {
    display.print("\xAC ");
  }else{
    display.print("  ");
  }
  
  String sDate = dt.getDate();
  
  display.println(utf8rus(sDate));    

  // Вывести данные
  
  display.setCursor(x, y + 16);
  display.setTextSize(s);
  
  display.println(utf8rus(S));
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

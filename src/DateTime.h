#ifndef DateTime_H
#define DateTime_H

#include <ESP8266HTTPClient.h>

class DateTime {

  private:
  
  uint16_t Year,Month,Day,Hour,Minute,Second,Millisecond,EndMonth,DayWeek;
  uint16_t startMillis;

  void calculate(uint16_t deltaMillis) {

    uint16_t countSecond,countMinute,countHour,countDay;
    
    Millisecond = Millisecond + deltaMillis;
    
    if (Millisecond > 999) {
      countSecond = Millisecond/1000;
      Millisecond = Millisecond - (countSecond*1000);
      Second = Second + countSecond;
    }

    if (Second > 59) {
      countMinute = Second/59;
      Second = Second - (countMinute*60);
      Minute = Minute + countMinute;
    }
    
    if (Minute > 59) {
      countHour = Minute/59;
      Minute = Minute - (countHour*60);
      Hour = Hour + countHour;
    }

    if (Hour > 23) {
      countDay = Hour/23;
      Hour = Hour - (countDay*24);
      Day = Day + countDay;

      DayWeek = DayWeek + 1;
      if (DayWeek > 7) {
        DayWeek = 1;
      }
    }
    
    if (Day > EndMonth) {
      Day = 1;
    }
  }
  
  public:

  void loadInit(HTTPClient &http) {
    
    const char * headerKeys[] = {"Year", "Month", "Day", "Hour", "Minute", "Second", "Millisecond", "EndMonth", "DayWeek"};
    const size_t numberOfHeaders = 9;
    
    http.collectHeaders(headerKeys, numberOfHeaders);
  }
  
  void load(HTTPClient &http) {

    Year        = http.header("Year").substring(2).toInt();
    Month       = http.header("Month").toInt();
    Day         = http.header("Day").toInt();
    Hour        = http.header("Hour").toInt();
    Minute      = http.header("Minute").toInt();
    Second      = http.header("Second").toInt();
    Millisecond = http.header("Millisecond").toInt();
    EndMonth    = http.header("EndMonth").toInt();
    DayWeek     = http.header("DayWeek").toInt();
    
    DEBUG_MSGF("%d.%d.%d %d:%d:%d:%d EndMonth-%d, DayWeek-%d\n", Year,Month,Day,Hour,Minute,Second,Millisecond,EndMonth,DayWeek);
    
    startMillis = millis();
  }
  
  String getDate() {
    
    uint32_t finishMillis;
    uint16_t deltaMillis;
    String s;
    
    finishMillis = millis();
    deltaMillis  = finishMillis - startMillis;
    
    calculate(deltaMillis);
    
    if (Day<10) {s = s + "0";}
    s = s + String(Day) + ".";
    if (Month<10) {s = s + "0";}
    s = s + String(Month) + ".";
    s = s + String(Year) + " ";
    
    switch (DayWeek)  {
      case 2: s = s + "Вт "; break;
      case 3: s = s + "Ср "; break;
      case 4: s = s + "Чт "; break;
      case 5: s = s + "Пт "; break;
      case 6: s = s + "Сб "; break;
      case 7: s = s + "Вс "; break;
      default: s = s + "Пн "; 
    }

    if (Hour<10) {s = s + "0";}
    s = s + String(Hour) + ":";
    if (Minute<10) {s = s + "0";}
    s = s + String(Minute);
    
    startMillis = finishMillis;

    return s;
  }

};

DateTime dt;

#endif

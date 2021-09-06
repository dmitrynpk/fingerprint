#ifndef Other_H
#define Other_H

void beeper(byte count) {

  for (int j = 1; j <= count ; j++) {
#ifdef USEBUZZER
    tone(PIN_BUZZER, 1000);
    delay(200);
    noTone(PIN_BUZZER);
    delay(200);
#endif
  }
}

void printHex(int num, int precision) {

    char tmp[16];
    char format[128];
    /*
    DEBUG_MSG(num);
    */
    sprintf(format, "%%.%dX", precision);
    sprintf(tmp, format, num);
    DEBUG_MSGF(tmp);
    DEBUG_MSG(",");
}

unsigned char h2int(char c) {
  
    if (c >= '0' && c <='9'){
        return((unsigned char)c - '0');
    }
    if (c >= 'a' && c <='f'){
        return((unsigned char)c - 'a' + 10);
    }
    if (c >= 'A' && c <='F'){
        return((unsigned char)c - 'A' + 10);
    }
    return(0);
}

String urlDecode(String str) {
    
    String encodedString="";
    char c;
    char code0;
    char code1;
    for (int i =0; i < str.length(); i++){
        c=str.charAt(i);
      if (c == '+'){
        encodedString+=' ';  
      }else if (c == '%') {
        i++;
        code0=str.charAt(i);
        i++;
        code1=str.charAt(i);
        c = (h2int(code0) << 4) | h2int(code1);
        encodedString+=c;
      } else{
        
        encodedString+=c;  
      }
      
      yield();
    }
    
   return encodedString;
}

#endif

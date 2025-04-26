
#ifndef TimeFun_H
#define TimeFun_H

// Para el tiempo
//-------------------------------------
#include <ESP32Time.h>

//ESP32Time rtc;        // Horario sin desplazamiento 
ESP32Time rtc(0); // Horario -6 UTC de Costa Rica -3600*ZONA_HORARIA

void setCurrentInternalTime() {
  //
  rtc.setTime(30, 24, 15, 17, 1, 2021);  // 17th Jan 2021 15:24:30  
}

String getCurrentInternalTime() {
  //
  Serial.println(rtc.getTime("RTC0: %A, %B %d %Y %H:%M:%S"));   // (String) returns time with specified format 
  Serial.println(rtc.getLocalEpoch());         //  (unsigned long) epoch without offset, same for all instances

  return rtc.getTime("%A, %B %d %Y %H:%M:%S");

  //  Serial.println(rtc.getTime());          //  (String) 15:24:38
  //  Serial.println(rtc.getDate());          //  (String) Sun, Jan 17 2021
  //  Serial.println(rtc.getDate(true));      //  (String) Sunday, January 17 2021
  //  Serial.println(rtc.getDateTime());      //  (String) Sun, Jan 17 2021 15:24:38
  //  Serial.println(rtc.getDateTime(true));  //  (String) Sunday, January 17 2021 15:24:38
  //  Serial.println(rtc.getTimeDate());      //  (String) 15:24:38 Sun, Jan 17 2021
  //  Serial.println(rtc.getTimeDate(true));  //  (String) 15:24:38 Sunday, January 17 2021
  //
  //  Serial.println(rtc.getMicros());        //  (long)    723546
  //  Serial.println(rtc.getMillis());        //  (long)    723
  //  Serial.println(rtc.getEpoch());         //  (long)    1609459200
  //  Serial.println(rtc.getSecond());        //  (int)     38    (0-59)
  //  Serial.println(rtc.getMinute());        //  (int)     24    (0-59)
  //  Serial.println(rtc.getHour());          //  (int)     3     (1-12)
  //  Serial.println(rtc.getHour(true));      //  (int)     15    (0-23)
  //  Serial.println(rtc.getAmPm());          //  (String)  pm
  //  Serial.println(rtc.getAmPm(true));      //  (String)  PM
  //  Serial.println(rtc.getDay());           //  (int)     17    (1-31)
  //  Serial.println(rtc.getDayofWeek());     //  (int)     0     (0-6)
  //  Serial.println(rtc.getDayofYear());     //  (int)     16    (0-365)
  //  Serial.println(rtc.getMonth());         //  (int)     0     (0-11)
  //  Serial.println(rtc.getYear());          //  (int)     2021
}


// Los siguiente es para utilizar la hora de un rtc externo
//================================================================================================

#include "RTClib.h"
RTC_DS1307 rtcExt;

TwoWire WireRTC = TwoWire(1);  // para usar el segundo I2C

uint8_t setupExternalRTC() {

  //Wire2.begin(25, 26);  // SDA, SCL  // para usar el segundo I2C
  WireRTC.begin(I2C_SDA, I2C_SCL);  // SDA, SCL  // para usar el segundo I2C

  if (! rtcExt.begin(&WireRTC)) {  // &Wire2
    Serial.println("Couldn't find RTC");
    Serial.flush();
    //while (1) delay(10);  // Esperar a que se reinicie
    return 1;
  }

  if (! rtcExt.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtcExt.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    //rtcExt.adjust(DateTime(2014, 1, 21, 3, 0, 0));

    return 2;  // Indica que la hora seteada es la de fabrica
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // rtcExt.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtcExt.adjust(DateTime(2014, 1, 21, 3, 0, 0));

  return 0;
}


bool sincronizarHoraRTC(int timeZone = -6) {
  rtc.offset = 3600*timeZone; // change offset value

  // Sincroniza la hora del RTC interno con la del RTC externo
  if (! rtcExt.isrunning()) {
    Serial.println("RTC externo no está funcionando.");
    return false;
  }

  //DateTime now = rtcExt.now();  // Obtener la hora del RTC externo

  rtc.setTime(rtcExt.now().unixtime() - 3600*timeZone);  // 1st Jan 2021 00:00:00

  //rtc.offset = 7200; // change offset value
  //rtc.setTime(now.second(), now.minute(), now.hour(), now.day(), now.month(), now.year());

  Wire.endTransmission();  // Finalizar el uso del RTC una vez sincronizada la hora

  return true;
}


String getCurrentExternalTime() {
  //
  DateTime now = rtcExt.now();
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
  return String(now.year(), DEC) + '/' + String(now.month(), DEC) + '/' + String(now.day(), DEC) + ' ' + String(now.hour(), DEC) + ':' + String(now.minute(), DEC) + ':' + String(now.second(), DEC);
}



// Funcion para obtener el tiempo de un RTC externo
uint32_t getExternalEpoch(){
    // Obtiene el tiempo de un RTC externo
    return rtcExt.now().unixtime();
}


#endif // TimeFun_H
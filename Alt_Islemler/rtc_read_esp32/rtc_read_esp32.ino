#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 rtc;

String rtc_time = ",,;,,";

bool rtc_find_state = false;
bool rtc_running_state = false;

void setup () 
{
  Serial.begin(115200);
  
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  //rtc.adjust(DateTime(2022, 6, 18, 18, 6, 0));
  
  rtc_find_state = rtc.begin();
  if (!rtc_find_state) 
  {
    Serial.println("Couldn't find RTC");
  }

  rtc_running_state = rtc.isrunning();
  if (!rtc_running_state)
  {
    Serial.println("RTC is NOT running!");
  }
  
}

void loop () 
{
  // 19,6,2022;13,40,29
    rtc_time = rtc_okuma(&rtc);
    Serial.println(rtc_time);
    delay(1000);
}


String rtc_okuma(RTC_DS1307 *newRTC)
{
    String time_payload = "";
    
    rtc_find_state = newRTC->begin();
    rtc_running_state = newRTC->isrunning();
    if (rtc_find_state && rtc_running_state)
    {
      DateTime now = newRTC->now();
      time_payload = String(now.day()) + "," + String(now.month()) + "," + String(now.year()) + ";" + String(now.hour()) + "," + String(now.minute()) + "," + String(now.second());
    }
    else
    {
      Serial.println("RTC ERROR!");
      time_payload = ",,;,,";
    }
    
    return time_payload;
}

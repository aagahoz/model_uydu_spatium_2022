#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

#include <Wire.h>
#include <Adafruit_BMP280.h>
#include "RTClib.h"
//#include <WiFi.h>
//#include <WiFiUdp.h>

#include "LoRa_E32.h"
#define RXD2 16
#define TXD2 17
LoRa_E32 e32ttl(&Serial2);
typedef  struct 
{
  float tas_latitude;
  float tas_longtitude;
  float tas_gps_altitude;
  float tas_pressure;
  float tas_altitude;
} Signal;
Signal data;

#include <Servo.h>

Adafruit_BMP280 bmp;
RTC_DS1307 newRTC;
//WiFiUDP udp;
Servo myservo1;  
Servo myservo2;

void TaskRTC( void *pvParameters );
void TaskBMP280( void *pvParameters );
void TaskLoRa( void *pvParameters );

bool rtc_find_state = false;
bool rtc_running_state = false;

////////////////////////////////////////
const String p1_takimNo = "389590";
String p2_paketNumarasi = "0";
String p3_gondermeSaati = ",,,,,";
String p4_basinc1 = "";
String p5_basinc2 = "";
String p6_yukseklik1 = "";
String p7_yukseklik2 = "";
String p8_irtifaFarki = "";
String p9_inisHizi = "";
String p10_sicaklik = "";
String p11_pilGerilimi = "";
String p12_gps1Latitude = "";
String p13_gps2Latitude = "";
String p14_gps1Altitude = "";
String p15_gps2Altitude = "";
String p16_gps1Longtitude = "";
String p17_gps2Longtitude = "";
String p18_uyduStatusu = "";
String p19_pitch = "";
String p20_roll = "";
String p21_yaw = "";
String p22_donusSayisi = "";
String p21_videoAktarimBilgisi = "";
////////////////////////////////////////


void setup() {
  
  Serial.begin(115200);
  
  unsigned status;
  status = bmp.begin(0x76);
  
  e32ttl.begin();
  delay(500);
  
  rtc_find_state = newRTC.begin();
  if (!rtc_find_state) 
  {
    Serial.println("Couldn't find RTC");
  }

  rtc_running_state = newRTC.isrunning();
  if (!rtc_running_state)
  {
    Serial.println("RTC is NOT running!");
  }
  
  // Now set up two tasks to run independently.
  xTaskCreatePinnedToCore(
    TaskRTC
    ,  "TaskRTC"   // A name just for humans
    ,  4096  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    TaskBMP280
    ,  "TaskBMP"
    ,  4096  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    TaskLoRa
    ,  "TaskBMP"
    ,  4096  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);
    
}

void loop(){}

void TaskRTC(void *pvParameters)
{
  (void) pvParameters;

  for (;;) 
  {    
    rtc_find_state = newRTC.begin();
    rtc_running_state = newRTC.isrunning();
    if (rtc_find_state && rtc_running_state)
    {
      DateTime now = newRTC.now();
      p3_gondermeSaati = String(now.day()) + "," + String(now.month()) + "," + String(now.year()) + ";" + String(now.hour()) + "," + String(now.minute()) + "," + String(now.second());
      Serial.println("RTC--> " + p3_gondermeSaati);
    }
    else
    {
      Serial.println("RTC ERROR!");
      p3_gondermeSaati = ",,;,,";
    }
    vTaskDelay(800); 
  }
}

void TaskBMP280(void *pvParameters) 
{
  (void) pvParameters;

  for (;;)
  {
    p10_sicaklik = String(bmp.readTemperature());
    p4_basinc1 = String(bmp.readPressure());
    p6_yukseklik1 = String(bmp.readAltitude(1017.5));
    Serial.println("BMP280--> " + p4_basinc1 + "," + p10_sicaklik + "," + p6_yukseklik1);
    vTaskDelay(800);  
  }
}

void TaskLoRa(void *pvParameters) 
{
  (void) pvParameters;

  for (;;)
  {
     while (e32ttl.available()  > 1) 
     {
        ResponseStructContainer rsc = e32ttl.receiveMessage(sizeof(Signal));
        data = *(Signal*) rsc.data;
        rsc.close();
        p5_basinc2 = data.tas_pressure;
        p7_yukseklik2 = data.tas_altitude;
        p13_gps2Latitude = data.tas_latitude;
        p15_gps2Altitude = data.tas_gps_altitude;
        p17_gps2Longtitude  = data.tas_longtitude;
        Serial.println("LoRa--> " + p5_basinc2 + "," + p7_yukseklik2 + "," + p13_gps2Latitude + "," + p15_gps2Altitude + "," + p17_gps2Longtitude);
    }
    vTaskDelay(800);  
  }
}

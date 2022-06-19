#include <WiFi.h>
#include <WiFiUdp.h>

#include <Servo.h>

#include <Wire.h>
#include "RTClib.h"

Servo myservo1;  
Servo myservo2;

RTC_DS1307 rtc;

//const char * ssid = "AC-ESP32";
//const char * pwd = "987654321";

const char * ssid = "SPATIUM";
const char * pwd = "team.spatium";

//const char * udpAddress = "192.168.4.24";

const char * udpAddress = "192.168.31.132";
const int udpPort = 44444;
char komut_arrayi[4]; // 4 bitlik komut arrayi 0000 manuel Servo - motor tahrik - bos - bos

String rtc_time = ",,;,,";

bool servolar_acik_mi = true;

bool rtc_find_state = false;
bool rtc_running_state = false;

WiFiUDP udp;

void setup()
{
  Serial.begin(115200);
  
  WiFi.begin(ssid, pwd);
  
  myservo1.attach(12);
  myservo2.attach(13);

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

void loop()
{
  rtc_time = rtc_okuma(&rtc); //RTC Okuma
  Serial.println(rtc_time);


  // Telemetri paketi hazirlama
  char udp_payload[250];
  String datalar = "2929,1,14:30,5,6,15,20,10,5,28,98,40.806298,29.355541,258,40.806298,29.355541,2564,1,10,20,5,5,EVET";
  datalar.toCharArray(udp_payload, 250);
  int sizePayload = 0;
  for (int i=0; udp_payload[i] != '\0'; i++) // olusan telemetri dizisinin boyutunu hesaplar
  {
    sizePayload += 1;
  }

  // Yer Istasyonuna veri gonderme
  udp.beginPacket(udpAddress, udpPort);  // WiFiUDP.beginPacket(hostIp, port);
  udp.write((uint8_t *)udp_payload, sizePayload);  // WiFiUDP.write(buffer, size);
  Serial.print("Payload: ");
  Serial.println(sizePayload);
  udp.endPacket(); // paketi gonderir, basarili ise 1, basarisiz ise 1 dondurur
  
  // Yer Istasyonundan komut alma 
  udp.parsePacket();  // gelen datayi okunabilir hale getirir, read fonks dan once kullanilmali
  if(udp.read(komut_arrayi, 4) > 0)  // esp32 ye gelen datalari okur
  {
    Serial.print("Server to client ( Yer istasyonundan esp32`ye ) : ");
    Serial.println(komut_arrayi);  // alinan komutlar bastirilir
    
    if (komut_arrayi[0] == '0')  // servo komut durumu
    {
      servolar_acik_mi = false;
    }
    if (komut_arrayi[0] == '1')
    {
      servolar_acik_mi = true;
    }
  }
  
  udp.flush(); // istemciye yazilmis ancak okunmamis datalari siler

  // alinan komut son durumuna gore servo acip kapatma 0000 - 0001
  servo_kontrolu(servolar_acik_mi, &myservo1, &myservo2);

  // char arrayini resetleme
  memset(udp_payload, 0, 120);
  delay(1000);
}

void servo_kontrolu(bool durum, Servo *servo1, Servo *servo2)
{
  if (durum == true)
  {
    servo1->write(0);
    servo2->write(0);
  }

  else if (durum == false)
  {
    servo1->write(90);
    servo2->write(90);
  }
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

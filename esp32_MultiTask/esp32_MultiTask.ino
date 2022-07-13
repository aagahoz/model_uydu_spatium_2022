/*          TASK DURUMLARI
 *  Telemetri gonderimi       -
 *  Komut gonderimi           -
 *  Dosya aktarımı            -
 *  Servo motor kontrol       -
 *  Analog Pil okuma          -
 *  RTC okuma                 +
 *  BMP280 okuma              +
 *  SD Card telemetri yazma   -
 *  Raspberry pi UART         -
 *  LoRa UART                 +
 
 SD CARD PinOut   
    Vcc         ->   5V
    Ground      ->   Ground
    MOSI        ->   23     
    MISO        ->   19
    CS          ->   5     
    CLK         ->   18
    
 RTC PinOut
    Vcc         ->   5V
    Ground      ->   Ground
    SDA         ->   21     
    SCL         ->   22

 BMP280 PinOut
    Vcc         ->   5V
    Ground      ->   Ground
    SDA         ->   21     
    SCL         ->   22
    
 Analog Pil Gerilimi PinOut
    Analog Pin  ->  34
    GND         ->  Ground
  
 Servo1 PinOut
    Vcc         ->   Regulator +
    Ground      ->   Ground
    PWM Sinyal  ->   12
    
 Servo2 PinOut
    Vcc         ->   Regulator +
    Ground      ->   Ground
    PWM Sinyal  ->   13
  
 Raspberry Pi PinOut
    Ground      ->  Ground
    TX          ->  RX
    RX          ->  TX

 LoRa PinOut
    5V          ->  5V
    M0          ->  Ground
    M1          ->  Ground
    AUX         ->  Ground
    Ground      ->  Ground
    TX          ->  16
    RX          ->  17

  Fırçasız Motor1 PinOut
    Ground      ->   Ground
    PWM Sinyal  ->   25
  
 Fırçasız Motor2 PinOut
    Ground      ->   Ground
    PWM Sinyal  ->   26
  
 Fırçasız Motor3 PinOut
    Ground      ->   Ground
    PWM Sinyal  ->   27
 */
 
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

//const char * ssid = "AC-ESP32";
//const char * pwd = "987654321";

const char * ssid = "SPATIUM";
const char * pwd = "team.spatium";

//const char * udpAddress = "192.168.4.24";
const char * udpAddress = "192.168.31.132";
const int udpPort = 44444;
char gelen_komut[5]; // 4 bitlik komut arrayi 0000 manuel Servo - motor tahrik - bos - bos

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

Adafruit_BMP280 BMP280;
RTC_DS1307 RTC;
//WiFiUDP udp;
Servo servo1;  
Servo servo2;

void TaskRTC( void *pvParameters );
void TaskBMP280( void *pvParameters );
void TaskLoRa( void *pvParameters );
void TaskBatteryVoltage( void *pvParameters );
void TaskRaspberryPiUART(void *pvParameters);
void TaskServoControl(void *pvParameters);
void TaskTelemetryCommunication(void *pvParameters);
void TaskFileTransfer(void *pvParameters);
void TaskTelemeryLoggerSdCard(void *pvParameters);
void TaskMotorControl(void *pvParameters);

bool rtc_find_state = false;
bool rtc_running_state = false;

bool servolari_ac = true;

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
String p23_videoAktarimBilgisi = "";
////////////////////////////////////////
String main_payload = "";

void setup() {
  
  Serial.begin(115200);

//  WiFi.begin(ssid, pwd);
  
  unsigned status;
  status = BMP280.begin(0x76);
  
  e32ttl.begin();
  delay(500);
  
  rtc_find_state = RTC.begin();
  if (!rtc_find_state) 
  {
    Serial.println("Couldn't find RTC");
  }

  rtc_running_state = RTC.isrunning();
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
    ,  "TaskBMP280"
    ,  4096  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    TaskLoRa
    ,  "TaskLoRa"
    ,  4096  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    TaskBatteryVoltage
    ,  "TaskBatteryVoltage"
    ,  4096  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);
  
  xTaskCreatePinnedToCore(
    TaskRaspberryPiUART
    ,  "TaskRaspberryPiUART"
    ,  4096  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    TaskServoControl
    ,  "TaskServoControl"
    ,  4096  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);
  
  xTaskCreatePinnedToCore(
    TaskTelemetryCommunication
    ,  "TaskTelemetryCommunication"
    ,  4096  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    TaskTelemeryLoggerSdCard
    ,  "TaskTelemeryLoggerSdCard"
    ,  4096  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);
  
  xTaskCreatePinnedToCore(
    TaskFileTransfer
    ,  "TaskFileTransfer"
    ,  4096  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    TaskMotorControl
    ,  "TaskFileTransfer"
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
    rtc_find_state = RTC.begin();
    rtc_running_state = RTC.isrunning();
    if (rtc_find_state && rtc_running_state)
    {
      DateTime now = RTC.now();
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
    p10_sicaklik = String(BMP280.readTemperature());
    p4_basinc1 = String(BMP280.readPressure());
    p6_yukseklik1 = String(BMP280.readAltitude(1017.5));
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

void TaskBatteryVoltage(void *pvParameters) 
{
  (void) pvParameters;

  for (;;)
  {

    vTaskDelay(800);  
  }
}

void TaskRaspberryPiUART(void *pvParameters) 
{
  (void) pvParameters;

  for (;;)
  {

    vTaskDelay(800);  
  }
}

void TaskServoControl(void *pvParameters) 
{
  (void) pvParameters;

  for (;;)
  {
    if (servolari_ac == true)     // ACILAR AYARLANACAK
    {
      servo1.write(0);
      servo2.write(0);
    }
  
    else if (servolari_ac == false)
    {
      servo1.write(90);
      servo2.write(90);
    }
    vTaskDelay(800);  
  }
}

void TaskMotorControl(void *pvParameters) 
{
  (void) pvParameters;

  for (;;)
  {

    vTaskDelay(800);  
  }
}

void TaskTelemetryCommunication(void *pvParameters) 
{
  (void) pvParameters;

  for (;;)
  {
    /*
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
    telemetri_gonderilme_durum = udp.endPacket(); // paketi gonderir, basarili ise 1, basarisiz ise 1 dondurur
    if (udp.endPacket() == 1)
      Serial.println("--> UDP Paket Gonderildi");
    else
      Serial.println("--> UDP Paket Alinmadi");
  
    
    // Yer Istasyonundan komut alma 
    udp.parsePacket();  // gelen datayi okunabilir hale getirir, read fonks dan once kullanilmali
    if(udp.read(gelen_komut, 4) > 0)  // esp32 ye gelen datalari okur
    {
      Serial.print("Server to client ( Yer istasyonundan esp32`ye ) : ");
      Serial.println(gelen_komut);  // alinan komutlar bastirilir
      strcpy(komut_durumu, gelen_komut);
    }
    
    if (komut_durumu[0] == '0')  // servo komut durumu
    {
      servolar_acik_mi = false;
    }
    if (komut_durumu[0] == '1')
    {
      servolar_acik_mi = true;
    }
    
    Serial.print("Komut Durumu: ");
    Serial.println(komut_durumu);
    udp.flush(); // istemciye yazilmis ancak okunmamis datalari siler
    
    // char arrayini resetleme
    memset(udp_payload, 0, 120);
    */
    vTaskDelay(800);  
  }
}

void TaskFileTransfer(void *pvParameters) 
{
  (void) pvParameters;

  for (;;)
  {

    vTaskDelay(800);  
  }
}

void TaskTelemeryLoggerSdCard(void *pvParameters) 
{
  (void) pvParameters;

  for (;;)
  {
    main_payload = p1_takimNo + "," + p2_paketNumarasi
    + "," + p3_gondermeSaati + "," + p4_basinc1 + "," + p5_basinc2
    + "," + p6_yukseklik1 + "," + p7_yukseklik2 + "," + p8_irtifaFarki
    + "," + p9_inisHizi + "," + p10_sicaklik + "," + p11_pilGerilimi
    + "," + p12_gps1Latitude + "," + p13_gps2Latitude
    + "," + p14_gps1Altitude + "," + p15_gps2Altitude
    + "," + p16_gps1Longtitude + "," + p17_gps2Longtitude
    + "," + p18_uyduStatusu + "," + p19_pitch + "," + p20_roll
    + "," + p21_yaw + "," + p22_donusSayisi + "," + p23_videoAktarimBilgisi;
    Serial.print("main_payload > ");
    Serial.println(main_payload);

    vTaskDelay(800);  
  }
}
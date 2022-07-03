/*
 *  LoRa Receiver       -
 *  BMP280              +
 *  Raspberry Pi UART   -

   LoRa UART Pinout
    -----
      
   BMP280 PinOut   
    Vcc         ->   3.3V
    Ground      ->   Ground
    SDA         ->   A4     
    SCL         ->   A5
    
   ESP32 PinOut

    ??

 
 */

#include <Wire.h>
#include <Adafruit_BMP280.h>
#include "LoRa_E32.h"
#include <SoftwareSerial.h>

Adafruit_BMP280 bmp; 
SoftwareSerial mySerial(10, 11); // Arduino RX <-- e32 TX, Arduino TX --> e32 RX
LoRa_E32 e32ttl(&mySerial);

typedef  struct 
{
  String tas_latitude;
  String tas_longtitude;
  String tas_gps_altitude;
  String tas_pressure;
  String tas_altitude;
} Signal;
Signal tasiyici_lora;

String bmp_payload = "";
String lora_payload = "";
String main_payload = "";

void setup()
{
  Serial.begin(9600);
  
  e32ttl.begin();
  
  unsigned status;
  status = bmp.begin(0x76);
}

void loop() 
{
    bmp_payload = bmp_returner();
    lora_payload = lora_returner();

    main_payload = bmp_payload + "," +  lora_payload; 
    
    Serial.println(main_payload);
    
    main_payload = ",,,,,,";
    lora_payload = ",,,,";
    bmp_payload = ",,";
    
    delay(50);
    Serial.flush();
}

String bmp_returner()
{    
    String payload = ",,";
    payload = String(bmp.readTemperature()) + "," + String(bmp.readPressure()) + "," + String(bmp.readAltitude(1017.5));
    
//    Serial.println(bmp_payload);
    return payload;
}


String lora_returner()
{
    String payload = ",,,,";

    while (e32ttl.available()  > 1) 
    {
      ResponseStructContainer rsc = e32ttl.receiveMessage(sizeof(Signal));
      tasiyici_lora = *(Signal*) rsc.data;
      rsc.close();
    }
    payload = tasiyici_lora.tas_latitude + "," + tasiyici_lora.tas_longtitude + "," + tasiyici_lora.tas_gps_altitude + "," + tasiyici_lora.tas_pressure + "," + tasiyici_lora.tas_altitude;
}

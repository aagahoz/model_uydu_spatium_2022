/*
 *  LoRa Receiver   +
 *  BMP280          +
 *  ESP32 UART      +

   LoRa UART Pinout
    Vcc         ->   5V
    Ground      ->   Ground
    Tx          ->   11     
    Rx          ->   10
    AUX         ->   Ground     
    M0          ->   Ground
    M1          ->   Ground
      
   BMP280 PinOut   
    Vcc         ->   3.3V
    Ground      ->   Ground
    SDA         ->   A4     
    SCL         ->   A5
    
   ESP32 PinOut
    Ground     ->   Ground
    Tx         ->   4
    Rx         ->   3
*/

#include <Wire.h>
#include <Adafruit_BMP280.h>
#include "LoRa_E32.h"
#include <SoftwareSerial.h>

Adafruit_BMP280 bmp; 
SoftwareSerial mySerial(10, 11); // Arduino RX <-- e32 TX, Arduino TX --> e32 RX
SoftwareSerial send_to_esp32(3, 4); // Arduino RX <-- esp32 TX, Arduino TX --> esp32 RX
LoRa_E32 e32ttl(&mySerial);

typedef  struct 
{
  float tas_latitude;
  float tas_longtitude;
  float tas_gps_altitude;
  float tas_pressure;
  float tas_altitude;
} Signal;
Signal tasiyici_lora;

String bmp_payload = "";
String lora_payload = "";
String main_payload = "";

void setup()
{
  Serial.begin(9600);
  send_to_esp32.begin(38400);
  e32ttl.begin();
  
  unsigned status;
  status = bmp.begin(0x76);
}

void loop() 
{
    bmp_payload = bmp_returner();
    lora_payload = lora_returner();

    
    main_payload = bmp_payload + "," + lora_payload; 

    send_to_esp32.println(main_payload);
    Serial.println(main_payload);
    
    main_payload = ",,,,,,";
    lora_payload = ",,,,";
    bmp_payload = ",,";
    
    delay(200);
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
      Serial.println("succes");
    }
    payload = String(tasiyici_lora.tas_latitude) + "," + String(tasiyici_lora.tas_longtitude) + "," + String(tasiyici_lora.tas_gps_altitude) + "," + String(tasiyici_lora.tas_pressure) + "," + String(tasiyici_lora.tas_altitude);
    return payload;
}

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

Adafruit_BMP280 bmp; 

String bmp_payload = "";
String main_payload = "";

void setup()
{
  Serial.begin(9600);
  
  unsigned status;
  status = bmp.begin(0x76);
}

void loop() 
{
    bmp_payload = bmp_returner();
    main_payload = bmp_payload + "," ;

    Serial.println(main_payload);


    main_payload = "";

    delay(50);
    Serial.flush();
}

String bmp_returner()
{    
    String payload = "";
    payload = String(bmp.readTemperature()) + "," + String(bmp.readPressure()) + "," + String(bmp.readAltitude(1017.5));
    
//    Serial.println(bmp_payload);
    return payload;
}

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
    
   Raspberry Pi PinOut

    ??

 
 */

#include <Wire.h>
#include <Adafruit_BMP280.h>

String bmp_payload = "";


void setup()
{
  unsigned status;
  status = bmp.begin(0x76);
}

void loop() 
{
    bmp_payload = bmp_returner();
    main_payload = bmp_payload + "," ;

    Serial.println(main_payload);


    main_payload = "";
    Serial.flush();

}

String bmp_returner()
{
    String bmp_temprature = "";
    String bmp_pressure = "";
    String bmp_altitude = "";
    String payload = "";
    
    bmp_temprature = String(bmp.readTemperature());
    bmp_pressure = String(bmp.readPressure());
    bmp_altitude = String(bmp.readAltitude(1017.5));
    payload = bmp_pressure + "," + bmp_temprature + "," + bmp_altitude;
    
//    Serial.println(bmp_payload);
    return payload;
}

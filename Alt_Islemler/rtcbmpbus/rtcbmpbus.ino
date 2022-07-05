#include <Wire.h>
#include <ds3231.h>
#include <Adafruit_BMP280.h>
Adafruit_BMP280 bmp; // I2C

struct ts t; 
 
void setup() {
  Serial.begin(9600);
  Wire.begin();
  while ( !Serial ) delay(100);
  DS3231_init(DS3231_CONTROL_INTCN);
  /*----------------------------------------------------------------------------
  In order to synchronise your clock module, insert timetable values below !
  ----------------------------------------------------------------------------*/
//  t.hour=12; 
//  t.min=30;
//  t.sec=0;
//  t.mday=25;
//  t.mon=12;
//  t.year=2021;
// 
//  DS3231_set(t); 
  Serial.println(F("BMP280 test"));
  unsigned status;
  status = bmp.begin(0x76);
  if (!status) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
    while (1) delay(10);
  }
}
 
void loop() {
  DS3231_get(&t);
  Serial.print("Date : ");
  Serial.print(t.mday);
  Serial.print("/");
  Serial.print(t.mon);
  Serial.print("/");
  Serial.print(t.year);
  Serial.print("\t Hour : ");
  Serial.print(t.hour);
  Serial.print(":");
  Serial.print(t.min);
  Serial.print(".");
  Serial.println(t.sec);
  Serial.print(F("Temperature = "));
  Serial.print(bmp.readTemperature());
  Serial.println(" *C");
  Serial.print(F("Pressure = "));
  Serial.print(bmp.readPressure());
  Serial.println(" Pa");
  Serial.print(F("Approx altitude = "));
  Serial.print(bmp.readAltitude(1013.25)); /* Adjusted to local forecast! */
  Serial.println(" m");
 
  delay(1000);
}

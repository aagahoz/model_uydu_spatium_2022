#include "LoRa_E32.h"
#include <SoftwareSerial.h>

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

void setup() {
  Serial.begin(9600);
  //Serial2.begin(); 
  e32ttl.begin();
  delay(500);
}

void loop() {
  while (e32ttl.available()  > 1) {
    ResponseStructContainer rsc = e32ttl.receiveMessage(sizeof(Signal));
    data = *(Signal*) rsc.data;
    rsc.close();
    Serial.print(F("gelen gps latitude bilgisi: "));
    Serial.println(data.tas_latitude);
    Serial.print(F("gelen gps longtitude bilgisi: "));
    Serial.println(data.tas_longtitude);
    Serial.print(F("gelen gps yükseklik bilgisi: "));
    Serial.println(data.tas_gps_altitude);

    Serial.print(F("gelen basınç bilgisi: "));
    Serial.println(data.tas_pressure);
    Serial.print(F("gelen yükseklik bilgisi: "));
    Serial.println(data.tas_altitude);
  }
}

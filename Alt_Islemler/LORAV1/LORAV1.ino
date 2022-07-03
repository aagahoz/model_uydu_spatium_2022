/*
 *  BMP280 Okuma        +
 *  GPS Okuma           +
 *  LoRa Haberlesme     +
 */

/*  
 LoRa UART Pinout
    -----
 
 BMP280 PinOut   
    Vcc         ->   3.3V
    Ground      ->   Ground
    SDA         ->   A4     
    SCL         ->   A5
    
 GPS PinOut  
    Vcc         ->   3.3V
    Ground      ->   Ground
    Tx          ->   D4     
    Rx          ->   D3

Lora PinOut
    Vcc         ->   5V
    Ground      ->   Ground
    Tx          ->   D10     
    Rx          ->   D11
    AUX         ->   Ground
    M0          ->   Ground     
    M1          ->   Ground
    
 */
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include "LoRa_E32.h"

#include <Wire.h>
#include <Adafruit_BMP280.h>

static const int RXPin = 3, TXPin = 4;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial portgps(RXPin, TXPin);
SoftwareSerial portlora(10, 11);

LoRa_E32 e32ttl(&portlora);

Adafruit_BMP280 bmp;


typedef  struct 
{
  float tas_latitude = 1234.23;
  float tas_longtitude = 4323.44;
  float tas_gps_altitude = 1050.2;
  float tas_pressure = 1023.4;
  float tas_altitude = 1200;
} Signal;

Signal data;

void setup()
{
  Serial.begin(9600);
  
  unsigned status;
  status = bmp.begin(0x76);
  portgps.begin(GPSBaud);
  delay(500);
  e32ttl.begin();
  delay(500);
}

void loop()
{
  portgps.listen(); 
  if (gps.altitude.isValid()) {
    Serial.print("GPS Latitude: ");
    Serial.println(gps.location.lat(), 6);
    data.tas_latitude = gps.location.lat();
    Serial.print("GPS Longtitude: ");
    Serial.println(gps.location.lng(), 6);
    data.tas_longtitude = gps.location.lng();
    Serial.print("GPS Altitude: ");
    Serial.println(gps.altitude.meters(),2);
    data.tas_gps_altitude = gps.altitude.meters();
    
  }
  else
    Serial.println( "error");

  smartDelay(1000);

  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));

  Serial.println();
  
    Serial.print(F("Pressure = "));
    Serial.print(bmp.readPressure());
    Serial.println(" Pa");
    data.tas_pressure = bmp.readPressure();
    Serial.print(F("Approx altitude = "));
    Serial.print(bmp.readAltitude(1013.25)); /* Adjusted to local forecast! */
    Serial.println(" m");
    data.tas_altitude = bmp.readAltitude(1013.25);

    Serial.println();

  portlora.listen(); 

  ResponseStatus rs = e32ttl.sendFixedMessage(0, 44, 23, &data, sizeof(Signal));
  Serial.println(rs.getResponseDescription());

}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (portgps.available())
      gps.encode(portgps.read());
  } while (millis() - start < ms);
}

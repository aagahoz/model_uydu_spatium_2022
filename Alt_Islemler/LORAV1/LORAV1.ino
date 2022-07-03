#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include "LoRa_E32.h"

static const int RXPin = 3, TXPin = 4;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial portgps(RXPin, TXPin);
SoftwareSerial portlora(10, 11);

LoRa_E32 e32ttl(&portlora);

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
  portgps.begin(GPSBaud);
  delay(500);
  e32ttl.begin();
  delay(500);
}

void loop()
{


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

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

typedef  struct {
  int irtifa=20;
  byte pitch;
  byte roll;
  byte yaw;
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
  portgps.listen(); 
  if (gps.altitude.isValid()) {
    Serial.print("GPS Roket irtifa: ");
    Serial.println(19);
    data.irtifa = 19;
  }
  else
    Serial.println( "error");

  smartDelay(1000);

  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));

  Serial.println();

  portlora.listen(); 

  ResponseStatus rs = e32ttl.sendFixedMessage(0, 63, 23, &data, sizeof(Signal));
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

/*
 *  BMP280 Okuma        +
 *  GPS Okuma           +
 *  LoRa Haberlesme     -
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
 */

#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

#include <Wire.h>
#include <Adafruit_BMP280.h>

static const int RXPin = 3, TXPin = 4;
static const uint32_t GPSBaud = 9600;

TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

Adafruit_BMP280 bmp;

String bmp_payload = "";
String gps_payload = "";
String main_payload = "";

// <101482.97,33.32,22.16,29.365244,40.810955,17.30>

void setup()
{
  Serial.begin(9600);

  unsigned status;
  status = bmp.begin(0x76);

  ss.begin(GPSBaud);
}

void loop()
{
    bmp_payload = bmp_returner();
    gps_payload = gps_returner();
  
    main_payload = bmp_payload + "," + gps_payload;
    main_payload = "<" + main_payload + ">";
    
    Serial.println(main_payload);
    
  //  String MyMessage = "<101482.97,33.32,22.16,29.365244,40.810955,17.30,,,,,,,,,,,,>";
    
    main_payload = "";
    bmp_payload = "";
    gps_payload = "";

  
  //delay(1000);
  smartDelay(1000);
  Serial.flush();
  
}

//////////////////////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////////////////////

String gps_returner()
{
  String payload = "";
  String latitude = "";
  String langtitude = "";
  String altitude =  "";
  
  latitude = String(gps.location.lat(),6);
  langtitude = String(gps.location.lng(),6);
  altitude = String(gps.altitude.meters(),2);

  payload = langtitude + "," + latitude + "," + altitude;
//  Serial.println(payload);
  
//  smartDelay(500);

  return payload;
}

//////////////////////////////////////////////////////////////////////////////////////////

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

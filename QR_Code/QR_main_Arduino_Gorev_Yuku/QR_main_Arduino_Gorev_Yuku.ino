/*
 *  LoRa Receiver       -
 *  BMP280              +
 *  MPU9250             +
 *  ESP32 UART          -

   LoRa UART Pinout
    -----
      
   BMP280 PinOut   
    Vcc         ->   3.3V
    Ground      ->   Ground
    SDA         ->   A4     
    SCL         ->   A5

   MPU9250 PinOut   
    Vcc         ->   3.3V
    Ground      ->   Ground
    SDA        ->   D11     
    SCL        ->   D13
    AD0        ->   D12     
    nCS        ->   10
    
   ESP32 UART PinOut
    Vcc         ->   5V
    Ground      ->   Ground
    RX          ->   RX     
    TX          ->   TX
 
*/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <MPU9250.h>

#define SPI_CLOCK 8000000  // 8MHz clock works.
#define SS_PIN   10 
#define INT_PIN  3

Adafruit_BMP280 bmp; 
MPU9250 mpu(SPI_CLOCK, SS_PIN);

String bmp_payload = "";
String mpu_payload = "";
String main_payload = "";

void setup()
{
  unsigned status;
  status = bmp.begin(0x76);

  SPI.begin();
  mpu.init(true);
  uint8_t wai_AK8963 = mpu.AK8963_whoami();
  mpu.calib_acc();
  mpu.calib_mag();

}

void loop() 
{
    bmp_payload = bmp_returner();
    mpu_payload = mpu_returner();
    
    main_payload = bmp_payload + "," + mpu_payload;

    Serial.println(main_payload);


    main_payload = "";
    Serial.flush();
}

String bmp_returner()
{
    String payload = "";
    
    payload = String(bmp.readTemperature()) + "," + String(bmp.readPressure()) + "," + String(bmp.readAltitude(1017.5));
    
//    Serial.println(bmp_payload);
    return payload;
}

String mpu_returner()
{
    mpu.read_all();
    
    String payload = "";
    
    payload = String(mpu.gyro_data[0]) + "," + String(mpu.gyro_data[1]) + "," + String(mpu.gyro_data[2]) + "," + String(mpu.accel_data[0]) + "," + String(mpu.accel_data[1]) + "," + String(mpu.accel_data[2]);
    
//    Serial.println(bmp_payload);
    return payload;
}

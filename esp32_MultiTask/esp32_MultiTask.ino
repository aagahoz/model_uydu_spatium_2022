/*          TASK DURUMLARI
 *  Telemetri gonderimi       +
 *  Manuel Tahrik             +
 *  Dosya aktarımı            -
 *  Manuel Ayrilma            +
 *  Analog Pil okuma          -
 *  RTC okuma                 +
 *  BMP280 okuma              +
 *  SD Card telemetri yazma   +
 *  Raspberry pi UART         +
 *  LoRa UART                 +
 
 SD CARD PinOut   
    Vcc         ->   5V
    Ground      ->   Ground
    MOSI        ->   23     
    MISO        ->   19
    CS          ->   5     
    CLK         ->   18
    
 RTC PinOut
    Vcc         ->   5V
    Ground      ->   Ground
    SDA         ->   21     
    SCL         ->   22

 BMP280 PinOut
    Vcc         ->   5V
    Ground      ->   Ground
    SDA         ->   21     
    SCL         ->   22
    
 Analog Pil Gerilimi PinOut
    Analog Pin  ->  34
    GND         ->  Ground
  
 Servo1 PinOut
    Vcc         ->   Regulator +
    Ground      ->   Ground
    PWM Sinyal  ->   12
    
 Servo2 PinOut
    Vcc         ->   Regulator +
    Ground      ->   Ground
    PWM Sinyal  ->   13
  
 Raspberry Pi PinOut
    Ground      ->  Ground
    TX          ->  RX
    RX          ->  TX

 LoRa PinOut
    5V          ->  5V
    M0          ->  Ground
    M1          ->  Ground
    AUX         ->  Ground
    Ground      ->  Ground
    TX          ->  16
    RX          ->  17

 Fırçasız Motor1 PinOut
    Ground      ->   Ground
    PWM Sinyal  ->   25
  
 Fırçasız Motor2 PinOut
    Ground      ->   Ground
    PWM Sinyal  ->   26
  
 Fırçasız Motor3 PinOut
    Ground      ->   Ground
    PWM Sinyal  ->   27
 */
 
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

#include <WiFi.h>
#include <WiFiUdp.h>
WiFiUDP udp;
#define WIFI_NETWORK "SPATIUM"
#define WIFI_PASSWORD "team.spatium"
const char * udpAddress = "192.168.31.132";
const int udpPort = 44444;
char gelen_komut[5]; // 4 bitlik komut arrayi 0000 ( manuel Servo - motor tahrik - bos - bos )
char komut_durumu[5];
bool servolar_acik_mi = true;

bool isWifiConnected = false;
bool telemetri_gonderilme_durum = false;
#define WIFI_TIMEOUT_MS 2000 // 20 second WiFi connection timeout
#define WIFI_RECOVER_TIME_MS 3000 // Wait 30 seconds after a failed connection attempt

#include <HardwareSerial.h>
HardwareSerial SerialPort(1);  //if using UART1
const byte NumChars = 85;
char ReceivedChars[NumChars];
char TempChars[NumChars];   
char MessageFromPC[NumChars] = {0};
boolean NewData = false;
typedef struct
{
    double enlem = 0;
    double boylam = 0;
    double yukseklik = 0;
    double hiz = 0;
    double pitch = 0;
    double roll = 0;
    double yaw = 0;
}Typedef_RasPiData;
Typedef_RasPiData Raspi;

#include <Wire.h>
#include <Adafruit_BMP280.h>
#include "RTClib.h"

#include "LoRa_E32.h"
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

// Libraries for SD card
#include "FS.h"
#include "SD.h"
#include <SPI.h>
#define SD_CS 5

#include <Servo.h>

Adafruit_BMP280 BMP280;
RTC_DS1307 RTC;
Servo servo1;  
Servo servo2;

byte servoPin1 = 25;
byte servoPin2 = 26;
byte servoPin3 = 27;
Servo esc1;
Servo esc2;
Servo esc3;
bool manuel_tahrik_aktif = false;

void TaskRTC( void *pvParameters );
void TaskBMP280( void *pvParameters );
void TaskLoRa( void *pvParameters );
void TaskBatteryVoltage( void *pvParameters );
void TaskRaspberryPiUART(void *pvParameters);
void TaskServoControl(void *pvParameters);
void keepWiFiAlive(void * parameter);
void TaskTelemetryCommunication(void *pvParameters);
void TaskFileTransfer(void *pvParameters);
void TaskTelemeryLoggerSdCard(void *pvParameters);
void TaskMotorControl(void *pvParameters);
void TaskKomutReceive(void *pvParameters); 
void TaskServoMotorControl(void *pvParameters);
void TaskMotorControl(void *pvParameters);

bool rtc_find_state = false;
bool rtc_running_state = false;

bool servolari_ac = true;
////////////////////////////////////////
const String p1_takimNo = "389590";
String p2_paketNumarasi = "0";
String p3_gondermeSaati = ",,,,,";
String p4_basinc1 = "";
String p5_basinc2 = "";
String p6_yukseklik1 = "";
String p7_yukseklik2 = "";
String p8_irtifaFarki = "";
String p9_inisHizi = "";
String p10_sicaklik = "";
String p11_pilGerilimi = "";
String p12_gps1Latitude = "";
String p13_gps2Latitude = "";
String p14_gps1Altitude = "";
String p15_gps2Altitude = "";
String p16_gps1Longtitude = "";
String p17_gps2Longtitude = "";
String p18_uyduStatusu = "";
String p19_pitch = "";
String p20_roll = "";
String p21_yaw = "";
String p22_donusSayisi = "00";
String p23_videoAktarimBilgisi = "HAYIR";
////////////////////////////////////////
String main_payload = "";

void setup() {
  
  Serial.begin(115200);
  SerialPort.begin(9600, SERIAL_8N1, 4, 2);
//  WiFi.begin(ssid, pwd);

  servo1.attach(12);
  servo2.attach(13);

  esc1.attach(servoPin1);
  esc2.attach(servoPin2);
  esc3.attach(servoPin3);
  delay(3000); 
  
  unsigned status;
  status = BMP280.begin(0x76);
  
  e32ttl.begin();
  delay(500);
  
  rtc_find_state = RTC.begin();
  if (!rtc_find_state) 
  {
    Serial.println("Couldn't find RTC");
  }

  rtc_running_state = RTC.isrunning();
  if (!rtc_running_state)
  {
    Serial.println("RTC is NOT running!");
  }

// Initialize SD card
  SD.begin(SD_CS);  
  if(!SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("ERROR - SD card initialization failed!");
    return;    // init failed
  }
  // If the data.txt file doesn't exist
  // Create a file on the SD card and write the data labels
  File file = SD.open("/data.txt");
  if(!file) {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/data.txt", "DATALAR \r\n");
  }
  else {
    Serial.println("File already exists");  
  }
  file.close();
  
  
  // Now set up two tasks to run independently.
  xTaskCreatePinnedToCore(
    TaskRTC
    ,  "TaskRTC"   // A name just for humans
    ,  4096  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    TaskBMP280
    ,  "TaskBMP280"
    ,  4096  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    TaskLoRa
    ,  "TaskLoRa"
    ,  4096  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    TaskBatteryVoltage
    ,  "TaskBatteryVoltage"
    ,  4096  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);
  
  xTaskCreatePinnedToCore(
    TaskRaspberryPiUART
    ,  "TaskRaspberryPiUART"
    ,  4096  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    TaskKomutReceive
    ,  "TaskKomutReceive"
    ,  4096  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);


  xTaskCreatePinnedToCore(
      keepWiFiAlive,
      "keepWiFiAlive",  // Task name
      5000,             // Stack size (bytes)
      NULL,             // Parameter
      2,                // Task priority
      NULL,             // Task handle
      ARDUINO_RUNNING_CORE
    );
  
  xTaskCreatePinnedToCore(
    TaskTelemetryCommunication
    ,  "TaskTelemetryCommunication"
    ,  4096  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    TaskTelemeryLoggerSdCard
    ,  "TaskTelemeryLoggerSdCard"
    ,  4096  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);
  
  xTaskCreatePinnedToCore(
    TaskFileTransfer
    ,  "TaskFileTransfer"
    ,  4096  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    TaskServoMotorControl
    ,  "TaskServoMotorControl"
    ,  4096  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    TaskMotorControl
    ,  "TaskMotorControl"
    ,  4096  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);
}

void loop(){}

void TaskRTC(void *pvParameters)
{
  (void) pvParameters;

  for (;;) 
  {    
    rtc_find_state = RTC.begin();
    rtc_running_state = RTC.isrunning();
    if (rtc_find_state && rtc_running_state)
    {
      DateTime now = RTC.now();
      p3_gondermeSaati = String(now.day()) + "," + String(now.month()) + "," + String(now.year()) + ";" + String(now.hour()) + "," + String(now.minute()) + "," + String(now.second());
      Serial.println("RTC--> " + p3_gondermeSaati);
    }
    else
    {
      Serial.println("RTC ERROR!");
      p3_gondermeSaati = ",,;,,";
    }
    vTaskDelay(800); 
  }
}

void TaskBMP280(void *pvParameters) 
{
  (void) pvParameters;

  for (;;)
  {
    p10_sicaklik = String(BMP280.readTemperature());
    p4_basinc1 = String(BMP280.readPressure());
    p6_yukseklik1 = String(BMP280.readAltitude(1017.5));
    Serial.println("BMP280--> " + p4_basinc1 + "," + p10_sicaklik + "," + p6_yukseklik1);
    vTaskDelay(800);  
  }
}

void TaskLoRa(void *pvParameters) 
{
  (void) pvParameters;

  for (;;)
  {
     while (e32ttl.available()  > 1) 
     {
        ResponseStructContainer rsc = e32ttl.receiveMessage(sizeof(Signal));
        data = *(Signal*) rsc.data;
        rsc.close();
        p5_basinc2 = data.tas_pressure;
        p7_yukseklik2 = data.tas_altitude;
        p13_gps2Latitude = data.tas_latitude;
        p15_gps2Altitude = data.tas_gps_altitude;
        p17_gps2Longtitude  = data.tas_longtitude;
        Serial.println("LoRa--> " + p5_basinc2 + "," + p7_yukseklik2 + "," + p13_gps2Latitude + "," + p15_gps2Altitude + "," + p17_gps2Longtitude);
    }
    vTaskDelay(800);  
  }
}

void TaskBatteryVoltage(void *pvParameters) 
{
  (void) pvParameters;

  for (;;)
  {

    vTaskDelay(800);  
  }
}

void TaskRaspberryPiUART(void *pvParameters) 
{
  (void) pvParameters;

  for (;;)
  {
    static boolean RecvInProgress = false;
    static byte ndx = 0;      // index
    char StartMarker = '<';
    char EndMarker = '>';
    char rc;          // received data

    while (SerialPort.available() > 0 && NewData == false)
    {
        rc = SerialPort.read();               // test for received data

        if (RecvInProgress == true)
        {         // found some!!
            if (rc != EndMarker)          // <> end marker
            {
                ReceivedChars[ndx] = rc;  // 1st array position=data
                ndx++;                    // next index 
                if (ndx >= NumChars)      // if index>= number of chars
                { 
                    ndx = NumChars - 1;   // index -1
                }
            }
            else                          // end marker found
            {
                ReceivedChars[ndx] = '\0'; // terminate the string  
                RecvInProgress = false;
                ndx = 0;                  // reset index
                NewData = true;           // new data received flag
            }
        }

        else if (rc == StartMarker)       // signal start of new data
        {
        RecvInProgress = true;
        }
    }
    if (NewData == true)                  // input received
    {
        strcpy(TempChars, ReceivedChars); // this temporary copy is necessary to protect the original data
                                          // because strtok() used in parseData() replaces the commas with \0
        
        char * StrTokIndx;                    // this is used by strtok() as an index
    
        StrTokIndx = strtok(TempChars,",");   // get the first control word
        Raspi.enlem = atof(StrTokIndx);    // convert this part to the first integer
    
        StrTokIndx = strtok(NULL, ",");       // this continues after 2nd ',' in the previous call
        Raspi.boylam = atof(StrTokIndx);         // convert this part to the first integer
    
        StrTokIndx = strtok(NULL, ",");
        Raspi.yukseklik = atof(StrTokIndx);    // last integer
            
        StrTokIndx = strtok(NULL, ",");       // this continues after 1st ',' in the previous call
        Raspi.hiz = atof(StrTokIndx);    // convert this part to the first integer
    
        StrTokIndx = strtok(NULL, ",");       // this continues after 2nd ',' in the previous call
        Raspi.pitch = atof(StrTokIndx);         // convert this part to the first integer
    
        StrTokIndx = strtok(NULL, ",");
        Raspi.roll = atof(StrTokIndx);    // last integer
    
        StrTokIndx = strtok(NULL, ",");
        Raspi.yaw = atof(StrTokIndx);    // last integer
        
        p9_inisHizi = Raspi.hiz;
        p12_gps1Latitude = Raspi.enlem;
        p14_gps1Altitude = Raspi.yukseklik;
        p16_gps1Longtitude = Raspi.boylam;
        p19_pitch = Raspi.pitch;
        p20_roll = Raspi.roll;
        p21_yaw = Raspi.yaw;   
        
        NewData = false;                  // reset new data
    }
    Serial.print("Raspi--> ");
    Serial.println(ReceivedChars);
    
    vTaskDelay(800);  
  }
}

void TaskKomutReceive(void *pvParameters) 
{
  (void) pvParameters;

  for (;;)
  {
    // Yer Istasyonundan komut alma 
    udp.parsePacket();  // gelen datayi okunabilir hale getirir, read fonks dan once kullanilmali
    if(udp.read(gelen_komut, 4) > 0)  // esp32 ye gelen datalari okur
    {
      Serial.print("Server to client ( Yer istasyonundan esp32`ye ) : ");
      Serial.println(gelen_komut);  // alinan komutlar bastirilir
      strcpy(komut_durumu, gelen_komut);
    }
    
    if (komut_durumu[0] == '0')  // servo komut durumu
    {
      servolar_acik_mi = false;
    }
    else if (komut_durumu[0] == '1')
    {
      servolar_acik_mi = true;
    }

    if (komut_durumu[1] == '0')  // servo komut durumu
    {
      manuel_tahrik_aktif = false;
    }
    else if (komut_durumu[1] == '1')
    {
      manuel_tahrik_aktif = true;
    }
    
    Serial.print("Komut Durumu: ");
    Serial.println(komut_durumu);
    vTaskDelay(800);  
  }
}

void TaskServoMotorControl(void *pvParameters) 
{
  (void) pvParameters;

  for (;;)
  {
    if (servolar_acik_mi == true)
    {
      servo1.write(0);
      servo2.write(0);
    }
  
    else if (servolar_acik_mi == false)
    {
      servo1.write(90);
      servo2.write(90);
    }
    vTaskDelay(800);  
  }
}

void TaskMotorControl(void *pvParameters) 
{
  (void) pvParameters;

  for (;;)
  {
    if (manuel_tahrik_aktif == true)
    {
      esc1.writeMicroseconds(1500); // Send signal to ESC.
      esc2.writeMicroseconds(1500); // Send signal to ESC.
      esc3.writeMicroseconds(1500); // Send signal to ESC.
      vTaskDelay(8000);
      manuel_tahrik_aktif = false;
      komut_durumu[1] = '0';
    }

    esc1.writeMicroseconds(1000); // Send signal to ESC.
    esc2.writeMicroseconds(1000); // Send signal to ESC.
    esc3.writeMicroseconds(1000); // Send signal to ESC.
  
    vTaskDelay(800);  
  }
}

void keepWiFiAlive(void * parameter){
    for(;;){
        if(WiFi.status() == WL_CONNECTED){
            isWifiConnected = true;
            vTaskDelay(3000 / portTICK_PERIOD_MS);
            continue;
        }

        Serial.println("[WIFI] Connecting");
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);

        unsigned long startAttemptTime = millis();

        // Keep looping while we're not connected and haven't reached the timeout
        while (WiFi.status() != WL_CONNECTED && 
                millis() - startAttemptTime < WIFI_TIMEOUT_MS){}

        // When we couldn't make a WiFi connection (or the timeout expired)
      // sleep for a while and then retry.
        if(WiFi.status() != WL_CONNECTED){
            Serial.println("[WIFI] FAILED");
            vTaskDelay(WIFI_RECOVER_TIME_MS / portTICK_PERIOD_MS);
        continue;
        }

        Serial.print("Wifi Connected IP: ");
        Serial.println(WiFi.localIP());
    }
}

void TaskTelemetryCommunication(void *pvParameters) 
{
  (void) pvParameters;

  for (;;)
  {
    if (isWifiConnected == true)
      {
        Serial.println("Wifi is Alive");

        // Telemetri paketi hazirlama
        char udp_payload[250];
       // String datalar = "2929,1,14:30,5,6,15,20,10,5,28,98,40.806298,29.355541,258,40.806298,29.355541,2564,1,10,20,5,5,EVET";        main_payload.toCharArray(udp_payload, 250);
        main_payload.toCharArray(udp_payload, 250);
        int sizePayload = 0;
        for (int i=0; udp_payload[i] != '\0'; i++) // olusan telemetri dizisinin boyutunu hesaplar
        {
          sizePayload += 1;
        }
      
        // Yer Istasyonuna veri gonderme
        udp.beginPacket(udpAddress, udpPort);  // WiFiUDP.beginPacket(hostIp, port);
        udp.write((uint8_t *)udp_payload, sizePayload);  // WiFiUDP.write(buffer, size);
        Serial.print("Payload: ");
        Serial.println(sizePayload);
        telemetri_gonderilme_durum = udp.endPacket(); // paketi gonderir, basarili ise 1, basarisiz ise 1 dondurur
        if (udp.endPacket() == 1)
          Serial.println("--> UDP Paket Gonderildi");
        else
          Serial.println("--> UDP Paket Alinmadi");
        udp.flush(); // istemciye yazilmis ancak okunmamis datalari siler
        memset(udp_payload, 0, 120);
      }
      else
      {
        Serial.println("Wifi is NOT Alive");
      }
      vTaskDelay(800);  
  }
}

void TaskFileTransfer(void *pvParameters) 
{
  (void) pvParameters;

  for (;;)
  {

    vTaskDelay(800);  
  }
}

void TaskTelemeryLoggerSdCard(void *pvParameters) 
{
  (void) pvParameters;

  for (;;)
  {
    main_payload = p1_takimNo + "," + p2_paketNumarasi
    + "," + p3_gondermeSaati + "," + p4_basinc1 + "," + p5_basinc2
    + "," + p6_yukseklik1 + "," + p7_yukseklik2 + "," + p8_irtifaFarki
    + "," + p9_inisHizi + "," + p10_sicaklik + "," + p11_pilGerilimi
    + "," + p12_gps1Latitude + "," + p13_gps2Latitude
    + "," + p14_gps1Altitude + "," + p15_gps2Altitude
    + "," + p16_gps1Longtitude + "," + p17_gps2Longtitude
    + "," + p18_uyduStatusu + "," + p19_pitch + "," + p20_roll
    + "," + p21_yaw + "," + p22_donusSayisi + "," + p23_videoAktarimBilgisi + "\r\n";
    
    appendFile(SD, "/data.txt", main_payload.c_str());
        
    Serial.print("main_payload---> ");
    Serial.println(main_payload);

    vTaskDelay(800);  
  }
}


void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

// Write to the SD card (DON'T MODIFY THIS FUNCTION)
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

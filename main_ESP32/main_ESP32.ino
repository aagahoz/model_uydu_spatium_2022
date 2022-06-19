#include <WiFi.h>
#include <WiFiUdp.h>
#include <Servo.h>

Servo myservo1;  
Servo myservo2;  
//const char * ssid = "AC-ESP32";
//const char * pwd = "987654321";

const char * ssid = "SPATIUM";
const char * pwd = "team.spatium";

//const char * udpAddress = "192.168.4.24";

const char * udpAddress = "192.168.31.132";
const int udpPort = 44444;
char incoming[2];
char payload[120];

bool servolar_acik_mi = true;

WiFiUDP udp;

void setup()
{
  Serial.begin(115200);
  WiFi.begin(ssid, pwd);
  myservo1.attach(12);
  myservo2.attach(13);  
}

void loop()
{
  char payload[250];
  String datalar = "2929,1,14:30,5,6,15,20,10,5,28,98,40.806298,29.355541,258,40.806298,29.355541,2564,1,10,20,5,5,EVET";
  datalar.toCharArray(payload, 250);
  int sizePayload = 0;
  for (int i=0; payload[i] != '\0'; i++)
  {
    sizePayload += 1;
  }

  // GCS ye veri gonderme
  udp.beginPacket(udpAddress, udpPort);
  udp.write((uint8_t *)payload, sizePayload);
  Serial.print("Payload: ");
  Serial.println(sizePayload);
  udp.endPacket();
  
  // veri alma GCS dan
  udp.parsePacket();
  if(udp.read(incoming, 4) > 0)
  {
    Serial.print("Server to client: ");
    Serial.println(incoming);
    if (incoming[0] == '0')
    {
      Serial.println("Basarili_0");
      servolar_acik_mi = false;
    }
    if (incoming[0] == '1')
    {
      Serial.println("Basarili_1");
      servolar_acik_mi = true;
    }
  }
  udp.flush();

  servo_kontrolu(servolar_acik_mi, &myservo1, &myservo2);

  memset(payload, 0, 120);
  delay(1000);
}

void servo_kontrolu(bool durum, Servo *servo1, Servo *servo2)
{
  if (durum == true)
  {
    servo1->write(0);
    servo2->write(0);
  }

  else if (durum == false)
  {
    servo1->write(90);
    servo2->write(90);
  }
}

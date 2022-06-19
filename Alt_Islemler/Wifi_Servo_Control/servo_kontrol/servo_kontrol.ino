#include <Servo.h>

Servo myservo1;  
Servo myservo2;  

bool servolar_acik_mi = true;

void setup() 
{
  myservo1.attach(12);
  myservo2.attach(13);  
}

void loop() 
{
  servo_kontrolu(servolar_acik_mi, &myservo1, &myservo2);

  servolar_acik_mi = !servolar_acik_mi;
  delay(3000);
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


// https://www.arduino.cc/reference/en/libraries/wifi/
// https://github.com/arduino-libraries/WiFi
// https://github.com/RoboticsBrno/ServoESP32

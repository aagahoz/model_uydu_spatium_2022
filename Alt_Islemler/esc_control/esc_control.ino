#include <Servo.h>

Servo esc1; //Creating a servo class with name as esc
Servo esc2; //Creating a servo class with name as esc
Servo esc3; //Creating a servo class with name as esc

void setup()

{
  Serial.begin(9600);

//  esc1.attach(13); //Specify the esc signal pin,Here as D8
//  esc1.attach(26); //Specify the esc signal pin,Here as D8
  esc1.attach(25); //Specify the esc signal pin,Here as D8
  esc2.attach(26); //Specify the esc signal pin,Here as D8
  esc3.attach(27); //Specify the esc signal pin,Here as D8

//    for (int i=0; i<650; i++)
//    {
//      esc1.writeMicroseconds(1000); //initialize the signal to 1000
//      Serial.print(i);
//      Serial.println("  arm");
//    }

}



void loop()

{
  
  int val; //Creating a variable val
  
  //val= analogRead(34); //Read input from analog pin a0 and store in val

  val = 0;
  
  val= map(val, 0, 4095,1000,2000); //mapping val to minimum and maximum(Change if needed)
  
  esc1.writeMicroseconds(val); //using val as the signal to esc
  esc2.writeMicroseconds(val); //using val as the signal to esc
  esc3.writeMicroseconds(val); //using val as the signal to esc
  delay(3000);

  val = 1600;

  val= map(val, 0, 4095,1000,2000); //mapping val to minimum and maximum(Change if needed)
  
  esc1.writeMicroseconds(val); //using val as the signal to esc
  esc2.writeMicroseconds(val); //using val as the signal to esc
  esc3.writeMicroseconds(val); //using val as the signal to esc
  delay(3000);

  Serial.println(val);
}

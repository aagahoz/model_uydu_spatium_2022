float vin=0.0;   //battery voltage
float vout=0.0;    //A0 pin reading voltage
float r1=11880.0;    //r1 value
float r2=10000.0;      //r2 value

void setup()
{  
   analogReadResolution(12);
   Serial.begin(9600);     //  opens serial port, sets data rate to 9600 bps
}

void loop()
{
  float vtoplam= 0.0;
  int count=0;
  
  for(int i=0; i<75; i++)
    {
      float analogvalue = analogRead(34);      //reading value from analog pin
      vout = analogvalue * 2.373 / 4096 ;           //analog value to voltage (it is pin's voltage)
      vin = (((r1+r2)* vout)/r2);
      //Serial.println(vout);
      if(vin != 0.00)
      {
        vtoplam = vtoplam + vin;
        count = count+1 ;
      }
      delay(10);
    }
  float voltage = 2.32 * (vtoplam/count);
  Serial.print("Voltage= ");        
  Serial.print(voltage);
  Serial.println(" Volts");
  delay(250);
}

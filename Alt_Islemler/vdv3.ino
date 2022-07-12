float vin=0.0;   //battery voltage
float vout=0.0;    //A0 pin reading voltage
float r1=47200.0;    //r1 value
float r2=66700.0;      //r2 value

void setup()
{
   Serial.begin(9600);     //  opens serial port, sets data rate to 9600 bps
}

void loop()
{
  float vtoplam= 0.0;
  int count=1;
  
  for(int i=0; i<30; i++)
    {
      float analogvalue = analogRead(A0);      //reading value from analog pin
      vout = analogvalue * 5 / 1024 ;           //analog value to voltage (it is pin's voltage)
      vin = (((r1+r2)* vout)/r2);
      //Serial.println(vin);
      if(vin != 0.00)
      {
        vtoplam = vtoplam + vin;
        count = count+1 ;
      }
      delay(25);
    }
  float voltage = vtoplam/(count-1);
  Serial.print("Voltage= ");        
  Serial.print(voltage);
  Serial.println(" Volts");
  delay(250);
}

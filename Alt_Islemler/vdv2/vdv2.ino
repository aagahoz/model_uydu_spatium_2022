float voltage=0.0;  //battery voltage
float r1=47000.0;    //r1 value
float r2=68000.0;      //r2 value

void setup()
{
   Serial.begin(9600);     //  opens serial port, sets data rate to 9600 bps
}

void loop()
{
    float analogvalue = analogRead(A0); //reading value from analog pin
    Serial.println(analogvalue*(5.0/1023.0)); 
    float analogtovoltage = analogvalue*(5.0/1023.0); //analog value to voltage (it is pins voltage)
    voltage= (((r1+r2)*analogtovoltage)/r2);
    Serial.print("Voltage= ");                 // prints the voltage value in the serial monitor
    Serial.print(voltage);
    Serial.println(" Volts");
    delay(1000);
}

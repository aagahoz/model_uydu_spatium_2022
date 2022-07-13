// https://microcontrollerslab.com/esp32-uart-communication-pins-example/#:~:text=ESP32%20UART%20Pins&text=Each%20of%20them%20exposes%204,to%20the%20SPI%20flash%20memory.

#include <HardwareSerial.h>

HardwareSerial SerialPort(1);  //if using UART1
char uartRaspiData[120];
int i;
char key[7] = "$RASPI";

typedef struct
{  
  char section[120];
  
  char sentenceIdentifier[7];   //1
  double latitude;        //3
  double longtitude;        //5
  double altitude;        //10
  
}Typedef_Raspi;

Typedef_Raspi raspiStruct;

void setup()
{
  SerialPort.begin(15200, SERIAL_8N1, 4, 2); 
}

void loop()
{
  i = 0;
  while (SerialPort.available() && i < 100)
  {
    uartRaspiData[i] = SerialPort.read();
    i++;
  }

  char *p;
  p = strstr(uartRaspiData, key);
  
  int j=0;  
  while (p[j] != '>' && j < 100)
  {
    raspiStruct.section[i] = p[i];
    i++;
  }

  char* token = strtok(raspiStruct.section, ",");
  strcpy(raspiStruct.sentenceIdentifier, token);
  
  token = strtok(NULL, ",");
  raspiStruct.latitude = (double)atof(token);
  
  token = strtok(NULL, ",");
  raspiStruct.longtitude = (double)atof(token);
  
  token = strtok(NULL, ",");
  raspiStruct.altitude = (double)atof(token);

  Serial.print("Ham Data: ");
  Serial.println(uartRaspiData);
  
  Serial.print("Section Data: ");
  Serial.println(raspiStruct.section);

  Serial.print("Sentence Identifier: ");
  Serial.println(raspiStruct.sentenceIdentifier);

  Serial.print("Latitude: ");
  Serial.println(raspiStruct.latitude);

  Serial.print("Longtitude: ");
  Serial.println(raspiStruct.longtitude);

  Serial.print("Altitude: ");
  Serial.println(raspiStruct.altitude);
}

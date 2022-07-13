#include <HardwareSerial.h>
HardwareSerial SerialPort(1);  //if using UART1

const byte NumChars = 85;
char ReceivedChars[NumChars];
char TempChars[NumChars];   
char MessageFromPC[NumChars] = {0};

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

boolean NewData = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  SerialPort.begin(9600, SERIAL_8N1, 4, 2); 
}

void loop() {
    RecvWithStartEndMarkers();            // test for serial input
    Serial.println(ReceivedChars);
    if (NewData == true)                  // input received
    {
        strcpy(TempChars, ReceivedChars); // this temporary copy is necessary to protect the original data
                                          // because strtok() used in parseData() replaces the commas with \0
        ParseData();                      // split the data
        NewData = false;                  // reset new data

        Serial.print("Data1: ");
        Serial.println(Raspi.enlem);

        Serial.print("Data2: ");
        Serial.println(Raspi.boylam);

        Serial.print("Data3: ");
        Serial.println(Raspi.yukseklik);

        Serial.print("Data4: ");
        Serial.println(Raspi.hiz);

        Serial.print("Data5: ");
        Serial.println(Raspi.pitch);

        Serial.print("Data6: ");
        Serial.println(Raspi.roll);

        Serial.print("Data7: ");
        Serial.println(Raspi.yaw);
        
    }
    delay(500);
}

void RecvWithStartEndMarkers()
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
}

void ParseData()                          // split the data into its parts
{                                         // Serial.println(MessageFromPC); ECHO data received
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
}

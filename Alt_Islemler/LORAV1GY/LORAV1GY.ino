#include "LoRa_E32.h"
#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 11); // Arduino RX <-- e32 TX, Arduino TX --> e32 RX
LoRa_E32 e32ttl(&mySerial);

typedef  struct {
  int irtifa;
  byte pitch;
  byte roll;
  byte yaw;
} Signal;

Signal data;



void setup() {
  Serial.begin(9600);
  e32ttl.begin();
  delay(500);
}

void loop() {
  while (e32ttl.available()  > 1) {
    ResponseStructContainer rsc = e32ttl.receiveMessage(sizeof(Signal));
    data = *(Signal*) rsc.data;
    rsc.close();
    Serial.print(F("gelen irtifa bilgisi: "));
    Serial.println(data.irtifa);
  }

}

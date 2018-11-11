#include <SoftwareSerial.h>

#define BAUD_RATE 19200

SoftwareSerial swSer(14, 12, false, 256);

void setup() {
  Serial.begin(BAUD_RATE);
  swSer.begin(BAUD_RATE);
}

void loop() {
  while (swSer.available() > 0) {
    Serial.write(swSer.read());
    yield();
  }
  while (Serial.available() > 0) {
    swSer.write(Serial.read());
    yield();
  }

}

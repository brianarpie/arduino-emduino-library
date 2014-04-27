#include <EMduino.h>
#include <SPI.h>
#include "RF24.h"
#include "Narcoleptic.h"
#include "Volume.h"

EMduino port("receive");

void setup() {
  Serial.begin(115200);
  port.setupRadio();
}
void loop() {
  port.read();
}

#include <Wire.h>
#include "elcan_scale.h"
#define DEBUG 1
#define SCALE_I2C_ADDRESS 0x42
ElcanScale scale(SCALE_I2C_ADDRESS);
bool working = false;
void setup() {
  Wire.begin();
  Serial.begin(115200);
  pinMode(LED_BUILTIN, HIGH);
  if (!scale.begin()){
    Serial.println("#FAILED SCALE!!!");
  } else {
    working = true;
  };
}

void loop() {
  byte temp[7];
  if (working) {
    scale.get_weight(temp);
    Serial.print("#");
    for(byte b=0; b<7; b++) {
      Serial.write(temp[b]);
    }
    Serial.println(" ");
    delay(1200);
  }
}

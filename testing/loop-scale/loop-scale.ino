#include <Wire.h>
#include "scale_i2c.h"
#define DEBUG 1
#define SCALE_I2C_ADDRESS 0x42
ElcanScale scale;
bool working = false;
void setup() {
  Wire.begin();
  Serial.begin(57600);
  pinMode(LED_BUILTIN, HIGH);
  if (!scale.begin(SCALE_I2C_ADDRESS)){
    Serial.println("FAILED SCALE!!!");
  } else {
    working = true;
  };
}

void loop() {
  byte temp[7];
  if (working) {
    scale.get_weight(temp);
    for (byte p = 0; p < 6; p++) {
      Serial.write(temp[p]);
    }
    Serial.println("/");
    delay(1200);
  }
}

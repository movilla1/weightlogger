#include <Wire.h>

#define SCALE_I2C_ADDRESS 0x21
#define MAX_WEIGHT_SIZE 7

char weight[10];
char wireBuffer[10];
char pos;

void setup() {
  Serial.begin(9600); // serial standard for scales
  Wire.begin(SCALE_I2C_ADDRESS); //begin i2c slave
  pinMode(LED_BUILTIN, OUTPUT);
  Wire.onRequest(sendLastReadedWeight);
  memset(weight, 0, sizeof(weight));
  pos = 0;
}

void loop() {
  pos = Serial.readBytesUntil("\n", weight, sizeof(weight));
  weight[pos+1] = 0x00;
  memcpy(wireBuffer, weight, sizeof(wireBuffer));
  memset(weight,0, sizeof(weight));
}

void sendLastReadedWeight() {
  for (char i=0; i < MAX_WEIGHT_SIZE; i++) {
    Wire.write(wireBuffer[i]);
  }
}
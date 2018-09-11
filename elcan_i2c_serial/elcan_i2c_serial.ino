#include <Wire.h>
#include <Arduino.h>

#define SETUP_BTN 3
#define LED 8
#define I2C_ADDR 0x18
#define BUFFER_SIZE 32

char dataBuffer[BUFFER_SIZE];
byte pos;
void setup() {
  Wire.begin(I2C_ADDR);
  pinMode(LED, OUTPUT);
  pinMode(SETUP_BTN, INPUT_PULLUP);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestService);
  Serial.begin(115200); //start the serial interface
  digitalWrite(LED, HIGH);
  pos = 0;
}

void loop() {
  if (Serial.available()) {
    digitalWrite(LED,LOW);
    while(Serial.available() && pos < BUFFER_SIZE) {
      dataBuffer[pos] = Serial.read();
      pos++;
    }
    digitalWrite(LED, HIGH);
  }
}

void receiveEvent(int count) {
  char t;
  while(Wire.available()) {
    t = Wire.read();
    Serial.print(t); 
  }
}

void requestService() {
  byte tmp = 0;
  while(tmp < pos) {
    Wire.write(dataBuffer[tmp]);
    tmp ++;
  }
}

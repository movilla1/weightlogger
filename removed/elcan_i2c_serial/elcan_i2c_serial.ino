#include <Wire.h>
#include <Arduino.h>

#define SETUP_BTN 3
#define LED 8
#define I2C_ADDR 0x18
#define BUFFER_SIZE 256

char dataBuffer[BUFFER_SIZE];
byte writePos;
byte readPos;

void setup() {
  Wire.begin(I2C_ADDR);
  pinMode(LED, OUTPUT);
  pinMode(SETUP_BTN, INPUT_PULLUP);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestService);
  Serial.begin(115200); //start the serial interface
  digitalWrite(LED, HIGH);
  writePos = 0;
  readPos = 0;
}

void loop() {
  if (Serial.available()) {
    digitalWrite(LED,LOW);
    while(Serial.available()) {
      dataBuffer[writePos] = Serial.read();
      writePos++;
      writePos %= BUFFER_SIZE; //circular buffer
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
  while(tmp < 32 && readPos < writePos) {
    Wire.write(dataBuffer[readPos]);
    readPos++;
    tmp++;
  }
  if (readPos >= writePos ) {
    memset(dataBuffer, 0, sizeof(dataBuffer));
    readPos = 0;
  }
}

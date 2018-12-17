/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

/*End of auto generated code by Atmel studio */

#include <Wire.h>
//Beginning of Auto generated function prototypes by Atmel Studio
void checkCommand(int count);
void answerMaster();
void sendLastReadedWeight();
//End of Auto generated function prototypes by Atmel Studio



#define OLDSCALE 1
//#define NEWSCALE 1
#define SCALE_I2C_ADDRESS 0x42
#define MAX_WEIGHT_SIZE 7
#define READY 0
#define SEND_INIT_STR 16
#define SEND_WEIGHT 64
#define LED 8
#define WEIGHT_LENGTH 7

char weight[10];
char wireBuffer[10];
char pos;
byte sysState;

void setup() {
#ifdef NEWSCALE
  Serial.begin(9600); // serial standard for scales
#endif
#ifdef OLDSCALE
  Serial.begin(4800);
#endif
  Wire.begin(SCALE_I2C_ADDRESS); //begin i2c slave
  pinMode(LED_BUILTIN, OUTPUT);
  Wire.onRequest(answerMaster);
  Wire.onReceive(checkCommand);
  memset(weight, 0, sizeof(weight));
  pos = 0;
  sysState = READY;
  digitalWrite(LED, HIGH);
}

void loop() {
  byte tmp;
  switch(sysState) {
    case READY:
#ifdef NEWSCALE
      pos = Serial.readBytesUntil('\n', weight, sizeof(weight));
      weight[pos+1] = 0x00;
      memcpy(wireBuffer, weight, sizeof(wireBuffer));
      memset(weight,0, sizeof(weight));
#endif
#ifdef OLDSCALE
      if (Serial.available() >= 7) {
        for (pos=0; pos < 7; pos++) {
          tmp = Serial.read();
          if (tmp == 0x1C) {
            memcpy(wireBuffer, weight, WEIGHT_LENGTH);
            memset(weight,0, sizeof(weight));
          } else {
            weight[pos] = tmp;
          }
        }
      }
#endif
      break;
  }
}

void checkCommand(int count) {
  byte b;
  b = Wire.read(); //get the first byte only
  switch(b) {
    case 'I':
      sysState = SEND_INIT_STR;
      break;
    case 'W':
      sysState = SEND_WEIGHT;
      break;
  }
  while (Wire.available()) {
    b = Wire.read(); //clear the extra data sent, if any
  }
}

void answerMaster() {
  switch(sysState) {
    case SEND_WEIGHT:
      digitalWrite(LED, LOW);
      sendLastReadedWeight();
      digitalWrite(LED, HIGH);
      sysState = READY;
      break;
    case SEND_INIT_STR:
      Wire.println(F("SCALEK"));
      sysState = READY;
      break;
  }
}

void sendLastReadedWeight() {
  for (char i=0; i < MAX_WEIGHT_SIZE; i++) {
    Wire.write(wireBuffer[i]);
  }
}

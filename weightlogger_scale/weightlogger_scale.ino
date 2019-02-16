#include <Wire.h>

#define SCALE_I2C_ADDRESS 0x42
#define OLD_SCALE = 29
#define GAMMA_SCALE = 60
#define MAX_WEIGHT_SIZE 7
#define READY 0
#define SEND_INIT_STR 16
#define SEND_WEIGHT 64
#define LED 8
#define SETUP_JMP 5
#define WEIGHT_LENGTH 7

char weight[10];
char wireBuffer[10];
char pos;
byte scaleMode;
byte sysState;

void setup() {
  Wire.begin(SCALE_I2C_ADDRESS); //begin i2c slave
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SETUP_JMP, INPUT_PULLUP);
  delay(300); // wait for the input to become stable
  if (digitalRead(SETUP_JMP) == 1) {
    scaleMode = OLD_SCALE;
    Serial.begin(4800);
  } else {
    scaleMode = GAMMA_SCALE;
    Serial.begin(9600); // serial standard for scales
  }
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
      if (scaleMode == GAMMA_SCALE) {
        pos = Serial.readBytesUntil('\n', weight, sizeof(weight));
        weight[pos+1] = 0x00;
        memcpy(wireBuffer, weight, sizeof(wireBuffer));
        memset(weight,0, sizeof(weight));
      } else {
        read_old_scale();
      }
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
      memset(wireBuffer, 0, sizeof(wireBuffer));
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

void read_old_scale() {
  byte tmp;
  if (Serial.available() >= 7) {
    for (pos=0; pos < 7; pos++) {
      tmp = Serial.read();
      if (tmp == 0x1C || tmp == 0x1B) {
        memcpy(wireBuffer, weight, WEIGHT_LENGTH);
        memset(weight, 0, sizeof(weight));
      } else {
        weight[pos] = tmp;
      }
      if (tmp == 0x1C) {
        digitalWrite(LED, HIGH);
      } else {
        digitalWrite(LED, LOW);
      }
    }
  }
}
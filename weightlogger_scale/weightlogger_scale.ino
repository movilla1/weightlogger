#include <Wire.h>

#define SCALE_I2C_ADDRESS 0x21
#define MAX_WEIGHT_SIZE 7
#define READY 0
#define SEND_INIT_STR 16
#define SEND_WEIGHT 64

char weight[10];
char wireBuffer[10];
char pos;
byte sysState;

void setup() {
  Serial.begin(9600); // serial standard for scales
  Wire.begin(SCALE_I2C_ADDRESS); //begin i2c slave
  pinMode(LED_BUILTIN, OUTPUT);
  Wire.onRequest(answerMaster);
  Wire.onReceive(checkCommand);
  memset(weight, 0, sizeof(weight));
  pos = 0;
  sysState = READY;
}

void loop() {
  switch(sysState) {
    case READY:
      pos = Serial.readBytesUntil("\n", weight, sizeof(weight));
      weight[pos+1] = 0x00;
      memcpy(wireBuffer, weight, sizeof(wireBuffer));
      memset(weight,0, sizeof(weight));
      break;
  }
}

void checkCommand(int count) {
  if (count==3) {
    byte b = Wire.read(); //get the first byte only
    switch(b) {
      case 'I':
        sysState = SEND_INIT_STR;
        break;
      case 'W':
        sysState = SEND_WEIGHT;
        break;
    }
  }
}

void answerMaster() {
  switch(sysState) {
    case SEND_WEIGHT:
      sendLastReadedWeight();
      break;
    case SEND_INIT_STR:
      memset(wireBuffer, 0, sizeof(wireBuffer));
      strcat (wireBuffer, "INIOK");
      Wire.println(wireBuffer);
      break;
  }
}

void sendLastReadedWeight() {
  for (char i=0; i < MAX_WEIGHT_SIZE; i++) {
    Wire.write(wireBuffer[i]);
  }
}

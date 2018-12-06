#include <EEPROM.h>

#define BLINK_PERIOD 300

long lastBlink;
bool newval;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  lastBlink = millis();
  newval = false;
}

void loop() {
  if (Serial.available()) {
    process_serial();
  }
  else {
    blink();
  }
}

void blink() {
  if (millis()-lastBlink > BLINK_PERIOD) {
    newval = !newval;
    digitalWrite(LED_BUILTIN, newval);
    lastBlink = millis();
  }
}

void process_serial() {
  byte cmd = Serial.read();
  switch (cmd) {
    case 'd':
      dump_eeprom();
      break;
    case 'e':
      erase_eeprom();
      break;
  }
}

void dump_eeprom() {
  byte readed;
  for (int i=0; i <= 0x3FF; i++) {
    readed = EEPROM.read(i);
    Serial.print(readed, HEX);
    Serial.write('-');
    if (i % 8 == 0 && i != 0) {
      Serial.write('\r');
      Serial.write('\n');
    }
  }
}

void erase_eeprom() {
  for (int i=0; i < 0x400; i++) {
    EEPROM.write(i, 0xFF);
    Serial.write('.');
  }
}

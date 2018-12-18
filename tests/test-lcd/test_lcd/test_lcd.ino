#include "elcan_lcd.h"
#include "definitions.h"

char tmp[64];
ElcanLCDManager lcdm;

void setup() {
  Serial.begin(9600);
  Serial.println("#Start");
  memset(tmp, 0, sizeof(tmp));
  Serial.println("#almost");
  lcdm.init();
  Serial.println("#init");
  lcdm.show_message("Initialized!");
  Serial.println("#Done");
}

void loop() {
  delay(2000);
  Serial.println("#Alive");
  if (Serial.available()) {
    Serial.readBytesUntil('\n', tmp, sizeof(tmp));
  }
  lcdm.show_message(tmp);
  memset(tmp, 0, sizeof(tmp));//*/
  //lcdm.show_error(3);
}

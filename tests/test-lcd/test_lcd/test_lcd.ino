#include "elcan_lcd.h"
#include "definitions.h"

#define WAIT_ENTER while(cha != '\n'){cha = Serial.read();}
char tmp[64];
ElcanLCDManager lcdm;

void setup() {
  Serial.begin(115200);
  Serial.println("#Start");
  memset(tmp, 0, sizeof(tmp));
  Serial.println("#almost");
  lcdm.init();
  Serial.println("#init");
  lcdm.show_message("Initialized!");
  Serial.println("#Done");
}

void loop() {
  char cha;
  char ip[]="192.168.0.1";
  Serial.println("#Alive");
  delay(2000);
  Serial.println("#ready");
  sprintf(tmp,"Wait, this is?");
  lcdm.show_ready(tmp);
  delay(2000);
  Serial.println("#Light");
  lcdm.light_on();
  delay(2000);
  Serial.print("#");
  Serial.println(ip);
  lcdm.show_ip(ip);
  delay(2000);
  Serial.println("#Long message");
  lcdm.show_message("INIT working second round again");
  delay(1400);
  //*/
}

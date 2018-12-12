/**
 * Function to show errors in blinks with the LED
 * shared in the whole project
 */
void show_error(uint8_t error_code) {
  byte blinks;
  lcd.clear();
  switch(error_code) {
    case ERROR_RFID:
      blinks = 2;
      lcd.print(F("Error 502"));
      break;
    case ERROR_RTC:
      blinks = 3;
      lcd.print(F("Error 501"));
    case ERROR_WIFI:
      blinks = 4;
      lcd.print(F("Error 503"));
      break;
    case ERROR_SCALE:
      blinks = 5;
      lcd.print(F("Error 504"));
      break;
    default:
      lcd.print(F("Error 500"));
      blinks = 1;
      break;
  }
  for (byte i=0; i < blinks; i++) {
    tone(BUZZER, 1500, 400);
  }
  delay(500); //wait 1/2 second between displays at least;
}

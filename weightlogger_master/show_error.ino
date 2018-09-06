/**
 * Function to show errors in blinks with the LED
 * shared in the whole project
 */
void show_error(uint8_t error_code) {
  uint8_t blinks;
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
    default:
      lcd.print(F("Error 500"));
      blinks = 1;
      break;
  }
  for (uint8_t i; i < blinks; i++) {
    digitalWrite(BUZZER, 0); //turn off the LED
    delay(300);
    digitalWrite(BUZZER, 1);
    delay(300);
  }
  delay(500); //wait 1/2 second between displays at least;
}

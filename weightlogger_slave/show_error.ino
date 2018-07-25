/**
 * Function to show errors in blinks with the LED
 * shared in the whole project
 */
void show_error(uint8_t error_code) {
  uint8_t blinks;
  switch(error_code) {
    case ERROR_WEIGHT:
      blinks = 2;
      break;
    case ERROR_RFID:
      blinks = 3;
      break;
    case ERROR_INVALID:
      blinks = 4;
      break;
    default:
      blinks = 1;
      break;
  }
  for (uint8_t i; i < blinks; i++) {
    digitalWrite(LED, 0); //turn off the LED
    delay(300);
    digitalWrite(LED, 1);
    delay(300);
  }
  delay(500); //wait 1/2 second between displays at least;
}

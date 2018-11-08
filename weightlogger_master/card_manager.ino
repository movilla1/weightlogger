#include "globals.h"

/**
 * this function compares a card with the readed bytes,
 * if they match it returns true
 */
bool compare_card(byte *card_one, byte *card_two) {
  byte i;
  for (i = 0; i < CARD_UID_SIZE; i++) {
#ifdef DEBUG    
    Serial.print("#");
    Serial.print(card_one[i], HEX);
    Serial.print("-");
    Serial.println(card_two[i], HEX);
#endif
    if (card_one[i] != card_two[i]) {
      return false;
    }
  }
  return true;
}

/**
 * this function checks if a card id sent is known by the device, if so, 
 * it returns the card_number value, it returns 0 for unknown cards
 */
byte is_known_card(byte *card_id) {
  struct card_block card;
  bool finish = false;
  int pos = 0;
  byte ret_val = 0;
  while(!finish) {
    EEPROM_readBlock(pos, card);
    if (compare_card(card_id, card.card_uid)) {
      finish = true;
      ret_val = card.card_number;
    }
    if (pos > MAX_EEPROM_POSITION && finish == false) {
      finish = true;
      ret_val = 0;
    }
    pos += sizeof(card);
  }
  return ret_val;
}

/**
 * Stores the card uid and card_number at the position indicated
 * positions are from 0 to 199
 */
void store_card(struct card_block card, byte position) {
  int pos = (5 * position);
  if ( position < 200) {  //if we are not full capacity
    EEPROM_writeBlock(pos, card); //store the card
  } else {
    sys_state = ERROR_INVALID;
  }
}

/**
 * Erases the data for a card_block in EEPROM
 * @params pos with the position to erase
 */
void erase_card(byte pos) {
  if (pos > 200) //always inside the position limit.
    return;
  int ppos = (pos * sizeof(struct card_block));
  for (byte i=0; i<sizeof(struct card_block); i++) {
    EEPROM.write(ppos+i, 0xff);
    delay(5); // 5mS between byte writes, to allow the data to be written
  }
}
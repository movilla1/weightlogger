#ifndef _CARD_MANAGER_
#include <Arduino.h>
byte is_known_card(byte *card_id);
bool compare_card(byte *card_one, byte *card_two);
bool store_card(struct card_block card, byte position);
void erase_card(byte pos);

#define _CARD_MANAGER_ 1
#endif
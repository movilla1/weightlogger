#ifndef _CARD_MANAGER_
char is_known_card(char *card_id);
bool compare_card(char *card_one, char *card_two);
bool store_card(struct card_block card, char position);
void erase_card(char pos);

#define _CARD_MANAGER_ 1
#endif
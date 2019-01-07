/*
 * card_format.h
 *
 * Created: 10-Dec-18 2:00:53 AM
 *  Author: movil
 */ 


#ifndef CARD_FORMAT_H_
#define CARD_FORMAT_H_
#include <Arduino.h>

struct card_block {
  byte card_uid[4];
  byte card_number;
};

#endif /* CARD_FORMAT_H_ */
/*
 * card_format.h
 *
 * Created: 10-Dec-18 2:00:53 AM
 *  Author: movil
 */ 


#ifndef CARD_FORMAT_H_
#define CARD_FORMAT_H_


struct card_block {
  char card_uid[4];
  char card_number;
};

#endif /* CARD_FORMAT_H_ */
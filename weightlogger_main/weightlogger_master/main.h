/*
 * main.h
 *
 * Created: 10-Dec-18 1:39:58 AM
 *  Author: movil
 */ 


#ifndef MAIN_H_
#define MAIN_H_
bool check_card_and_act();
void open_barrier();
bool check_elapsed_time();
void send_to_server();
void send_intrussion_attemp_to_server();
void alertUnknown();
void do_known_beeps();
void get_tag_data();
void check_wifi();
char tag_string_to_bytes(char *tagstring, byte *tag_uid, byte *remove);

/************************************************************************/
/* Macros for beeps                                                     */
/************************************************************************/
#define DO_KNOWN_BEEPS for(char b=0; b<2; b++){tone(BUZZER, 800, 100); delay(100);}
#define DO_INTRUSSION_BEEPS for(char i=0; i<3; i++){tone(BUZZER, 1500, 200);delay(150);}
#endif /* MAIN_H_ */
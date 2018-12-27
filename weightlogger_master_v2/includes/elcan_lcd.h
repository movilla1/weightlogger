/*
 * elcan_lcd.h
 *
 * Created: 10-Dec-18 12:36:33 AM
 *  Author: Mario O. Villarroel
 */ 


#ifndef _ELCAN_LCD_MGR
#include "definitions.h"
#include "substring.h"
#include <LiquidCrystal_I2C.h>

class ElcanLCDManager {
	protected:
    LiquidCrystal_I2C *lcd;
    long backlightStart;
  public:
    void init();
	  ElcanLCDManager();
	  void show_message(char *msg);
	  void show_error(char error_code);
	  void show_ip(char *ipaddr);
	  void show_ready(char *dateString);
    void check_light();
    void light_on();
};
#define _ELCAN_LCD_MGR 1
#endif 

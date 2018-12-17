#include <EEPROM.h>
#include <Arduino.h>  // for type definitions
#ifndef __EEWB_
template <class T> int EEPROM_writeBlock(int ee, const T& value)
{
    const char* p = (const char*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          EEPROM.write(ee++, *p++);
    return i;
}

template <class T> int EEPROM_readBlock(int ee, T& value)
{
    char* p = (char*)(void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          *p++ = EEPROM.read(ee++);
    return i;
}
#define __EEWB_ 1
#endif

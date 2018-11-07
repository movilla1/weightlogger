#include "globals.h"

void dec_to_str (char* str, uint32_t val, size_t digits)
{
  size_t i=1;

  for(; i<=digits; i++)
  {
    str[digits-i] = (char)((val % 10) + '0');
    val/=10;
  }
  str[i-1] = '\0'; // assuming you want null terminated strings?
}

void hex_string_to_byte_array(char *src, byte *out_array, byte start, byte end) {
  char pos = 0;
  char tmp;
  for (byte c = start; c < end; c += 2) {
    tmp = src[c] > 0x39 ? ((src[c] - 'A') + 10) * 16 : (src[c] - '0') * 16;
    tmp += src[c+1] > 0x39 ? ((src[c+1] - 'A') + 10) : (src[c+1] - '0');
    out_array[pos] = tmp;
    pos ++;
  }
}

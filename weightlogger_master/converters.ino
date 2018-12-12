#include "globals.h"

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

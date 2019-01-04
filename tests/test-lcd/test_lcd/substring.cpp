#include <Arduino.h>
void substring(byte src[], byte dst[], int p, int l)
{
  int c = p, d = 0;
  while (c <= l) {
    dst[d++] = src[c++];
  }
  dst[d] = '\0';
}

void short_concat(byte dest[], byte src[])
{
  char c = 0, d = 0;
  while (dest[c] != '\0') {
    c++;
  }
  while (src[d] != '\0') {
    dest[c] = src[d];
    d++; c++;
  }
  dest[c] = '\0';
}

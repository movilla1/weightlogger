void hex_string_to_byte_array(char *src, char *out_array, char start, char end) {
  char pos = 0;
  char tmp;
  for (char c = start; c < end; c += 2) {
    tmp = src[c] > 0x39 ? ((src[c] - 'A') + 10) * 16 : (src[c] - '0') * 16;
    tmp += src[c+1] > 0x39 ? ((src[c+1] - 'A') + 10) : (src[c+1] - '0');
    out_array[pos] = tmp;
    pos ++;
  }
}

void substring(char src[], char dst[], int p, int l) {
	int c = 0;	
	while (c < l) {
		dst[c] = src[p+c-1];
		c++;
	}
	src[c] = '\0';
}

void short_concat(char dest[], char src[]) {
   char c, d;
   c = 0;
   while (dest[c] != '\0') {
      c++;      
   }
   d = 0;
   while (src[d] != '\0') {
      dest[c] = src[d];
      d++;
      c++;    
   }
   dest[c] = '\0';
}

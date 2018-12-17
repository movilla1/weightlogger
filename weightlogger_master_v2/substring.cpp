void substring(char src[], char dst[], int p, int l) {
	int c = 0;	
	while (c < l) {
		dst[c] = src[p+c-1];
		c++;
	}
	src[c] = '\0';
}
#include <stdio.h>

void circuit(int input);
int XOR(int a, int b);

int main(void)
{
	circuit(0);

	return 0;
}

void circuit(int input)
{
	int i, j;
	int poly[4] = {1, 1, 0, 1};
	int s[3] = {0, 0, 0};
	int in[5] = {1, 0, 0, 1, 0};
	int out;

	for (j = 0; j < 5; j++) {
		out = s[2];
		for (i = 2; i >= 0; i--) {
			//printf("%d\n", i);
			if (i == 0) {
				if (poly[i] == 1) {
					//s[i] = XOR(in[j], out);
					s[i] = in[j] ^ out;
				}
				else {
					s[i] = in[j];
				}
			}
			else {
				if (poly[i] == 1) {
					//s[i] = XOR(s[i-1], out);
					s[i] = s[i-1] ^ out;
				}
				else {
					s[i] = s[i-1];
				}
			}
			//printf("%d\n", s[i-1]);
		}
		/*
		puts("");
		printf("%d\n", out);
		puts("");
		*/
	}
}

int XOR(int a, int b)
{
	return (a + b) % 2;
}

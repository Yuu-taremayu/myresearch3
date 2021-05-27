#include <stdio.h>

void circuit(void);
int XOR(int a, int b);
void circuit2(void);

int main(void)
{
	//circuit();
	circuit2();

	return 0;
}

void circuit(void)
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
				if (poly[0] == 1) {
					//s[i] = XOR(in[j], out);
					s[0] = in[j] ^ out;
				}
				else {
					s[0] = in[j];
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

void circuit2(void)
{
	/* 1 + x^2 + x^3 + x^4 + x^8 */
	int poly[8+1] = {1, 0, 1, 1, 1, 0, 0, 0, 1};
	/* 8bit shift resister */
	int s[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	int in;
	int out;
	int i, j, k;
	/* GF info */
	int index[256];
	int vector[256];

	for (k = 0; k < (256-1); k++) {
		for (j = 0; j < (256-1); j++) {
			/* create input */
			if (j == k) {
				in = 1;
			}
			else {
				in = 0;
			}
			printf("%d ", in);

			/* polynomial division */
			out = s[7];
			for (i = (8-1); i >= 0; i--) {
				if (i == 0) {
					if (poly[0] == 1) {
						s[0] = in ^ out;
					}
					else {
						s[0] = in;
					}
				}
				else {
					if (poly[i] == 1) {
						s[i] = s[i-1] ^ out;
					}
					else {
						s[i] = s[i-1];
					}
				}
				printf("%d", s[7-i]);
			}
			printf(" %d\n", out);
		}
		puts("");

		/*
		if (k == 255) {
			vector[k] = 0;
		}
		else {
			int tmp = 0;
			for (j = 0; j < 8; j++) {
				printf("%d", s[j]);
				tmp = tmp + (s[j] << j);
			}
			vector[k] = tmp;
		}
		*/
		int tmp = 0;
		for (j = 0; j < 8; j++) {
			//printf("%d", s[j]);
			tmp = tmp + (s[j] << j);
		}
		vector[k] = tmp;
		
		/* clear shift resister */
		for (j = 0; j < 8; j++) {
			s[j] = 0;
		}

		/* set index */
		/*
		if (k == 255) {
			index[k] = 0;
		}
		else {
			index[k] = 255-k-1;
		}
		*/
		index[k] = 254-k;
	}
	index[255] = 0;
	vector[255] = 0;

	for (k = 0; k < 256; k++) {
		printf("%d %d\n", index[k], vector[k]);
	}
}

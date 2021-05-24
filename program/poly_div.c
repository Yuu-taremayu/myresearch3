#include <stdio.h>

void circuit(void);

int main(void)
{
	int gene_poly[8+1] = {1, 0, 1, 1, 1, 0, 0, 0, 1};

	/*
	circuit();
	*/

	return 0;
}

void circuit(void)
{
	int s[8] = {0};
	int in[256+1];
	int out[256+1];
	int i;
	int j;

	for (i = 0; i < (256+1); i++) {
		/*
		printf("%d ", i);
		*/
		for (j = 0; j < (256+1); j++) {
			if (i == j) {
				in[i] = 1;
			}
			else {
				in[i] = 0;
			}
			/*
			printf("%d", in[i]);
			*/
		}
		/*
		puts("");
		*/
	}
}

#include <stdio.h>
#include <stdlib.h>

#define FIELD_SIZE 256
#define BIT_MASK 0xff

typedef struct GF_info
{
	unsigned int *index;
	unsigned int *vector;
} GF_info;

void poly_calc(GF_info GF);
void poly_div(unsigned int *index, unsigned int *vector);
unsigned int field_add(unsigned int x, unsigned int y);
unsigned int field_sub(unsigned int x, unsigned int y);
unsigned int field_mul(unsigned int x, unsigned int y, GF_info GF);
unsigned int field_div(unsigned int x, unsigned int y, GF_info GF);

int main(void)
{
	int i, j;
	unsigned int ans;
	unsigned int *index;
	unsigned int *vector;

	index = (unsigned int *)malloc(sizeof(unsigned int) * FIELD_SIZE);
	vector = (unsigned int *)malloc(sizeof(unsigned int) * FIELD_SIZE);

	poly_div(index, vector);

	GF_info GF = {index, vector};

	for (i = 0; i < 20; i++) {
		printf("%d %x\n", GF.index[i], GF.vector[i]);
	}

	/*
	for (i = 0; i < FIELD_SIZE; i++) {
		for (j = 0; j < FIELD_SIZE; j++) {
			ans = field_add(i, j);
			printf("%x + %x = %x\n", i, j, ans);
		}
	}
	*/

	/*
	for (i = 0; i < FIELD_SIZE; i++) {
		for (j = 0; j < FIELD_SIZE; j++) {
			ans = field_sub(i, j);
			printf("%x - %x = %x\n", i, j, ans);
		}
	}
	*/

	for (i = 0; i < 17; i++) {
		for (j = 0; j < 17; j++) {
			ans = field_mul(i, j, GF);
			printf("%x * %x = %x\n", i, j, ans);
		}
	}
	//printf("%d\n\n", field_mul(1, 2, GF));

	poly_calc(GF);

	free(index);
	free(vector);

	return 0;
}

void poly_calc(GF_info GF)
{
	int i, j;
	unsigned int sum = 0, mul = 1, ind = 1;
	unsigned int poly[2] = {102, 215};
	unsigned int id[3] = {1, 2, 3};

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 2; j++) {
			mul = field_mul(ind, poly[j], GF);
			sum = field_add(sum, mul);
			//printf("%d, %d, %d\n", sum, mul, ind);
			ind = field_mul(ind, id[i], GF);
			//printf("%d, %d\n", i, j);
		}
		//printf("%d\n", sum);
		sum = 0;
		mul = 1;
		ind = 1;
	}
}

void poly_div(unsigned int *index, unsigned int *vector)
{
	unsigned int gene_poly[8 + 1] = {1, 0, 1, 1, 1, 0, 0, 0, 1};
	unsigned int s[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	unsigned int in = 0, out = 0;
	int i = 0, j = 0, k = 0;
	unsigned int temp = 0;

	for (i = 0; i < (FIELD_SIZE - 1); i++) {
		for (j = 0; j < (FIELD_SIZE - 1); j++) {
			if (i == j) {
				in = 1;
			}
			else {
				in = 0;
			}

			out = s[7];
			for (k = (8 - 1); k >= 0; k--) {
				if (k == 0) {
					if (gene_poly[0] == 1) {
						s[0] = in ^ out;
					}
					else {
						s[0] = in;
					}
				}
				else {
					if (gene_poly[k] == 1) {
						s[k] = s[k - 1] ^ out;
					}
					else {
						s[k] = s[k - 1];
					}
				}
			}
		}
		temp = 0;
		for (j = 0; j < 8; j++) {
			temp = temp + (s[j] << j);
		}
		vector[(FIELD_SIZE - 1) - i] = temp;

		index[(FIELD_SIZE - 1) - i] = (FIELD_SIZE - 2) - i;

		for (j = 0; j < 8; j++) {
			s[j] = 0;
		}
	}
	index[0] = 0;
	vector[0] = 0;

	/*
	for (i = 0; i < FIELD_SIZE; i++) {
		printf("%d %x\n", index[i], vector[i]);
	}
	*/
}

unsigned int field_add(unsigned int x, unsigned int y)
{
	return (x ^ y) & BIT_MASK;
}

unsigned int field_sub(unsigned int x, unsigned int y)
{
	return (x ^ y) & BIT_MASK;
}

unsigned int field_mul(unsigned int x, unsigned int y, GF_info GF)
{
	if (x == 0 || y == 0) {
		return 0;
	}

	int i;
	int indX = 0, indY = 0, indAns = 0;

	for (i = 0; i < FIELD_SIZE; i++) {
		if (x == GF.vector[i]) {
			indX = i - 1;
		}

		if (y == GF.vector[i]) {
			indY = i - 1;
		}
	}
	indAns = (indX + indY) % (FIELD_SIZE - 1);

	/*
	for (i = 1; i < FIELD_SIZE; i++) {
		if (GF.index[indAns - 1] == GF.vector[i]) {
			return GF.vector[i];
		}
	}

	return EXIT_FAILURE;
	*/
	return GF.vector[indAns + 1];
}

unsigned int field_div(unsigned int x, unsigned int y, GF_info GF)
{
	if (x == 0) {
		return 0;
	}
	else if (y == 0) {
		return EXIT_FAILURE;
	}

	int i = 0;
	int indX, indY, indAns;

	for (i = 0; i < FIELD_SIZE; i++) {
		if (x == GF.vector[i]) {
			indX = i - 1;
		}

		if (y == GF.vector[i]) {
			indY = i - 1;
		}
	}
	indAns = (indX + indY) % (FIELD_SIZE - 1);

	/*
	for (i = 1; i < FIELD_SIZE; i++) {
		if (GF.index[indAns + 1] == GF.vector[i]) {
			return GF.vector[i];
		}
	}

	return EXIT_FAILURE;
	*/
	return GF.vector[indAns + 1];
}

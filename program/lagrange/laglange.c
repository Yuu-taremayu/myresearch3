#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Galois field size */
/* 2^3 */
/* also use modulo num */
#define FIELD_SIZE 8

/* Bit mask */
/* use BIT_MASK for truncate upper bit */
#define BIT_MASK 0x07

/* structure of GF */
typedef const struct GF_info {
	unsigned int vector[FIELD_SIZE];
	unsigned int index[FIELD_SIZE];
} GF_info;

/* lagrange interpolation */
void lagrange(void);
void base_poly(int dataNum, int i, unsigned int x, unsigned int dataX[]);

/* arithmetic functions */
unsigned int field_add(unsigned int x, unsigned int y);
unsigned int field_sub(unsigned int x, unsigned int y);
unsigned int field_mul(unsigned int x, unsigned int y, GF_info GF);
unsigned int field_div(unsigned int x, unsigned int y, GF_info GF);

int main(void)
{
	GF_info GF = {
		{0, 1, 2, 3, 4, 5, 6, 7},
		{0, 1, 2, 4, 3, 6, 7, 5}};
	unsigned int testX = 4;
	unsigned int testY = 5;
	unsigned int testZ;

	testZ = field_add(testX, testY);
	printf("%d + %d = %d\n", testX, testY, testZ);
	testZ = field_sub(testX, testY);
	printf("%d - %d = %d\n", testX, testY, testZ);
	testZ = field_mul(testX, testY, GF);
	printf("%d * %d = %d\n", testX, testY, testZ);
	testZ = field_div(testX, testY, GF);
	printf("%d / %d = %d\n", testX, testY, testZ);

	return 0;
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
	int indX;
	int indY;
	int indAns;

	for (i = 1; i < FIELD_SIZE; i++) {
		if (x == GF.index[i]) {
			indX = i - 1;
		}
		if (y == GF.index[i]) {
			indY = i - 1;
		}
	}
	/* debug lines */
	/*
	printf("indX = %d\n", indX);
	printf("indY = %d\n", indY);
	*/
	indAns = (indX + indY) % (FIELD_SIZE - 1);

	for (i = 1; i < FIELD_SIZE; i++) {
		if (GF.index[indAns + 1] == GF.vector[i]) {
			return GF.vector[i];
		}
	}

	return EXIT_FAILURE;
}

unsigned int field_div(unsigned int x, unsigned int y, GF_info GF)
{
	if (x == 0) {
		return 0;
	}
	else if (y == 0) {
		return EXIT_FAILURE;
	}

	int i;
	int indX;
	int indY;
	int indAns;

	for (i = 1; i < FIELD_SIZE; i++) {
		if (x == GF.index[i]) {
			indX = i - 1;
		}
		if (y == GF.index[i]) {
			indY = i - 1;
		}
	}
	/* debug lines */
	/*
	printf("indX = %d\n", indX);
	printf("indY = %d\n", indY);
	*/
	indAns = (indX * ((FIELD_SIZE - 1) - indY)) % (FIELD_SIZE - 1);

	for (i = 1; i < FIELD_SIZE; i++) {
		if (GF.index[indAns + 1] == GF.vector[i]) {
			return GF.vector[i];
		}
	}

	return EXIT_FAILURE;
}

void base_poly(int dataNum, int i, unsigned int x, unsigned int dataX[])
{
	unsigned int l;
	int j;

	for (j = 0; j < dataNum; j++) {
		if (j != i) {
		}
	}
}

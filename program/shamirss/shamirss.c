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

/* generating functions to prepare secret sharing */
void generate_server_id(unsigned int *serverId, int n);
void generate_polynomial(unsigned int *poly, int k);

/* lagrange interpolation */
unsigned int lagrange(int dataNum, unsigned int dataX[], unsigned int dataY[], GF_info GF);
unsigned int base_poly(int dataNum, int i, unsigned int x, unsigned int dataX[], GF_info GF);

/* arithmetic functions */
unsigned int field_add(unsigned int x, unsigned int y);
unsigned int field_sub(unsigned int x, unsigned int y);
unsigned int field_mul(unsigned int x, unsigned int y, GF_info GF);
unsigned int field_div(unsigned int x, unsigned int y, GF_info GF);

int main(void)
{
	GF_info GF = {
		{0, 1, 2, 3, 4, 5, 6, 7},
		{0, 1, 2, 4, 3, 6, 7, 5}
	};
	int k = 3;
	int n = 4;
	unsigned int *serverId;
	unsigned int *poly;
	int dataNum = 0;
	unsigned int L = 0;

	serverId = (unsigned int *)malloc(sizeof(unsigned int) * n);
	poly = (unsigned int *)malloc(sizeof(unsigned int) * k);

	generate_server_id(serverId, n);
	generate_polynomial(poly, k);

	free(serverId);
	free(poly);

	return 0;
}

/* prepare server IDs that are all different */
void generate_server_id(unsigned int *serverId, int n)
{
	int i = 0;

	for (i = 0; i < n; i++) {
		serverId[i] = i + 1;
	}
}

/* prepare polynomial for generating shares */
void generate_polynomial(unsigned int *poly, int k)
{
	int i = 0;

	for (i = 0; i < k; i++) {
		poly[i] = rand() % 2;
	}
}

/* lagrange interpolation on GF(extension field) */
/* each operations what is used here is shown at "field_***" functions */
unsigned int lagrange(int dataNum, unsigned int dataX[], unsigned int dataY[], GF_info GF)
{
	unsigned int x = 0;
	unsigned int l1 = 0;
	unsigned int l2 = 0;
	unsigned int l = 0;
	unsigned int L = 0;
	int i = 0;

	for (i = 0; i < dataNum; i++) {
		l1 = base_poly(dataNum, i, x, dataX, GF);
		l2 = base_poly(dataNum, i, dataX[i], dataX, GF);
		l = field_div(l1, l2, GF);
		L = field_add(L, field_mul(dataY[i], l, GF));
	}

	return L;
}

/* calculation base polynomial for lagrange interpolation */
/* each operations what is used here is shown at "field_***" functions */
unsigned int base_poly(int dataNum, int i, unsigned int x, unsigned int dataX[], GF_info GF)
{
	unsigned int sub = 0;
	unsigned int l = 0;
	int j = 0;

	for (j = 0; j < dataNum; j++) {
		if (j != i) {
			sub = field_sub(x, dataX[j]);
			l = field_mul(l, sub, GF);
		}
	}

	return l;
}

/* basic operations */
/* addition on GF(extension field) */
/* xor and bit mask*/
unsigned int field_add(unsigned int x, unsigned int y)
{
	return (x ^ y) & BIT_MASK;
}

/* subtraction on GF(extension field) */
/* the same as field_add() */
unsigned int field_sub(unsigned int x, unsigned int y)
{
	return (x ^ y) & BIT_MASK;
}

/* multiplication on GF(extension field) */
/* convert vector to exponentiation, calc mod and reconvert */
unsigned int field_mul(unsigned int x, unsigned int y, GF_info GF)
{
	if (x == 0 || y == 0) {
		return 0;
	}

	int i = 0;
	int indX = 0;
	int indY = 0;
	int indAns = 0;

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

/* division on GF(extension field) */
/* convert vector to exponentiation, calc mod and reconvert */
unsigned int field_div(unsigned int x, unsigned int y, GF_info GF)
{
	if (x == 0) {
		return 0;
	}
	else if (y == 0) {
		return EXIT_FAILURE;
	}

	int i = 0;
	int indX = 0;
	int indY = 0;
	int indAns = 0;

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


#include <stdio.h>
#include <stdlib.h>

#include "field_op.h"
#include "const.h"

/* set GF info that GF vector */
void set_GF_info(int *GF_vector);

/* lagrange interpolation */
int lagrange(int dataNum, int dataX[], int dataY[], int *GF_vector);
int base_poly(int dataNum, int i, int x, int dataX[], int *GF_vector);

/* arithmetic functions */
int field_add(int x, int y);
int field_sub(int x, int y);
int field_mul(int x, int y, int *GF_vector);
int field_div(int x, int y, int *GF_vector);

/* set index and GF_vector of elements on GF */
void set_GF_info(int *GF_vector)
{
	int gene_poly[8+1] = {1, 0, 1, 1, 1, 0, 0, 0, 1};
	int mem[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	int in = 0, out = 0;
	int i = 0, j = 0, k = 0;
	int temp = 0;

	for (i = 0; i < (FIELD_SIZE - 1); i++) {
		for (j = 0; j < (FIELD_SIZE - 1); j++) {
			if (i == j) {
				in = 1;
			}
			else {
				in = 0;
			}

			out = mem[7];
			for (k = (8 - 1); k >= 0; k--) {
				if (k == 0) {
					if (gene_poly[0] == 1) {
						mem[0] = in ^ out;
					}
					else {
						mem[0] = in;
					}
				}
				else {
					if (gene_poly[k] == 1) {
						mem[k] = mem[k - 1] ^ out;
					}
					else {
						mem[k] = mem[k - 1];
					}
				}
			}
		}
		temp = 0;
		for (j = 0; j < 8; j++) {
			temp = temp + (mem[j] << j);
		}
		GF_vector[(FIELD_SIZE - 1) - i] = temp;

		for (j = 0; j < 8; j++) {
			mem[j] = 0;
		}
	}
	GF_vector[0] = 0;
	
}

/* lagrange interpolation on GF(extension field) */
/* each operations what used here is shown at "field_***" functions */
int lagrange(int dataNum, int dataX[], int dataY[], int *GF_vector)
{
	int x = 0;
	int l1 = 0;
	int l2 = 0;
	int l = 0;
	int L = 0;
	int i = 0;

	for (i = 0; i < dataNum; i++) {
		l1 = base_poly(dataNum, i, x, dataX, GF_vector);
		l2 = base_poly(dataNum, i, dataX[i], dataX, GF_vector);
		l = field_div(l1, l2, GF_vector);
		L = field_add(L, field_mul(dataY[i], l, GF_vector));
	}

	return L;
}

/* calculation base polynomial for lagrange interpolation */
/* each operations what is used here is shown at "field_***" functions */
int base_poly(int dataNum, int i, int x, int dataX[], int *GF_vector)
{
	int sub = 0;
	int l = 1;
	int j = 0;

	for (j = 0; j < dataNum; j++) {
		if (j != i) {
			sub = field_sub(x, dataX[j]);
			l = field_mul(l, sub, GF_vector);
		}
	}

	return l;
}

/* basic operations */
/* addition on GF(extension field) */
/* xor and bit mask*/
int field_add(int x, int y)
{
	return (x ^ y) & BIT_MASK;
}

/* subtraction on GF(extension field) */
/* the same as field_add() */
int field_sub(int x, int y)
{
	return (x ^ y) & BIT_MASK;
}

/* multiplication on GF(extension field) */
/* convert vector to exponentiation, calc mod and reconvert */
int field_mul(int x, int y, int *GF_vector)
{
	if (x == 0 || y == 0) {
		return 0;
	}

	int i = 0;
	int indX = 0;
	int indY = 0;
	int indAns = 0;

	for (i = 1; i < FIELD_SIZE; i++) {
		if (x == GF_vector[i]) {
			indX = i - 1;
		}
		if (y == GF_vector[i]) {
			indY = i - 1;
		}
	}
	indAns = (indX + indY) % (FIELD_SIZE - 1);

	return GF_vector[indAns + 1];
}

/* division on GF(extension field) */
/* convert vector to exponentiation, calc mod and reconvert */
int field_div(int x, int y, int *GF_vector)
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

	for (i = 0; i < FIELD_SIZE; i++) {
		if (x == GF_vector[i]) {
			indX = i - 1;
		}
		if (y == GF_vector[i]) {
			indY = i - 1;
		}
	}
	indAns = (indX + ((FIELD_SIZE - 1) - indY)) % (FIELD_SIZE - 1);

	return GF_vector[indAns + 1];
}


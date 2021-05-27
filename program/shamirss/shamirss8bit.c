#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Galois field size */
/* 2^8 */
/* also use modulo num */
#define FIELD_SIZE 256

/* Bit mask */
/* use BIT_MASK for truncate upper bit */
#define BIT_MASK 0xff

/* structure of GF */
typedef const struct GF_info {
	unsigned int vector[FIELD_SIZE];
	unsigned int index[FIELD_SIZE];
} GF_info;

/* parameters of Secret Sharing */
typedef const struct SS_param {
	int k;
	int n;
} SS_param;

/* set GF info that index and vector */
void set_GF_info(unsigned int *vector, unsigned int *index);

/* generating functions to prepare secret sharing */
void generate_server_id(unsigned int *serverId, int n);
void generate_polynomial(unsigned int *poly, unsigned int secret, int k);

/* create shares */
void create_shares(unsigned int *serverId, unsigned int *poly, unsigned int *shares, SS_param SS, GF_info GF);

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
	const GF_info GF = {
		{0, 1, 2, 3, 4, 5, 6, 7},
		{0, 1, 2, 4, 3, 6, 7, 5}
	};
	const SS_param SS = {3, 4};
	unsigned int *serverId = NULL;
	unsigned int *poly = NULL;
	unsigned int *shares = NULL;
	unsigned int secret = 5;
	unsigned int L = 0;

	printf("The secret is %d\n", secret);
	serverId = (unsigned int *)malloc(sizeof(unsigned int) * (SS.n));
	poly = (unsigned int *)malloc(sizeof(unsigned int) * (SS.k));
	shares = (unsigned int *)malloc(sizeof(unsigned int) * (SS.n));

	generate_server_id(serverId, SS.n);
	generate_polynomial(poly, secret, SS.k);
	create_shares(serverId, poly, shares, SS, GF);

	L = lagrange(SS.n, serverId, shares, GF);
	printf("L = %d\n", L);

	free(serverId);
	free(poly);
	free(shares);

	return 0;
}

/* prepare server IDs that are all different */
void generate_server_id(unsigned int *serverId, int n)
{
	int i = 0;

	for (i = 0; i < n; i++) {
		serverId[i] = i + 1;
		printf("serverId[%d] = %d\n", i, serverId[i]);
	}
}

/* prepare polynomial for generating shares */
void generate_polynomial(unsigned int *poly, unsigned int secret, int k)
{
	int i = 0;

	poly[0] = secret;
	for (i = 1; i < k; i++) {
		poly[i] = rand() % FIELD_SIZE;
		printf("poly[%d] = %d\n", i, poly[i]);
	}
}

/* create shares */
void create_shares(unsigned int *serverId, unsigned int *poly, unsigned int *shares, SS_param SS, GF_info GF)
{
	int i = 0;
	int j = 0;
	unsigned int t1 = 0;
	unsigned int t2 = 1;
	unsigned int t3 = 1;

	for (i = 0; i < SS.n; i++) {
		for (j = 0; j < SS.k; j++) {

			/*
			for (k = 0; k < SS.k; k++) {
				t2 = field_mul(t2, serverId[i], GF);
				printf("k = %d\n", k);
				printf("t2 = %d\n", t2);
			}
			*/

			t2 = field_mul(t3, poly[j], GF);
			t1 = field_add(t1, t2);

			/*
			printf("%d %d %d\n", t1, t2, t3);
			*/

			t3 = field_mul(t3, serverId[i], GF);
		}
		shares[i] = t1;
		printf("shares[%d] = %d\n", i, shares[i]);
		t1 = 0;
		t3 = 1;
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
		/*
		printf("l1 = %d, l2 = %d\n", l1, l2);
		*/
		l = field_div(l1, l2, GF);
		/*
		printf("l = %d\n", l);
		*/
		L = field_add(L, field_mul(dataY[i], l, GF));
	}

	return L;
}

/* calculation base polynomial for lagrange interpolation */
/* each operations what is used here is shown at "field_***" functions */
unsigned int base_poly(int dataNum, int i, unsigned int x, unsigned int dataX[], GF_info GF)
{
	unsigned int sub = 0;
	unsigned int l = 1;
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
	indAns = (indX + ((FIELD_SIZE - 1) - indY)) % (FIELD_SIZE - 1);

	for (i = 1; i < FIELD_SIZE; i++) {
		if (GF.index[indAns + 1] == GF.vector[i]) {
			return GF.vector[i];
		}
	}

	return EXIT_FAILURE;
}


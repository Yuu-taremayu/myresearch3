#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Galois field size */
/* 2^8 */
/* also use modulo num */
#define FIELD_SIZE 256

/* Bit mask */
/* use BIT_MASK for truncate upper bit */
#define BIT_MASK 0xff

/* parameters of Secret Sharing */
typedef struct SS_param {
	int k;
	int n;
} SS_param;

/* set GF info that GF vector */
void set_GF_info(unsigned int *GF_vector);

/* generating functions to prepare secret sharing */
void generate_server_id(unsigned int *serverId, int n);
void generate_polynomial(unsigned int *poly, unsigned int secret, int k);

/* create shares */
void create_shares(unsigned int *serverId, unsigned int *poly, unsigned int *shares, SS_param SS, unsigned int *GF_vector);

/* lagrange interpolation */
unsigned int lagrange(int dataNum, unsigned int dataX[], unsigned int dataY[], unsigned int *GF_vector);
unsigned int base_poly(int dataNum, int i, unsigned int x, unsigned int dataX[], unsigned int *GF_vector);

/* arithmetic functions */
unsigned int field_add(unsigned int x, unsigned int y);
unsigned int field_sub(unsigned int x, unsigned int y);
unsigned int field_mul(unsigned int x, unsigned int y, unsigned int *GF_vector);
unsigned int field_div(unsigned int x, unsigned int y, unsigned int *GF_vector);

int main(void)
{
	const SS_param SS = {20, 30};
	unsigned int *serverId = NULL;
	unsigned int *poly = NULL;
	unsigned int *shares = NULL;
	unsigned int secret = 108;
	unsigned int L = 0;
	unsigned int *GF_vector = NULL;

	srand((unsigned)time(NULL));
	//srand(0);

	GF_vector = (unsigned int *)malloc(sizeof(unsigned int) * FIELD_SIZE);

	set_GF_info(GF_vector);
	//set_GF_info(GF_vector, index);
	/*
	for (int i = 0; i < FIELD_SIZE; i++) {
		printf("%d %x\n", GF.index[i], GF.GF_vector[i]);
	}
	*/

	printf("The secret is %d\n", secret);
	serverId = (unsigned int *)malloc(sizeof(unsigned int) * (SS.n));
	poly = (unsigned int *)malloc(sizeof(unsigned int) * (SS.k));
	shares = (unsigned int *)malloc(sizeof(unsigned int) * (SS.n));

	generate_server_id(serverId, SS.n);
	generate_polynomial(poly, secret, SS.k);
	create_shares(serverId, poly, shares, SS, GF_vector);

	L = lagrange(SS.n, serverId, shares, GF_vector);
	printf("L = %d\n", L);

	free(serverId);
	free(poly);
	free(shares);
	free(GF_vector);

	return 0;
}

/* set index and GF_vector of elements on GF */
void set_GF_info(unsigned int *GF_vector)
{
	unsigned int gene_poly[8+1] = {1, 0, 1, 1, 1, 0, 0, 0, 1};
	unsigned int mem[8] = {0, 0, 0, 0, 0, 0, 0, 0};
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
	printf("poly[%d] = %d\n", 0, poly[0]);
	for (i = 1; i < k; i++) {
		poly[i] = rand() % FIELD_SIZE;
		printf("poly[%d] = %d\n", i, poly[i]);
	}
}

/* create shares */
void create_shares(unsigned int *serverId, unsigned int *poly, unsigned int *shares, SS_param SS, unsigned int *GF_vector)
{
	int i = 0;
	int j = 0;
	unsigned int t1 = 0;
	unsigned int t2 = 1;
	unsigned int t3 = 1;

	for (i = 0; i < SS.n; i++) {
		for (j = 0; j < SS.k; j++) {
			t2 = field_mul(t3, poly[j], GF_vector);
			t1 = field_add(t1, t2);
			t3 = field_mul(t3, serverId[i], GF_vector);
		}
		shares[i] = t1;
		printf("shares[%d] = %d\n", i, shares[i]);
		t1 = 0;
		t2 = 1;
		t3 = 1;
	}
}

/* lagrange interpolation on GF(extension field) */
/* each operations what is used here is shown at "field_***" functions */
unsigned int lagrange(int dataNum, unsigned int dataX[], unsigned int dataY[], unsigned int *GF_vector)
{
	unsigned int x = 0;
	unsigned int l1 = 0;
	unsigned int l2 = 0;
	unsigned int l = 0;
	unsigned int L = 0;
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
unsigned int base_poly(int dataNum, int i, unsigned int x, unsigned int dataX[], unsigned int *GF_vector)
{
	unsigned int sub = 0;
	unsigned int l = 1;
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
/* convert GF_vector to exponentiation, calc mod and reconvert */
unsigned int field_mul(unsigned int x, unsigned int y, unsigned int *GF_vector)
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
/* convert GF_vector to exponentiation, calc mod and reconvert */
unsigned int field_div(unsigned int x, unsigned int y, unsigned int *GF_vector)
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


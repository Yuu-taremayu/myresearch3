#include <stdio.h>
#include <string.h>

/* Galois field size */
/* 2^3 */
/* also use modulo num */
#define FIELD_SIZE 8

/* Bit mask */
/* use BIT_MASK for truncate upper bit */
#define BIT_MASK 0x07

unsigned int field_add(unsigned int x, unsigned int y);
unsigned int field_sub(unsigned int x, unsigned int y);
unsigned int field_mul(unsigned int x, unsigned int y, unsigned int GF_vector[FIELD_SIZE], unsigned int GF_index[FIELD_SIZE]);

int main(int argc, char *argv[])
{
	unsigned int GF_vector[FIELD_SIZE] = {0, 1, 2, 3, 4, 5, 6, 7};
	unsigned int GF_index[FIELD_SIZE] = {0, 1, 2, 4, 3, 6, 7, 5};
	unsigned int i;
	unsigned int dataX[2] = {0, 1};
	unsigned int dataY[2] = {0, 3};

	unsigned int testX = 2;
	unsigned int testY = 1;
	unsigned int testZ;

	testZ = field_add(testX, testY);
	printf("%d + %d = %d\n", testX, testY, testZ);
	testZ = field_sub(testX, testY);
	printf("%d - %d = %d\n", testX, testY, testZ);
	testZ = field_mul(testX, testY, GF_vector, GF_index);
	printf("%d * %d = %d\n", testX, testY, testZ);

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

unsigned int field_mul(unsigned int x, unsigned int y, unsigned int GF_vector[FIELD_SIZE], unsigned int GF_index[FIELD_SIZE])
{
	if (x == 0 || y == 0) {
		return 0;
	}

	int i;
	int indX;
	int indY;
	int indAns;

	for (i = 1; i < FIELD_SIZE; i++) {
		if ((~x & GF_index[i]) == 0x00) {
			indX = i - 1;
		}
		if ((~y & GF_index[i]) == 0x00) {
			indY = i - 1;
		}
	}
	printf("indX = %d\n", indX);
	printf("indY = %d\n", indY);
	indAns = (indX + indY) % (FIELD_SIZE - 1);

	return GF_vector[GF_index[indAns + 1]];
}

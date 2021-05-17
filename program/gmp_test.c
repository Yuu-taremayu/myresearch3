#include <stdio.h>

#include "gmp.h"

int main(int argc, char *argv[])
{
	mpz_t x, y, z;

	mpz_init(x);
	mpz_init_set_str(y, "4", 10);

	mpz_set_str(x, "256", 10);

	mpz_out_str(stdout, 10, x);
	puts("");
	mpz_out_str(stdout, 2, x);
	puts("");
	mpz_out_str(stdout, 10, y);
	puts("");
	mpz_out_str(stdout, 2, y);
	puts("");
	puts("");

	mpz_add(z, x, y);
	mpz_out_str(stdout, 10, z);
	puts("");

	mpz_and(z, x, y);
	mpz_out_str(stdout, 2, z);
	puts("");

	mpz_ior(z, x, y);
	mpz_out_str(stdout, 2, z);
	puts("");

	mpz_xor(z, x, y);
	mpz_out_str(stdout, 2, z);
	puts("");

	mpz_clear(x);
	mpz_clear(y);
	mpz_clear(z);

	return 0;
}

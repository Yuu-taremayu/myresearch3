/*
 * set GF info that GF vector
 */
extern void set_GF_info(int *GF_vector);

/*
 * lagrange interpolation
 */
extern int lagrange(int dataNum, int dataX[], int dataY[], int *GF_vector);
extern int base_poly(int dataNum, int i, int x, int dataX[], int *GF_vector);

/*
 * arithmetic functions
 */
extern int field_add(int x, int y);
extern int field_sub(int x, int y);
extern int field_mul(int x, int y, int *GF_vector);
extern int field_div(int x, int y, int *GF_vector);

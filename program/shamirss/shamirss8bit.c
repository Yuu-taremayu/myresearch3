#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define _GNU_SOURCE
#include <getopt.h>

/* define message of usage */
#define USAGE "Usage: %s [--mode=m] [FILE ...]\n"

/* define extension of share file */
#define EXT ".share"
#define EXTLEN (int)strlen(EXT)

/* Galois field size */
/* 2^8 */
/* also use modulo num */
#define FIELD_SIZE 256

/* Bit mask */
/* use BIT_MASK for truncate upper bit */
#define BIT_MASK 0xff

/* for reading shares */
#define BUFSIZE 2

/* parameters of Secret Sharing */
typedef struct SS_param {
	int k;
	int n;
} SS_param;
const SS_param SS = {2, 3};

/* struct of options */
static struct option longopts[] = {
	{"mode",	required_argument,	NULL,	'm'},
	{"help",	no_argument,		NULL,	'h'},
	{0,		0,			0,	0}
};

/* set GF info that GF vector */
void set_GF_info(int *GF_vector);

/* operations of secret sharing */
void split(char *path, int *GF_vector);
void combine(char *path[], int shareNum, int *GF_vector);

/* generating functions to prepare secret sharing */
void generate_server_id(int *serverId, int n);
void generate_polynomial(int *poly, int secret, int k);

/* create shares */
void create_shares(int *serverId, int *poly, int *shares, SS_param SS, int *GF_vector);

/* lagrange interpolation */
int lagrange(int dataNum, int dataX[], int dataY[], int *GF_vector);
int base_poly(int dataNum, int i, int x, int dataX[], int *GF_vector);

/* arithmetic functions */
int field_add(int x, int y);
int field_sub(int x, int y);
int field_mul(int x, int y, int *GF_vector);
int field_div(int x, int y, int *GF_vector);

int main(int argc, char *argv[])
{
	int *GF_vector = NULL;
	int opt;
	char *mode_flag = NULL;

	srand((unsigned)time(NULL));

	/* parse option */
	if (argc < 2) {
		fprintf(stderr, "Too a few arguments\n");
		fprintf(stderr, USAGE, argv[0]);
		exit(EXIT_FAILURE);
	}

	while ((opt = getopt_long(argc, argv, "hm:", longopts, NULL)) != -1) {
		switch (opt) {
			case 'm':
				mode_flag = optarg;
				break;
			case 'h':
				fprintf(stdout, USAGE, argv[0]);
				exit(EXIT_SUCCESS);
			case '?':
				fprintf(stderr, USAGE, argv[0]);
				exit(EXIT_FAILURE);
		}
	}

	/* init GF info, vector */
	GF_vector = (int *)malloc(sizeof(int) * FIELD_SIZE);
	set_GF_info(GF_vector);

	if (strcmp(mode_flag, "split") == 0) {
		fprintf(stdout, "mode:%s\n", mode_flag);
		if (optind == argc - 1) {
			printf("%s\n", argv[optind]);
			char *path = NULL;
			path = (char *)malloc(sizeof(char) * strlen(argv[optind]));
			strcpy(path, argv[optind]);
			split(path, GF_vector);
		}
		else {
			fprintf(stderr, USAGE, argv[0]);
			exit(EXIT_FAILURE);
		}
	}
	else if (strcmp(mode_flag, "combine") == 0) {
		fprintf(stdout, "mode:%s\n", mode_flag);
		if (optind == argc) {
			fprintf(stderr, USAGE, argv[0]);
			exit(EXIT_FAILURE);
		}
		else {
			char **path = NULL;
			path = (char **)malloc(sizeof(char *) * (argc - optind));
			for (int i = 0; i < (argc - optind); i++) {
				path[i] = (char *)malloc(sizeof(char) * strlen(argv[optind + i]));
				strcpy(path[i], argv[optind + i]);
			}
			combine(path, (argc - optind), GF_vector);
			for (int i = 0; i < (argc - optind); i++) {
				free(path[i]);
			}
		}
	}
	else {
		fprintf(stderr, USAGE, argv[0]);
		exit(EXIT_FAILURE);
	}

	free(GF_vector);

	return 0;
}

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

/* split secret and create shares */
void split(char *path, int *GF_vector)
{
	/* for file IO */
	FILE *fp_sec = NULL;
	int fd_sec = 0;
	FILE **fp_sha = NULL;
	int *fd_sha = NULL;
	char *fileName = NULL;
	char *fileNum = NULL;
	int digit = 0;
	int fileNameLen = 0;
	int newFileMode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IWOTH | S_IROTH;
	/* for read or write */
	char chara;
	char *chara_sha = NULL;
	int secret = 0;
	int *serverId = NULL;
	int *poly = NULL;
	int *shares = NULL;
	int i;

	/* open secret file */
	fd_sec = open(path, O_RDONLY);
	if (fd_sec == -1) {
		fprintf(stderr, "err:open() %s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	fp_sec = fdopen(fd_sec, "r");
	if (fp_sec == NULL) {
		fprintf(stderr, "err:fdopen() %s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* dynamic memory allocation */
	serverId = (int *)malloc(sizeof(int) * (SS.n));
	poly = (int *)malloc(sizeof(int) * (SS.k));
	shares = (int *)malloc(sizeof(int) * (SS.n));
	chara_sha = (char *)malloc(sizeof(char) * 1);
	fp_sha = (FILE **)malloc(sizeof(FILE *) * SS.n);
	fd_sha = (int *)malloc(sizeof(int) * SS.n);
	if (SS.n < 10) {
		digit = 1;
	}
	else if (SS.n < 100) {
		digit = 2;
	}
	else {
		digit = 3;
	}
	fileNameLen = strlen(EXT) + 1 + digit;
	fileName = (char *)malloc(sizeof(char) * fileNameLen);
	fileNum = (char *)malloc(sizeof(char) * digit);

	/* create file of share */
	/* if it already exists the same name file, return EXIT_FAILURE */
	for (i = 0; i < SS.n; i++) {
		sprintf(fileNum, "%d", i + 1);

		if (snprintf(fileName, fileNameLen, "%s%s", fileNum, EXT) < (fileNameLen - digit)) {
			fprintf(stderr, "err:snprintf() %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		fd_sha[i] = open(fileName, O_CREAT | O_EXCL | O_APPEND | O_WRONLY, newFileMode);
		if (fd_sha[i] == -1) {
			fprintf(stderr, "err:open() %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		fp_sha[i] = fdopen(fd_sha[i], "w");
		if (fp_sha[i] == NULL) {
			fprintf(stderr, "err:fdopen() %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	generate_server_id(serverId, SS.n);

	/* read a byte, then create and write shares*/
	while ((chara = getc(fp_sec)) != EOF) {
		//printf("%c", chara);
		printf("%d\n", chara);
		secret = chara;
		if (secret > 255 || secret < 0) {
			fprintf(stderr, "invalid character\n");
			exit(EXIT_FAILURE);
		}
		generate_polynomial(poly, secret, SS.k);
		create_shares(serverId, poly, shares, SS, GF_vector);

		for (i = 0; i < SS.n; i++) {
			sprintf(chara_sha, "%02x", shares[i]);
			
			if (fputs(chara_sha, fp_sha[i]) == EOF) {
				fprintf(stderr, "err:fputs() %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}
		}
	}

	/* close stream */
	for (i = 0; i < SS.n; i++) {
		if (close(fd_sha[i]) == -1) {
			fprintf(stderr, "err:close() %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
	if (close(fd_sec) == -1) {
		fprintf(stderr, "err:close() %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	free(serverId);
	free(poly);
	free(shares);
	free(chara_sha);
	free(fileName);
	free(fileNum);
	free(fp_sha);
	free(fd_sha);
}

/* combine shares and restore secret */
void combine(char *path[], int shareNum, int *GF_vector)
{
	FILE **fp_sha = NULL;
	int *fd_sha = NULL;
	int *serverId = NULL;
	char **num = NULL;
	char *end = NULL;
	int *shares = NULL;
	int secret = 0;
	char **chara_sec = NULL;
	char c;
	int i = 0, j = 0, k = 0;

	fp_sha = (FILE **)malloc(sizeof(FILE *) * shareNum);
	fd_sha = (int *)malloc(sizeof(int) * shareNum);
	serverId = (int *)malloc(sizeof(int) * shareNum);
	shares = (int *)malloc(sizeof(int) * shareNum);
	chara_sec = (char **)malloc(sizeof(char *) * shareNum);
	for (i = 0; i < shareNum; i++) {
		chara_sec[i] = (char *)malloc(sizeof(char) * BUFSIZE);
	}
	
	/* open share file */
	for (i = 0; i < shareNum; i++) {
		fd_sha[i] = open(path[i], O_RDONLY);
		if (fd_sha[i] == -1) {
			fprintf(stderr, "err:open() %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		fp_sha[i] = fdopen(fd_sha[i], "r");
		if (fp_sha[i] == NULL) {
			fprintf(stderr, "err:fdopen() %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	/* extract serverId from file name*/
	num = (char **)malloc(sizeof(char *) * shareNum);
	j = 0;
	for (i = 0; i < shareNum; i++) {
		while (path[i][j] != '.') {
			j++;
		}
		num[i] = (char *)malloc(sizeof(char) * (j + 1));
		for (k = 0; k < j; k++) {
			num[i][k] = path[i][k];
		}
		num[i][k + 1] = '\0';
		serverId[i] = (int)strtol(num[i], &end, 10) - 1;
		j = 0;
	}

	/* read 2 characters, then reconstruct secret from shares */
	j = 0;
	while ((c = fgetc(fp_sha[0])) != EOF) {
		chara_sec[0][j] = c;
		for (i = 1; i < shareNum; i++) {
			c = fgetc(fp_sha[i]);
			chara_sec[i][j] = c;
		}
		if (j == (BUFSIZE - 1)) {
			for (k = 0; k < shareNum; k++) {
				printf("%s ", chara_sec[k]);
				shares[k] = (int)strtol(chara_sec[k], &end, 16);
				printf("%d ", shares[k]);
			}
			secret = lagrange(shareNum, serverId, shares, GF_vector);
			printf("%d", secret);
			puts("");
		}
		j = (j + 1) % BUFSIZE;
	}

	free(serverId);
	free(shares);
	for (i = 0; i < shareNum; i++) {
		free(fp_sha[i]);
		free(num[i]);
		free(chara_sec[i]);
	}
	free(fd_sha);
	free(num);
	free(chara_sec);
}

/* prepare server IDs that are all different */
void generate_server_id(int *serverId, int n)
{
	int i = 0;

	for (i = 0; i < n; i++) {
		serverId[i] = i + 1;
	}
}

/* prepare polynomial for generating shares */
void generate_polynomial(int *poly, int secret, int k)
{
	int i = 0;

	poly[0] = secret;
	for (i = 1; i < k; i++) {
		poly[i] = rand() % FIELD_SIZE;
	}
}

/* create shares */
void create_shares(int *serverId, int *poly, int *shares, SS_param SS, int *GF_vector)
{
	int i = 0;
	int j = 0;
	int t1 = 0;
	int t2 = 1;
	int t3 = 1;

	for (i = 0; i < SS.n; i++) {
		for (j = 0; j < SS.k; j++) {
			t2 = field_mul(t3, poly[j], GF_vector);
			t1 = field_add(t1, t2);
			t3 = field_mul(t3, serverId[i], GF_vector);
		}
		shares[i] = t1;
		t1 = 0;
		t2 = 1;
		t3 = 1;
	}
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


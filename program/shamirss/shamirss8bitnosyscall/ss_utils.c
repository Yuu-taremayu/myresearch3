#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "field_op.h"
#include "ss_utils.h"
#include "const.h"

/*
 * operations of secret sharing
 */
void split(char *path, int *GF_vector);
void combine(char *path[], int shareNum, int *GF_vector);

/*
 * generating functions to prepare secret sharing
 */
void generate_server_id(int *serverId, int n);
void generate_polynomial(int *poly, int secret, int k);

/*
 * create shares
 */
void create_shares(int *serverId, int *poly, int *shares, SS_param SS, int *GF_vector);

extern SS_param SS;

/*
 * split secret and create shares
 */
void split(char *path, int *GF_vector)
{
	/*
	 * for file IO
	 */
	FILE *fpSecret = NULL;
	int fdSecret = 0;
	FILE **fpShares = NULL;
	int *fdShares = NULL;
	char *fileName = NULL;
	char *fileNum = NULL;
	int digit = 0;
	int fileNameLen = 0;
	int newFileMode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IWOTH | S_IROTH;
	int openFlag = O_CREAT | O_EXCL | O_APPEND | O_WRONLY;
	/* 
	 * for read or write
	 */
	char chara;
	char *charaShares = NULL;
	int secret = 0;
	int *serverId = NULL;
	int *poly = NULL;
	int *shares = NULL;
	int i;

	/*
	 * open secret file
	 */
	fdSecret = open(path, O_RDONLY);
	if (fdSecret == -1) {
		fprintf(stderr, "err:open() %s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	fpSecret = fdopen(fdSecret, "r");
	if (fpSecret == NULL) {
		fprintf(stderr, "err:fdopen() %s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	/*
	 * dynamic memory allocation
	 */
	serverId = (int *)malloc(sizeof(int) * (SS.n));
	poly = (int *)malloc(sizeof(int) * (SS.k));
	shares = (int *)malloc(sizeof(int) * (SS.n));
	charaShares = (char *)malloc(sizeof(char) * 1);
	fpShares = (FILE **)malloc(sizeof(FILE *) * SS.n);
	fdShares = (int *)malloc(sizeof(int) * SS.n);
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

	/*
	 * create file of share
	 * if it already exists the same name file, return EXIT_FAILURE
	 */
	for (i = 0; i < SS.n; i++) {
		if (sprintf(fileNum, "%d", i + 1) < 0) {
			fprintf(stderr, "err:sprintf() %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		if (snprintf(fileName, fileNameLen, "%s%s", fileNum, EXT) < (fileNameLen - digit)) {
			fprintf(stderr, "err:snprintf() %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		fdShares[i] = open(fileName, openFlag, newFileMode);
		if (fdShares[i] == -1) {
			fprintf(stderr, "err:open() %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		fpShares[i] = fdopen(fdShares[i], "w");
		if (fpShares[i] == NULL) {
			fprintf(stderr, "err:fdopen() %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	generate_server_id(serverId, SS.n);

	/*
	 * read a byte, then create and write shares
	 */
	while ((chara = getc(fpSecret)) != EOF) {
		secret = chara;
		if (secret > 255 || secret < 0) {
			fprintf(stderr, "invalid character\n");
			exit(EXIT_FAILURE);
		}
		generate_polynomial(poly, secret, SS.k);
		create_shares(serverId, poly, shares, SS, GF_vector);

		for (i = 0; i < SS.n; i++) {
			if (sprintf(charaShares, "%02x", shares[i]) < 0) {
				fprintf(stderr, "err:sprintf() %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}
			
			if (fprintf(fpShares[i], "%s", charaShares) < 0) {
				fprintf(stderr, "err:fprintf() %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}
		}
	}

	/*
	 * close stream
	 */
	for (i = 0; i < SS.n; i++) {
		if (fclose(fpShares[i]) != 0) {
			fprintf(stderr, "err:fclose() %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
	if (fclose(fpSecret) != 0) {
		fprintf(stderr, "err:fclose() %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	/*
	 * free variables
	 */
	free(serverId);
	free(poly);
	free(shares);
	free(charaShares);
	free(fileName);
	free(fileNum);
	free(fpShares);
	free(fdShares);
}

/*
 * combine shares and restore secret
 */
void combine(char *path[], int shareNum, int *GF_vector)
{
	/*
	 * for file IO
	 */
	FILE **fpShares = NULL;
	int *fdShares = NULL;
	int *serverId = NULL;
	char **num = NULL;
	char *end = NULL;
	int *shares = NULL;
	int secret = 0;
	char **charaSecret = NULL;
	char *outputFilename = "secret.reconst";
	FILE *fpOut = NULL;
	int fdOut = 0;
	int newFilemode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IWOTH | S_IROTH;
	int openFlag = O_CREAT | O_EXCL | O_APPEND | O_WRONLY;
	int c;
	int i = 0, j = 0, k = 0;

	/*
	 * dynamic memory allocation
	 */
	fpShares = (FILE **)malloc(sizeof(FILE *) * shareNum);
	fdShares = (int *)malloc(sizeof(int) * shareNum);
	serverId = (int *)malloc(sizeof(int) * shareNum);
	shares = (int *)malloc(sizeof(int) * shareNum);
	charaSecret = (char **)malloc(sizeof(char *) * shareNum);
	for (i = 0; i < shareNum; i++) {
		charaSecret[i] = (char *)malloc(sizeof(char) * BUFSIZE);
	}
	
	/*
	 * open share file
	 */
	for (i = 0; i < shareNum; i++) {
		fdShares[i] = open(path[i], O_RDONLY);
		if (fdShares[i] == -1) {
			fprintf(stderr, "err:open() %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		fpShares[i] = fdopen(fdShares[i], "r");
		if (fpShares[i] == NULL) {
			fprintf(stderr, "err:fdopen() %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	/*
	 * open output file in create mode
	 */
	fdOut = open(outputFilename, openFlag, newFilemode);
	if (fdOut == -1) {
		fprintf(stderr, "err:open() %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	fpOut = fdopen(fdOut, "a");
	if (fpOut == NULL) {
		fprintf(stderr, "err:fdopen() %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	/*
	 * extract serverId from file name
	 */
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
		serverId[i] = (int)strtol(num[i], &end, 10);
		j = 0;
	}

	/*
	 * read 2 characters, then reconstruct secret from shares
	 */
	j = 0;
	while ((c = fgetc(fpShares[0])) != EOF) {
		charaSecret[0][j] = c;
		for (i = 1; i < shareNum; i++) {
			c = fgetc(fpShares[i]);
			charaSecret[i][j] = c;
		}
		if (j == (BUFSIZE - 1)) {
			for (k = 0; k < shareNum; k++) {
				shares[k] = (int)strtol(charaSecret[k], &end, 16);
			}
			secret = lagrange(shareNum, serverId, shares, GF_vector);

			if (fprintf(fpOut, "%c", secret) < 0) {
				fprintf(stderr, "err:fprintf() %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}
		}
		j = (j + 1) % BUFSIZE;
	}

	/*
	 * close stream
	 */
	for (i = 0; i < shareNum; i++) {
		if (fclose(fpShares[i]) != 0) {
			fprintf(stderr, "err:fclose() %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
	if (fclose(fpOut) != 0) {
		fprintf(stderr, "err:fclose() %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	/*
	 * free variables
	 */
	free(serverId);
	free(shares);
	for (i = 0; i < shareNum; i++) {
		free(num[i]);
		free(charaSecret[i]);
	}
	free(fpShares);
	free(fdShares);
	free(num);
	free(charaSecret);
}

/*
 * prepare server IDs that are all different
 */
void generate_server_id(int *serverId, int n)
{
	int i = 0;

	for (i = 0; i < n; i++) {
		serverId[i] = i + 1;
	}
}

/*
 * prepare polynomial for generating shares
 */
void generate_polynomial(int *poly, int secret, int k)
{
	int i = 0;

	poly[0] = secret;
	for (i = 1; i < k; i++) {
		poly[i] = rand() % FIELD_SIZE;
	}
}

/*
 * create shares
 */
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

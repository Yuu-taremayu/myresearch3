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

/* operations of secret sharing */
void split(char *path, int *GF_vector);
void combine(char *path[], int shareNum, int *GF_vector);

/* generating functions to prepare secret sharing */
void generate_server_id(int *serverId, int n);
void generate_polynomial(int *poly, int secret, int k);

/* create shares */
void create_shares(int *serverId, int *poly, int *shares, SS_param SS, int *GF_vector);

extern SS_param SS;

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
		printf("%x, %x\n", chara, EOF);
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
	int c;
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
		serverId[i] = (int)strtol(num[i], &end, 10);
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
				shares[k] = (int)strtol(chara_sec[k], &end, 16);
			}
			secret = lagrange(shareNum, serverId, shares, GF_vector);
			printf("%c", secret);
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

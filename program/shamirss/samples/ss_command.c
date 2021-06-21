#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#define _GNU_SOURCE
#include <getopt.h>

#define USAGE "Usage: %s [--mode=m] [FILE ...]\n"

static struct option longopts[] = {
	{"mode",	required_argument,	NULL,	'm'},
	{"help",	no_argument,		NULL,	'h'},
	{0,		0,			0,	0}
};

/* operation of secret sharing */
void split(char *path);
void combine(char *path[], int shareNum);

int main(int argc, char *argv[])
{
	int opt;
	char *mode_flag = NULL;

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

	if (strcmp(mode_flag, "split") == 0) {
		printf("mode:%s\n", mode_flag);
		if (optind == argc - 1) {
			printf("%s\n", argv[optind]);
			char *path = NULL;
			path = (char *)malloc(sizeof(char) * strlen(argv[optind]));
			strcpy(path, argv[optind]);
			split(path);
		}
		else {
			fprintf(stderr, USAGE, argv[0]);
			exit(EXIT_FAILURE);
		}
	}
	else if (strcmp(mode_flag, "combine") == 0) {
		printf("mode:%s\n", mode_flag);
		if (optind == argc) {
			fprintf(stderr, USAGE, argv[0]);
			exit(EXIT_FAILURE);
		}
		else {
			for (int i = optind; i < argc; i++) {
				printf("%s\n", argv[i]);
			}
			char **path = NULL;
			path = (char **)malloc(sizeof(char *) * (argc - optind));
			for (int i = 0; i < (argc - optind); i++) {
				path[i] = (char *)malloc(sizeof(char) * strlen(argv[optind + i]));
				strcpy(path[i], argv[optind + i]);
			}
			combine(path, (argc - optind));
		}
	}
	else {
		fprintf(stderr, USAGE, argv[0]);
		exit(EXIT_FAILURE);
	}

	return 0;
}

/* split step in secret sharing */
void split(char *path)
{
	FILE *fp;
	int fd;
	char c;

	fd = open(path, O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "error:%s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	fp = fdopen(fd, "r");
	if (fp == NULL) {
		fprintf(stderr, "error:%s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* go to shamir's secret sharing */
	/*
	while ((c = getc(fp)) != EOF) {
		printf("%c", c);
	}
	*/
	printf("Split success\n");
	fclose(fp);
}

/* combine step in secret sharing */
void combine(char *path[], int shareNum)
{
	FILE *fp;
	int fd;
	int i;

	for (i = 0; i < shareNum; i++) {
		fd = open(path[i], O_RDONLY);
		if (fd == -1) {
			fprintf(stderr, "error:%s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		fp = fdopen(fd, "r");
		if (fp == NULL) {
			fprintf(stderr, "error:%s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		fclose(fp);
	}

	printf("Combine success\n");
}

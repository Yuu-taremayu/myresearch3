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

#include "field_op.h"
#include "ss_utils.h"
#include "const.h"

/* struct of options */
static struct option longopts[] = {
	{"mode",	required_argument,	NULL,	'm'},
	{"help",	no_argument,		NULL,	'h'},
	{0,		0,			0,	0}
};

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

	while ((opt = getopt_long(argc, argv, "knhm:", longopts, NULL)) != -1) {
		switch (opt) {
			case 'k':
				mode_flag = optarg;
				break;
			case 'n':
				mode_flag = optarg;
				break;
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

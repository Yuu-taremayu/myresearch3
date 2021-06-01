#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _GNU_SOURCE
#include <getopt.h>

#define USAGE "Usage: %s [--mode=m] [FILE ...]\n"

static struct option longopts[] = {
	{"mode",	required_argument,	NULL,	'm'},
	{"help",	no_argument,		NULL,	'h'},
	{0,		0,			0,	0}
};

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
		printf("%s\n", mode_flag);
		if (optind == argc - 1) {
			printf("%s\n", argv[optind]);
		}
		else {
			fprintf(stderr, USAGE, argv[0]);
			exit(EXIT_FAILURE);
		}
	}
	else if (strcmp(mode_flag, "combine") == 0) {
		printf("%s\n", mode_flag);
		if (optind == argc) {
			fprintf(stderr, USAGE, argv[0]);
			exit(EXIT_FAILURE);
		}
		for (int i = optind; i < argc; i++) {
			printf("%s\n", argv[i]);
		}
	}
	else {
		fprintf(stderr, USAGE, argv[0]);
		exit(EXIT_FAILURE);
	}

	return 0;
}

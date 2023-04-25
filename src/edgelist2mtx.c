#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "arg.h"


char *argv0;
char errdesc[128] = "unknown error";


void printusage(void);
void printhelp(void);
void printerr(void);

int formatcheck(FILE *fd, int *np, int *mp);
int f2fconvert(FILE *infd, FILE *outfd, int n, int m);


void printusage(void) {
	printf("Usage:\n\tedgelist2mtx -h\n\tedgelist2mtx INPUT OUTPUT\n");
}

void printhelp(void) {
	printf("Convert a graph from INPUT text file in edgelist format into a MatrixMarket text file.\n");
	printf("\t-h\t\tprint this help\n");
}

void printerr(void) {
	printf("edgelist2mtx: %s\n", errdesc);
}

//TODO: need to check for multi-edges, add toggles to check for 1-based or 0-based IDs
//also add weighted format check toggle
int formatcheck(FILE* fd, int *np, int *mp) {
	char *line = NULL;
	size_t len = 0;
	int u, v;
	int linecnt = 0;
	int edgecnt = 0;

	int minid = -1, maxid = -1;
	int locmin, locmax;

	while(getline(&line, &len, fd) != -1) {
		++linecnt;
		if(line[0] == '%' || line[0] == '#') {
			continue;
		}
		++edgecnt;
		//TODO: stricter format ensuring (maybe strtok)
		if(sscanf(line, "%d %d\n", &u, &v) != 2) {
			printf("Error: invalid line %d: \"%.*s\"\n", linecnt, strlen(line) - 1, line);
			return -1;
		}
		if(u <= 0 || v <= 0) {
			printf("Error: invalid vertex id (line %d): \"%.*s\"\n", linecnt, strlen(line) - 1, line);
			return -1;
		}
		locmin = (u < v) ? u : v;
		locmax = (u > v) ? u : v;
		if(minid > locmin || minid == -1) {
			minid = locmin;
		}
		if(maxid < locmax) {
			maxid = locmax;
		}
	}

	if(linecnt == 0) {
		printf("Error: empty file\n");
		return -1;
	} else if(edgecnt == 0) {
		printf("Error: file contains only comments, no edges\n");
		return -1;
	}

	//TODO: currently assuming indices in file are 1-based
	*np = maxid;
	*mp = edgecnt;

	return 0;
}

int f2fconvert(FILE *infd, FILE *outfd, int n, int m) {
	char *line = NULL;
	size_t len = 0;

	fprintf(outfd, "%%%%MatrixMarket matrix coordinate pattern general\n");

	while(getline(&line, &len, infd) != -1) {
		if(line[0] == '%' || line[0] == '#') {
			line[0] = '%';
			fprintf(outfd, "%.*s\n", strcspn(line, "\n"), line);
		} else {
			break;
		}
	}

	fprintf(outfd, "%d %d %d\n", n, n, m);
	fprintf(outfd, "%s", line);

	while(getline(&line, &len, infd) != -1) {
		fprintf(outfd, "%s", line);
	}

	return 0;
}


int main(int argc, char *argv[]) {
	char *infilename;
	char *outfilename;

	ARGBEGIN {
	case 'h':
		printusage();
		printhelp();
		return 0;
	default:
		sprintf(errdesc, "invalid option: '%c'", ARGC());
		printerr();
		printusage();
		return -1;
	} ARGEND

	if(argc < 2) {
		sprintf(errdesc, "too few arguments");
		printerr();
		return -1;
	} else if(argc > 2) {
		sprintf(errdesc, "too many arguments");
		printerr();
		return -1;
	}

	infilename = argv[0];
	outfilename = argv[1];

	FILE *infd = fopen(infilename, "r");
	if(infd == NULL) {
		sprintf(errdesc, "unable to open input file \"%s\"", infilename);
		printerr();
		return -1;
	}

	int n, m;
	//TODO: check for correct ID range, multi-edges, all lines containing correct format ID ID edges
	if(formatcheck(infd, &n, &m) != 0) {
		sprintf(errdesc, "incorrect input file format");
		printerr();
		return -1;
	}

	rewind(infd);
	FILE *outfd = fopen(outfilename, "w");
	if(outfd == NULL) {
		sprintf(errdesc, "unable to open output file \"%s\"", outfilename);
		printerr();
		return -1;
	}

	//TODO: maybe add a version with whole file reading (or at least buffered reading)
	//for cases when memory is sufficient
	if(f2fconvert(infd, outfd, n, m) != 0) {
		sprintf(errdesc, "error while performing read-write sequence");
		printerr();
		return -1;
	}

	fclose(infd);
	fclose(outfd);

	return 0;
}


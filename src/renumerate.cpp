#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <limits>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "arg.h"

char *argv0;
char warndesc[512] = "unknown warning";
char errdesc[512] = "unknown error";

void printusage(void);
void printhelp(void);
void printwarn(void);
void printerr(void);

void
printusage(void)
{
	printf("Usage:\n\trenumerate -h\n\trenumerate [OPTION] INPUT OUTPUT\n");
}

void
printhelp(void)
{
	printf("Renumerate a graph in INPUT, write to OUTPUT.\n");
	printf("Valid options:\n");

	printf("\t-h    print this help\n");

	printf("\t-v V  set verbosity level V (default 0)\n");
	printf("\t       0: don't print anything\n");
	printf("\t       1: print some information\n");

	printf("\t-s S  use shifted renumeration: IDs start\n");
	printf("\t       with S instead of 0, S in [0, uint64_t max]\n");
}

void
printwarn(void)
{
	printf("renumerate \033[33mwarning\033[0m: %s\n", warndesc);
}

void
printerr(void)
{
	printf("renumerate \033[31merror\033[0m: %s\n", errdesc);
}

int
main(int argc, char *argv[])
{
	int verbosity = 0;
	uint64_t idshift = 0;

	ARGBEGIN {
	case 'h':
		printusage();
		printhelp();
		return 0;
	case 'v':
		sprintf(errdesc, "option-argument for option \"-v\" is mandatory");
		verbosity = atoi(EARGF(printerr()));
		if (verbosity <= 0 || verbosity > 1) {
			sprintf(errdesc, "invalid verbosity level: %d", verbosity);
			printerr();
			return 1;
		}
		break;
	case 's':
		sprintf(errdesc, "option-argument for option \"-s\" is mandatory");
		idshift = atoi(EARGF(printerr()));
		break;
	default:
		sprintf(errdesc, "invalid option \"%c\"", ARGC());
		printerr();
		printusage();
		return 1;
	} ARGEND

	if (argc < 2) {
		sprintf(errdesc, "too few arguments");
		printerr();
		return 1;
	} else if (argc > 2) {
		sprintf(errdesc, "too many arguments");
		printerr();
		return 1;
	}

	char *infilename = new char[strlen(argv[0])];
	strcpy(infilename, argv[0]);

	int argv1len = strlen(argv[1]);
	char outgraphfilesuff[] = ".renumerated";
	char *outgraphfilename = new char[argv1len + strlen(outgraphfilesuff)];
	strcpy(outgraphfilename, argv[1]);
	strcpy(outgraphfilename + argv1len, outgraphfilesuff);

	char new2oldfilesuff[] = ".new2old";
	char *new2oldfilename = new char[argv1len + strlen(new2oldfilesuff)];
	strcpy(new2oldfilename, argv[1]);
	strcpy(new2oldfilename + argv1len, new2oldfilesuff);

	char old2newfilesuff[] = ".old2new";
	char *old2newfilename = new char[argv1len + strlen(old2newfilesuff)];
	strcpy(old2newfilename, argv[1]);
	strcpy(old2newfilename + argv1len, old2newfilesuff);

	FILE *infd;
	infd = fopen(infilename, "r");
	if (infd == NULL) {
		sprintf(errdesc, "unable to read input file \"%s\"", infilename);
		printerr();
		return 1;
	}

	std::unordered_set<int64_t> invertices;
	std::unordered_map<int64_t, std::unordered_set<int64_t>> inedges;
	uint64_t edgecnt = 0;
	uint64_t linecnt = 0;

	char *line = NULL;
	size_t len = 0;
	int64_t v0, v1;
	while (getline(&line, &len, infd) != -1) {
		++linecnt;
		if (line[0] == '#' || line[0] == '%') {
			if (verbosity > 0) {
				printf("Encountered a commentary line\n");
			}
			continue;
		}

		if (sscanf(line, "%" SCNd64 " %" SCNd64, &v0, &v1) == 2) {
			inedges[v0].insert(v1);

			invertices.insert(v0);
			invertices.insert(v1);
			++edgecnt;
		} else {
			sprintf(errdesc, "invalid line %" PRIu64 ": \"%s\"", linecnt, line);
			printerr();
			return 1;
		}
	}

	if (line) {
		free(line);
	}

	fclose(infd);

	if (verbosity > 0) {	
		printf("Vertices: %lu, edges: %" PRIu64 "\n", invertices.size(), edgecnt);
	}

	if (verbosity > 0) {
		printf("Constructing renumeration structures\n");
	}

	std::vector<int64_t> new2oldvec = std::vector<int64_t>(invertices.begin(), invertices.end());

	std::unordered_map<int64_t, uint64_t> old2newmap;
	uint64_t i;
	for (i = 0; i < new2oldvec.size(); ++i) {
		old2newmap[new2oldvec[i]] = i;
	}

	if (verbosity > 0) {
		printf("Writing renumerated graph to \"%s\"\n", outgraphfilename);
	}

	FILE *outfd;
	outfd = fopen(outgraphfilename, "w");
	if (outfd == NULL) {
		sprintf(errdesc, "unable to open output file \"%s\"", outgraphfilename);
		printerr();
		return 1;
	}

	for (i = 0; i < new2oldvec.size(); ++i) {
		uint64_t u = new2oldvec[i];
		for (auto it = inedges[u].begin(); it != inedges[u].end(); ++it) {
			fprintf(outfd, "%" PRIu64 " %" PRIu64 "\n", i + idshift, old2newmap[*it] + idshift);
		}
	}

	fclose(outfd);

	if (verbosity > 0) {
		printf("Writing old-to-new conversion table to \"%s\"\n", old2newfilename);
	}

	outfd = fopen(old2newfilename, "w");
	if (outfd == NULL) {
		sprintf(errdesc, "unable to open output file \"%s\"", old2newfilename);
		printerr();
		return 1;
	}

	for (i = 0; i < new2oldvec.size(); ++i) {
		fprintf(outfd, "%" PRId64 " %" PRIu64 "\n", new2oldvec[i], i + idshift);
	}

	fclose(outfd);

	if (verbosity > 0) {
		printf("Writing new-to-old conversion table to \"%s\"\n", new2oldfilename);
	}

	outfd = fopen(new2oldfilename, "w");
	if (outfd == NULL) {
		sprintf(errdesc, "unable to open output file \"%s\"", new2oldfilename);
		printerr();
		return 1;
	}

	for (i = 0; i < new2oldvec.size(); ++i) {
		fprintf(outfd, "%" PRIu64 " %" PRId64 "\n", i + idshift, new2oldvec[i]);
	}

	fclose(outfd);

	delete[] infilename;
	delete[] outgraphfilename;
	delete[] new2oldfilename;
	delete[] old2newfilename;

	return 0;
}


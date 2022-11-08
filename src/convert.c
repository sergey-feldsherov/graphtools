#include <stdio.h>
#include <stdbool.h>
#include "arg.h"


void printUsage(void);
const char* usageStr = "convert [-dhs] INPUT [OUTPUT]";
void printHelp(void);

bool strict = false;
bool debug = false;
char *inputFileName = NULL;
char *outputFileName = NULL;

char *argv0 = NULL;


int main(int argc, char *argv[]) {
	ARGBEGIN {
	case 's':
		strict = true;
		break;
	case 'd':
		debug = true;
		break;
	case 'h':
		printHelp();
		break;
	default:
		printf("convert: invalid option\n");
		printUsage();
		printf("\nTry 'convert -h' for more options.\n");

		return -1;
	} ARGEND

	if(argc < 1) {
		printf("convert: missing INPUT\n");
		printUsage();

		return -1;
	} else {
		inputFileName = argv[0];
	}

	if(argc > 2) {
		printf("convert: too many arguments\n");
		printUsage();

		return -1;
	} else if(argc == 2) {
		outputFileName = argv[1];
	}

	if(debug) {
		printf("Reading input file: '%s'\n", inputFileName);
	}

	

	if(debug) {
		if(outputFileName == NULL) {
			printf("Writing output to STDOUT\n");
		} else {
			printf("Writing output to '%s'\n", outputFileName);
		}
	}

	return 0;
}


void printUsage(void) {
	printf("Usage: %s\n", usageStr);

	return;
}


void printHelp(void) {
	printUsage();

	printf("\t-d\trun in debug mode\n");
	printf("\t-h\tprint this message\n");
	printf("\t-s\tenable strict checking mode (currently no effect)\n");

	return;
}

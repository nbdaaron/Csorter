#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Sorter.h"

int main(int argc, char **argv) {

	struct csv *csv = parseCSV();
	
	return 0;
}

struct csv *parseCSV() {
	enum type *types = getTypes();
	char eofReached = 0, newlineFound = 0, entryFound = 0, next;
	int scanResult;
	struct csv *ret = malloc(sizeof(struct csv));

	ret->entries = malloc(sizeof(struct entry *) * maxEntries);
	while (!eofReached) {
		newlineFound = 0, entryFound = 0;
		while (!newlineFound && !eofReached) {
			scanResult = scanf("%c", &next);
			if (scanResult == EOF) {
				eofReached = 1;
				break;
			}
			if (next == '\r' || next == '\n') {
				newlineFound = 1;
				break;
			} else {
				entryFound = 1;
				printf("%c", next);
			}
		}
		if (entryFound) {
			printf("\nReached End of Line.\n");
		}
	}


	return NULL;
}

///Parse first line of CSV and get array of data types for values.
enum type *getTypes() {

	enum type *types = malloc(sizeof(enum type) * columns);

	char *currentInput = malloc(sizeof(char) * maxStringSize);
	char nextChar;
	int stringPosition, retPosition=0;
	char newlineFound = 0;

	//Ignore leading comma if exists.
	scanf(",");

	while (!newlineFound) {
		stringPosition = 0;
		while (scanf("%c", &nextChar) > 0) {
			if (nextChar == ',') {
				break;
			} else if (nextChar == '\n' || nextChar == '\r') {
				scanf("\r\n");
				newlineFound = 1;
				break;
			}
			currentInput[stringPosition] = nextChar;
			stringPosition++;
		}
		//Add null-terminating 0 to end of String.
		currentInput[stringPosition] = '\0';
		types[retPosition++] = getTypeFromColumnName(currentInput);
	}

	free(currentInput);

	return types;
}

enum type getTypeFromColumnName(char *name) {
	int i;

	for (i=0;i<stringValuesSize;i++) {
		if (strcmp(stringValues[i], name) == 0) {
			return string;
		}
	}

	for (i=0;i<intValuesSize;i++) {
		if (strcmp(intValues[i], name) == 0) {
			return integer;
		}
	}

	for (i=0;i<doubleValuesSize;i++) {
		if (strcmp(doubleValues[i], name) == 0) {
			return decimal;
		}
	}
	return error;
}

void mergesortMovieList(struct csv *list, char *query) {
	return;
}

void printMovieList(struct csv *list) {
	return;
}
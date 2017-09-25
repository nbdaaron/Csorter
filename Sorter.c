#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Sorter.h"

int main(int argc, char **argv) {

	struct csv *csv = parseCSV();
	
	return 0;
}

struct csv *parseCSV() {
	int i;
	char eofReached = 0, newlineFound = 0, entryFound = 0, next;
	int scanResult;
	struct csv *ret = malloc(sizeof(struct csv));
	struct headerInfo headerInfo = getHeaderInfo();
	ret->entries = malloc(sizeof(struct entry) * maxEntries);
	ret->columnTypes = headerInfo.types;
	ret->columnNames = headerInfo.columnNames;

	char *string = malloc(sizeof(char)*maxStringSize);
	int stringPosition;

	int quotationMarksFound = 0;

	int currentEntryPosition = 0;
	int currentValuePosition = 0;
	struct entry *currentEntry;
	union value *currentValue;

	while (!eofReached) {
		newlineFound = 0, entryFound = 0, stringPosition = 0;

		currentEntry = malloc(sizeof(struct entry));
		currentEntry -> values = malloc(sizeof(union value *) * columns);

		for (i = 0; i<columns; i++) {
			currentEntry -> values[i] = malloc(sizeof(union value *));
		}

		currentValue = malloc(sizeof(union value));

		while (!newlineFound && !eofReached) {
			scanResult = scanf("%c", &next);
			if (scanResult == EOF) {
				string[stringPosition++] = '\0';
				if (strlen(string)>0) {
					//printf("String: %s\n", string);
					currentEntry -> values[currentValuePosition++] -> stringVal = string;
				}
				stringPosition = 0;


				string = malloc(sizeof(char)*maxStringSize);
				eofReached = 1;
				break;
			}
			if (next == '\r' || next == '\n') {
				string[stringPosition++] = '\0';
				if (strlen(string)>0) {
					//printf("String: %s\n", string);
					currentEntry -> values[currentValuePosition++] -> stringVal = string;
				}
				stringPosition = 0;


				string = malloc(sizeof(char)*maxStringSize);
				newlineFound = 1;
				break;
			} /*else if (next == '"') {
				printf("Quotes Found!");
			}*/ else if (next == ',') {
				string[stringPosition++] = '\0';
				if (strlen(string)>0) {
					//printf("String: %s\n", string);
					currentEntry -> values[currentValuePosition++] -> stringVal = string;

					//For now, if the CSV file has too many columns in a single row, ignore the rest of that row.
					//Should be fixed when quotation marks are handled.
					if (currentValuePosition > columns) {
						newlineFound = 1;
					}
				}
				stringPosition = 0;	

				string = malloc(sizeof(char)*maxStringSize);
			} else {
				entryFound = 1;
				string[stringPosition++] = next;
			}
		}
		if (entryFound) {
			ret->entries[currentEntryPosition++] = currentEntry;

			currentEntry = malloc(sizeof(struct entry));
			currentEntry -> values = malloc(sizeof(union value *) * columns);
			for (i = 0; i<columns; i++) {
				currentEntry -> values[i] = malloc(sizeof(union value *));
			}
			currentValue = malloc(sizeof(union value));
			currentValuePosition = 0;
		}
	}


	return ret;
}

///Parse first line of CSV and get array of data types for values.
struct headerInfo getHeaderInfo() {

	struct headerInfo ret;

	char **columnNames = malloc(sizeof(char *) * columns);
	enum type *types = malloc(sizeof(enum type) * columns);

	char *currentInput;
	char nextChar;
	int stringPosition, retPosition=0;
	char newlineFound = 0;

	//Ignore leading comma if exists.
	scanf(",");

	while (!newlineFound) {
		currentInput = malloc(sizeof(char) * maxStringSize);
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
		types[retPosition] = getTypeFromColumnName(currentInput);
		columnNames[retPosition++] = currentInput;
	}

	ret.columnNames = columnNames;
	ret.types = types;

	return ret;
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
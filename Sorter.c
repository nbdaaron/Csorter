#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Sorter.h"

int main(int argc, char **argv) {

	struct csv *csv = parseCSV();
	char *sortBy = argv[2];
	
	//Debug Command to test CSV Parser.
	printRange(csv, 195, 215, 16);

	freeCSV(csv);
	
	printf("Sorting by: %s\n", sortBy);

	return 0;
}

struct csv *parseCSV() {

	//Pointer to CSV file that will be returned.
	struct csv *ret = malloc(sizeof(struct csv));

	//Retrieve header info and populate CSV with values.
	struct headerInfo headerInfo = getHeaderInfo();
	ret->columnTypes = headerInfo.types;
	ret->columnNames = headerInfo.columnNames;

	//Create space for CSV entries.
	ret->entries = getCSVEntries(ret->columnTypes);

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

struct entry **getCSVEntries(enum type *columnTypes) {

	//Return value: Array of Entry Pointers.
	struct entry **ret = malloc(sizeof(struct entry) * maxEntries);

	char eofReached = 0, newlineFound = 0, next;
	int scanResult;

	char *currentString = malloc(sizeof(char)*maxStringSize);;
	int stringPosition;

	int quotationMarksFound = 0;

	int currentEntryPosition = 0;
	int currentValuePosition = 0;
	struct entry *currentEntry;

	//Loop through each line until end of file reached.
	while (!eofReached) {
		newlineFound = 0, stringPosition = 0, quotationMarksFound = 0;

		//For each line, a new entry will be created (with an array of value pointers).
		currentEntry = malloc(sizeof(struct entry));
		currentEntry -> values = malloc(sizeof(union value) * columns);

		//Loop through each character within a line until line break or end of file reached.
		while (!newlineFound && !eofReached) {
			scanResult = scanf("%c", &next);
			if (scanResult == EOF) {
				eofReached = 1;
				currentString[stringPosition++] = '\0';
				if (strlen(currentString) == 0) {
					break;
				}
				if (columnTypes[currentValuePosition] == string) {
					currentEntry -> values[currentValuePosition++].stringVal = currentString;
				} else if (columnTypes[currentValuePosition] == integer) {
					currentEntry -> values[currentValuePosition++].intVal = atoi(currentString);
				} else if (columnTypes[currentValuePosition] == decimal) {
					currentEntry -> values[currentValuePosition++].decimalVal = atof(currentString);
				} else {
					printf("Error: Unknown column type for column number %d.", currentValuePosition);
					return NULL;
				}
				stringPosition = 0;

				currentString = malloc(sizeof(char)*maxStringSize);
				break;
			}
			if (next == '\r' || next == '\n') {
				if (strlen(currentString) == 0) {
					continue;
				}
				currentString[stringPosition++] = '\0';
				if (columnTypes[currentValuePosition] == string) {
					currentEntry -> values[currentValuePosition++].stringVal = currentString;
				} else if (columnTypes[currentValuePosition] == integer) {
					currentEntry -> values[currentValuePosition++].intVal = atoi(currentString);
				} else if (columnTypes[currentValuePosition] == decimal) {
					currentEntry -> values[currentValuePosition++].decimalVal = atof(currentString);
				} else {
					printf("Error: Unknown column type for column number %d.", currentValuePosition);
					return NULL;
				}
				stringPosition = 0;


				currentString = malloc(sizeof(char)*maxStringSize);
				newlineFound = 1;
				break;
			} else if (next == '"') {
				//If quotation marks found, ignore any commas until next quotation mark found.
				quotationMarksFound = !quotationMarksFound;
			} else if (next == ',' && !quotationMarksFound) {
				currentString[stringPosition++] = '\0';
				if (columnTypes[currentValuePosition] == string) {
					currentEntry -> values[currentValuePosition++].stringVal = currentString;
				} else if (columnTypes[currentValuePosition] == integer) {
					currentEntry -> values[currentValuePosition++].intVal = atoi(currentString);
				} else if (columnTypes[currentValuePosition] == decimal) {
					currentEntry -> values[currentValuePosition++].decimalVal = atof(currentString);
				} else {
					printf("Error: Unknown column type for column number %d.", currentValuePosition);
					return NULL;
				}

				//For now, if the CSV file has too many columns in a single row, ignore the rest of that row.
				//Should be fixed when quotation marks are handled.
				if (currentValuePosition > columns) {
					newlineFound = 1;
				}
				stringPosition = 0;	

				currentString = malloc(sizeof(char)*maxStringSize);
			} else {
				currentString[stringPosition++] = next;
			}
		}
		ret[currentEntryPosition++] = currentEntry;

		currentEntry = malloc(sizeof(struct entry));
		currentEntry -> values = malloc(sizeof(union value) * columns);
		currentValuePosition = 0;
	}

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

///Debugging method to print values from the CSV.
void printRange(struct csv *csv, int fromRow, int toRow, int columnNumber) {
//NOTE: ROW 2 ON EXCEL IS CONSIDERED ROW 0 HERE SINCE THE FIRST ROW CONTAINS HEADER INFO AND NOT ACTUAL DATA.

	char *columnTypeAsString;

	enum type columnType = csv->columnTypes[columnNumber];
	if (columnType == string) {
		columnTypeAsString = "String";
	} else if (columnType == integer) {
		columnTypeAsString = "Integer";
	} else if (columnType == decimal) {
		columnTypeAsString = "Decimal";
	}

	printf("Printing Rows %d to %d.\n", fromRow, toRow);
	printf("Column Number: %d\n", columnNumber);
	printf("Column Name: %s\n", csv->columnNames[columnNumber]);
	printf("Column Type: %s\n", columnTypeAsString);

	int rowCounter;
	for (rowCounter = fromRow; rowCounter <= toRow; rowCounter++) {
		if (columnType == string) {
			printf("Row %d: %s\n", rowCounter, csv->entries[rowCounter]->values[columnNumber].stringVal);
		} else if (columnType == integer) {
			printf("Row %d: %ld\n", rowCounter, csv->entries[rowCounter]->values[columnNumber].intVal);
		} else if (columnType == decimal) {
			printf("Row %d: %f\n", rowCounter, csv->entries[rowCounter]->values[columnNumber].decimalVal);
		}
		
	}

}

void mergesortMovieList(struct csv *csv, char *query) {
	return;
}

void printMovieList(struct csv *csv) {
	return;
}

///Frees CSV struct pointer for future usage.
void freeCSV(struct csv *csv) {
	int i;

	//Free Column Types (Array of Enums)
	free(csv->columnTypes);

	//Free Each Individual Column Name (Dynamically Allocated String)
	for (i=0;i<columns;i++) {
		free(csv->columnNames[i]);
	}

	//Free Column Names (Array of Dynamically Allocated Strings)
	free(csv->columnNames);

	//Free Each Individual CSV Entry (Array of Value Structs)
	for (i=0;i<maxEntries;i++) {
		free(csv->entries[i]);
	}

	//Free CSV Entries (Array of Entry Pointers)
	free(csv->entries);

	//Free CSV (Dynamically Allocated Struct)
	free(csv);
}

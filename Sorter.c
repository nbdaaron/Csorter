#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Sorter.h"
#include <unistd.h>

int main(int argc, char **argv) {

	struct csv *csv = parseCSV();
	char *sortBy = argv[2];

	printf("Sorting by: %s\n", sortBy);
	
	int compareIndex = mergesortMovieList(csv, sortBy, csv->columnTypes);
	printMovieList(csv, compareIndex);

	//Debug Command to test CSV Parser.
	//printRange(csv, 1505, 1515, 26);

	freeCSV(csv);
	printf("Program completed!\n");
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


	struct entryInfo entryInfo = getCSVEntries(ret->columnTypes);
	ret->entries = entryInfo.entries;
	ret->numEntries = entryInfo.numEntries;

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
				scanf("\r");
				scanf("\n");
				newlineFound = 1;
				break;
			}
			currentInput = addCharacterToString(currentInput, nextChar, stringPosition++);
		}
		//Add null-terminating 0 to end of String.
		currentInput = addCharacterToString(currentInput, '\0', stringPosition);
		types[retPosition] = getTypeFromColumnName(currentInput);
		columnNames[retPosition] = currentInput;

		retPosition++;
	}

	ret.columnNames = columnNames;
	ret.types = types;

	return ret;
}

struct entryInfo getCSVEntries(enum type *columnTypes) {

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
				if (stringPosition == 0) {
					break;
				}
				currentString = addCharacterToString(currentString, '\0', stringPosition);

				setValue(&(currentEntry -> values[currentValuePosition]), currentString, columnTypes[currentValuePosition]);
				currentValuePosition++;

				stringPosition = 0;

				currentString = malloc(sizeof(char)*maxStringSize);
				break;
			}
			if (next == '\r' || next == '\n') {
				scanf("\r");
				scanf("\n");
				if (stringPosition == 0) {
					continue;
				}
				addCharacterToString(currentString, '\0', stringPosition);
				setValue(&(currentEntry -> values[currentValuePosition]), currentString, columnTypes[currentValuePosition]);
				currentValuePosition++;

				stringPosition = 0;


				currentString = malloc(sizeof(char)*maxStringSize);
				newlineFound = 1;
				break;
			} else if (next == '"') {
				//If quotation marks found, ignore any commas until next quotation mark found.
				quotationMarksFound = !quotationMarksFound;
			} else if (next == ',' && !quotationMarksFound) {
				addCharacterToString(currentString, '\0', stringPosition);
				setValue(&(currentEntry -> values[currentValuePosition]), currentString, columnTypes[currentValuePosition]);
				currentValuePosition++;

				//For now, if the CSV file has too many columns in a single row, ignore the rest of that row.
				//Should be fixed when quotation marks are handled.
				if (currentValuePosition > columns) {
					newlineFound = 1;
				}
				stringPosition = 0;	

				currentString = malloc(sizeof(char)*maxStringSize);
			} else {
				addCharacterToString(currentString, next, stringPosition++);
			}
		}

		ret = addEntryToArray(ret, currentEntry, currentEntryPosition++);

		currentEntry = malloc(sizeof(struct entry));
		currentEntry -> values = malloc(sizeof(union value) * columns);
		currentValuePosition = 0;
	}

	struct entryInfo ei;
	ei.entries = ret;
	ei.numEntries = currentEntryPosition;

	return ei;
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
	} else {
		printf("Error: Unknown type printing range.");
		return;
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

int mergesortMovieList(struct csv *csv, char *query, enum type *columnTypes) {
	//implement
	// 1. parse the char *query string
	// 2. find the query string in the struct csv pointer->char **columnNames, return the indexed location
	// 3. sort the struct entry **entries
	// 4. sort the rows of entries, but compare using the double dereferenced entries at indexed location with strcom()
	
	char **columnNames = csv->columnNames; //array of strings
// int columnLength = sizeof(columnNames)/sizeof(*columnNames);
	
	int i; //i is the index to be sorted upon
	for (i=0; i < columns; i++){
		if (strcmp(columnNames[i], query)==0) {
			break;
		}
	}
	printf("COLUMN THAT MATCHES SEARCH%d\n",i);
	//check if header is found
	if (i == columns){
		printf("Error, could not find query in column names\n");
		exit(0);
	}
	
	// split arrays in half
	// call splitArray on both
	// 
	
	struct entry** entries = csv->entries;
	long low = 0;
	long high = csv->numEntries-1-1;
	printf("sizeof(entries)%d\n", csv->numEntries-1);
	//return i;
	MergeSort(low, high, entries, i, columnTypes); //entries is a pointer to the array of pointers 
	
	return i;
}

void MergeSort(long low, long high, struct entry** entries, int compareIndex, enum type *columnTypes){
	printf("%ld,%ld\n", low, high);
	if (low < high){
		//only manipulate "pointers"
		//sleep(1);
		MergeSort(low, ((low+high)/2), entries, compareIndex, columnTypes);
		MergeSort(((low+high)/2)+1, high, entries, compareIndex, columnTypes);
		MergeParts(low, high, entries, compareIndex, columnTypes);
	}
	return;
}

void MergeParts(long low, long high, struct entry** entries, int compareIndex, enum type *columnTypes){
	printf("DEBUG, MERGE CALLED, low, high%ld%ld\n", low, high);
	//take two sorted arrays, merge them together
	//how do you put two adjacent, sorted arrays together
	// (low+high)/2 is part of the lower array
	long  mid = (low+high)/2; //mid is part of the "lower" array
	
	//dynamically create an array of pointers for the next loop
	struct entry **tempArray1;
	struct entry **tempArray2;

	tempArray1 = malloc(sizeof(struct entry *)*(mid-low+1)); //allocate memory for the number of structs the lower array has
	tempArray2 = malloc(sizeof(struct entry *)*(high-mid));
	int i;
	for (i=0; i<mid-low+1; i++){
		tempArray1[i] = entries[low+i];
	}
	for (i=mid-low+1; i<high; i++){
		tempArray2[i] = entries[low+i];
	}
	//check if memory was allocated
	if (tempArray1==NULL || tempArray2==NULL){ //since 0 is false, tempArray1 will be 0 if malloc fails
		printf("Error in allocation of memory\n");
		exit(0);
	}
	printf("Flag 2\n");
	// pairwise comparisons and reordering
	long insertLocation = low;
	long index1 = low; //for the first temporary array
	long index2 = mid+1; //for the second temporary array
	//index2 goes up to and including high
	while (index1 <= mid && index2 <= high) { //the lower array gets the middle element
		printf("Flag 3--, Comparison Below, index1, index2 %ld%ld\n", index1, index2);
		printf("%s\n", (entries[index2]->values[compareIndex]).stringVal);
		printf("%s\n", (tempArray1[index1]->values[compareIndex]).stringVal);
		if (compareValue(&(tempArray1[index1]->values[compareIndex]),&(entries[index2]->values[compareIndex]),columnTypes[compareIndex])==-1) {
			//if the lower list has the smaller value
			entries[insertLocation] = tempArray1[index1-low];
			index1++;
		} else { //if the higher list has the smalller value
			entries[insertLocation] = tempArray2[index2-(mid+1)];
			index2++;
		}
		insertLocation++;
	}
	printf("Flag 4\n");
	
	//check if LOWER!! list has extra entries left, append to end
	while (index1 <= mid) {
		entries[insertLocation] = tempArray1[index1-low];
		index1++;
		insertLocation++;
	}

	while (index2 <= high) {
		entries[insertLocation] = tempArray2[index2-(mid+1)];
		index2++;
		insertLocation++;
	}

	//dont need to check if higher is there or not because it's already there
	
	//DONT FORGET TO FREE THE MALLOCED ARRAY
	free(tempArray1);
	free(tempArray2);
	printf("Flag 6\n");
	
	return;
} 

int compareValue(union value *location1, union value *location2, enum type dataType) {
	if (dataType == string) {
		printf("String: %s%s\n", location1->stringVal, location2->stringVal);
		if (strcmp(location1->stringVal,location2->stringVal)<0) {
			printf("-1\n");
			return -1; //first value is smaller
		}
	} else if (dataType == integer) {
		printf("int: %ld%ld\n", location1->intVal, location2->intVal);
		if ((location1->intVal) - (location2->intVal)<0) {
			printf("-1\n");
			return -1; //first value is smaller
		}
	} else if (dataType == decimal) {
		printf("decimal: %f%f\n", location1->decimalVal, location2->decimalVal);
		if ((location1->decimalVal) - (location2->decimalVal)<0) {
			printf("-1\n");
			return -1; //first value is smaller
		}
	} else {
		printf("Error: compareValue\n");
	}
	printf("1\n");
	return 1; //first value is bigger
}

void setValue(union value *location, char *value, enum type dataType) {
	if (dataType == string) {
		location->stringVal = value;
	} else if (dataType == integer) {
		location->intVal = atoi(value);
	} else if (dataType == decimal) {
		location->decimalVal = atof(value);
	} else {
		printf("Error: Unknown column type for value: %s.\n", value);
	}
}

void printMovieList(struct csv *csv, int compareIndex) {
	struct entry** entries = csv->entries;
	long size = csv->numEntries-1;
	int i;
	//printf("compareIndex=%d\n",compareIndex);
	for (i=0; i<size; i++){
		printf("%s\n", ((entries[i]->values)+compareIndex)->stringVal);
	}
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

char *addCharacterToString(char *string, char next, int position){
	if (position >= maxStringSize) {
		maxStringSize *= 10;
		string = realloc(string, sizeof(char) * maxStringSize);
	}
	string[position] = next;

	return string;
}

struct entry **addEntryToArray(struct entry **array, struct entry *entry, int position) {
	if (position >= maxEntries) {
		maxEntries *= 10;
		array = realloc(array, sizeof(struct entry) * maxEntries);
	}

	array[position] = entry;

	return array;
}

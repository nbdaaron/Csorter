#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SorterEC1.h"

int main(int argc, char **argv) {
	
	//struct csv takes in the whole csv file
	struct csv *csv = parseCSV();
	


	//sorts csv by sortBy
	mergesortMovieList(csv, "duration", csv->columnTypes);
	//prints out the whole csv in sorted order

	int numEntries = csv->numEntries;
	int movieNameColumn = getPositionFromColumnName("movie_title", csv);
	int durationColumn = getPositionFromColumnName("duration", csv);

	long int totalDuration = 0;
	int totalMoviesCounted = 0;
	int i;
	for (i=0;i<numEntries;i++) {
		if (csv->entries[i]->values[durationColumn].intVal > 0) {
			totalDuration += csv->entries[i]->values[durationColumn].intVal;
			totalMoviesCounted++;
		}
	}

	printf("Interesting Facts:\n");
	printf("The average movie is %ld minutes long! (Out of %d movies)\n", totalDuration / totalMoviesCounted, totalMoviesCounted);
	printf("The longest movie is \"%s\", which lasts %ld minutes!!!\n", csv->entries[numEntries-1]->values[movieNameColumn].stringVal, csv->entries[numEntries-1]->values[durationColumn].intVal);


	freeCSV(csv);
	return 0;
}

///Parses CSV and returns a pointer to CSV.
struct csv *parseCSV() {

	//Pointer to CSV file that will be returned.
	struct csv *ret = malloc(sizeof(struct csv));

	//Retrieve header info and populate CSV with values.
	struct headerInfo headerInfo = getHeaderInfo();
	ret->columnTypes = headerInfo.types;
	ret->columnNames = headerInfo.columnNames;

	//populate entries and total number of entries
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

///Retrieves CSV Entries through scanf and returns array of entries as well as entry count.
struct entryInfo getCSVEntries(enum type *columnTypes) {

	//Return value: Array of Entry Pointers.
	struct entry **ret = malloc(sizeof(struct entry *) * maxEntries);

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
		//printf("Current Entry: %d\n", currentEntryPosition);
	}

	struct entryInfo ei;
	ei.entries = ret;
	ei.numEntries = currentEntryPosition - 1;

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

void mergesortMovieList(struct csv *csv, char *query, enum type *columnTypes) {
	//array of strings
	char **columnNames = csv->columnNames; 
	
	//find the index of the desired field to sort by; color = 0, director_name = 1 ...
	int i; 
	for (i=0; i < columns; i++){
		if (strcmp(columnNames[i], query)==0) {
			break;
		}
	}
	
	//check if header is found
	if (i == columns){
		printf("Error, could not find query in column names\n");
		exit(0);
	}
	
	struct entry** entries = csv->entries;
	long low = 0;
	//numEntries includes the labels row (-1), to use Array indicies (-1)
	long high = csv->numEntries-1;
	
	//start mergeSort
	MergeSort(low, high, entries, i, columnTypes);
}

void MergeSort(long low, long high, struct entry** entries, int compareIndex, enum type *columnTypes){
	//split up array until single blocks are made
	if (low < high){
		//lower array has the "mid" element
		MergeSort(low, ((low+high)/2), entries, compareIndex, columnTypes);
		MergeSort(((low+high)/2)+1, high, entries, compareIndex, columnTypes);
		MergeParts(low, high, entries, compareIndex, columnTypes);
	}
	return;
}

void MergeParts(long low, long high, struct entry** entries, int compareIndex, enum type *columnTypes){
	// (low+high)/2 is part of the lower array
	long  mid = (low+high)/2;
	
	//dynamically create an array of pointers
	struct entry **tempArray1;
	struct entry **tempArray2;
	
	//copy the pointers from entries into tempArray1 and tempArray2
	tempArray1 = malloc(sizeof(struct entry *)*(mid-low+1)); 
	tempArray2 = malloc(sizeof(struct entry *)*(high-mid));
	int i;
	for (i=0; i<mid-low+1; i++){
		tempArray1[i] = entries[low+i];
	}
	for (i=0; i<high-mid; i++){
		tempArray2[i] = entries[mid+1+i];
	}
	
	//check if memory was not properly allocated by malloc
	if (tempArray1==NULL || tempArray2==NULL){
		printf("Error in allocation of memory\n");
		exit(0);
	}
	
	// insertLocation is the location in entries that will be overwritten
	long insertLocation = low;
	//for the first temporary array
	long index1 = low; 
	//for the second temporary array
	long index2 = mid+1; 
	while (index1 <= mid && index2 <= high) { //the lower array gets the middle element
		//compare succeeding elements in tempArray1 vs succeeding elements in tempArray2
		//dereference tempArray(1,2) at an index, dereference and grab values, dereference and grab string, decimal, or float value
		//compareValue returns -1 when element in tempArray1 is smaller and 1 whenelement in tempArray2 is bigger
		if (compareValue(&(tempArray1[index1-low]->values[compareIndex]),&(tempArray2[index2-(mid+1)]->values[compareIndex]),columnTypes[compareIndex])==-1) {
			//if tempArray1 has the smaller value or they're equal: this makes merge sort stable
			entries[insertLocation] = tempArray1[index1-low];
			index1++;
		} else { //if tempArray2  has the smalller value
			entries[insertLocation] = tempArray2[index2-(mid+1)];
			index2++;
		}
		//insertLocation will never go out of bounds because the loop stops when all the values put back into entries from low to high 
		insertLocation++;
	}
	
	//if tempArray1 still has more entries, put at the end of entries
	while (index1 <= mid) {
		entries[insertLocation] = tempArray1[index1-low];
		index1++;
		insertLocation++;
	}

	//if tempArray2 still has more entries, put at the end of entries
	while (index2 <= high) {
		entries[insertLocation] = tempArray2[index2-(mid+1)];
		index2++;
		insertLocation++;
	}
	
	free(tempArray1);
	free(tempArray2);
	
	return;
} 

int compareValue(union value *location1, union value *location2, enum type dataType) {
	//the values could be string, integer, or decimal
	if (dataType == string) {
		if (strcmp(location1->stringVal,location2->stringVal)<=0) {
			return -1; //first value is smaller or equal
		}
	} else if (dataType == integer) {
		if ((location1->intVal) - (location2->intVal)<=0) {
			return -1; //first value is smaller or equal
		}
	} else if (dataType == decimal) {
		if ((location1->decimalVal) - (location2->decimalVal)<=0) {
			return -1; //first value is smaller or equal
		}
	} else {
		printf("Error: compareValue\n");
	}
	return 1; //first value is bigger
}

void printSortedColumn(struct csv *csv, int compareIndex) {
	struct entry** entries = csv->entries;
	long size = csv->numEntries;
	int i;
	for (i=0; i<size; i++){
		printf("%s\n", (entries[i]->values[compareIndex]).stringVal);
	}
	return;
}

void printCSV(struct csv *csv) {
	struct entry** entries = csv->entries;
	long size = csv->numEntries;
	int i;
	int j;
	for (i=0;i<columns;i++) {
		if (i>0) {
			printf(",");
		}
		printf("%s", csv->columnNames[i]);
	}
	printf("\n");
	
	for (i=0; i<size; i++){
		for (j=0; j<columns; j++) {
			if (j>0) {
				printf(",");
			}
			enum type columnType = csv->columnTypes[j];
			if (columnType == string) {
				printf("\"%s\"", entries[i]->values[j].stringVal);
			} else if (columnType == integer) {
				printf("%ld", entries[i]->values[j].intVal);
			} else if (columnType == decimal) {
				printf("%f", entries[i]->values[j].decimalVal);
			}
		}
		printf("\n");
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

int getPositionFromColumnName(char *string, struct csv *csv) {
	int i;
	for (i=0;i<columns;i++) {
		if (strcmp(csv->columnNames[i], string) == 0) {
			return i;
		}
	}
	return -1;
}

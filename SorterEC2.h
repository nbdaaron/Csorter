//Maximum allowed number of columns for input through scanf.
unsigned int maxColumns = 100;

//Maximum allowed number of characters for input through scanf.
unsigned int maxStringSize = 5000;

//Maximum allowed number of movies for input through scanf.
unsigned int maxEntries = 10000;


//unions and structs
union value {
	long intVal;
	double decimalVal;
	char *stringVal;
};

struct entry {
	union value *values;
};

struct csv {
	char **columnNames;
	enum type *columnTypes;
	struct entry **entries;
	int numEntries;
	int numColumns;
};

enum type {
	string,
	integer,
	decimal,
	error = -1
};

struct headerInfo {
	enum type *types;
	char **columnNames;
	int numColumns;
};

struct entryInfo {
	struct entry **entries;
	int numEntries;
};


//CSV parsing methods
struct csv *parseCSV();
struct headerInfo getHeaderInfo();
struct entryInfo getCSVEntries(enum type *columnTypes);
enum type getTypeFromColumnName(char *name);

//Debugging Methods
void printRange(struct csv *csv, int fromRow, int toRow, int columnNumber);

//Sorting method: setup variables
void mergesortMovieList(struct csv *csv, char *query, enum type *columnTypes);

//Sorting method: recursive call, splits up array
void MergeSort(long low, long high, struct entry** entries, int compareIndex, enum type *columnTypes);

//Sorting method: regrouping
void MergeParts(long low, long high, struct entry** entries, int compareIndex, enum type *columnTypes);

//Comparing Values in an entry
int compareValue(union value *location1, union value *location2, enum type dataType);

//Output methods
void printSortedColumn(struct csv *csv, int compareIndex);
void printCSV(struct csv *csv);

//Cleanup methods
void freeCSV(struct csv *csv);

//Utility Methods
char *addCharacterToString(char *string, char next, int position);
struct entry **addEntryToArray(struct entry **array, struct entry *entry, int position);
void setValue(union value *location, char *value, enum type dataType);
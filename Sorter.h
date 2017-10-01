//Constants

const char *stringValues[] = {"color", "director_name", "actor_2_name", "genres", "actor_1_name", "movie_title", "actor_3_name", "plot_keywords", "movie_imdb_link", "language", "country", "content_rating"};
const unsigned int stringValuesSize = 12;
const char *intValues[] = {"num_critic_for_reviews", "duration", "director_facebook_likes", "actor_3_facebook_likes", "actor_1_facebook_likes", "gross", "num_voted_users", "cast_total_facebook_likes", "facenumber_in_poster", "num_user_for_reviews", "budget", "title_year", "actor_2_facebook_likes", "movie_facebook_likes"};
const unsigned int intValuesSize = 14;
const char *doubleValues[] = {"imdb_score", "aspect_ratio"};
const unsigned int doubleValuesSize = 2;

//Number of columns in CSV file.
const unsigned int columns = 28;

//Maximum allowed number of characters for input through scanf.
unsigned int maxStringSize = 5000;

//Maximum allowed number of movies for input through scanf.
unsigned int maxEntries = 10000;


/*****
 * *
 * *	Define structures and function prototypes for your sorter
 * *
 * *
 * *
 * ******/


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
};

enum type {
	string,
	integer,
	decimal,
	error = -1
};

//I dont have to worry about this method (Kelvin)
struct headerInfo {
	enum type *types;
	char **columnNames;
};

struct entryInfo {
	struct entry **entries;
	int numEntries;
};


//Suggestion: define a struct that mirrors a record (row) of the data set
//
//
////Suggestion: prototype a mergesort function

//CSV parsing methods
struct csv *parseCSV();
struct headerInfo getHeaderInfo();
struct entryInfo getCSVEntries(enum type *columnTypes);
enum type getTypeFromColumnName(char *name);

//Debugging Methods
void printRange(struct csv *csv, int fromRow, int toRow, int columnNumber);

//Sorting method: setup variables
int mergesortMovieList(struct csv *csv, char *query, enum type *columnTypes);

//Sorting method: recursive call, splits up array
void MergeSort(long low, long high, struct entry** entries, int compareIndex, enum type *columnTypes);

//Sorting method: regrouping
void MergeParts(long low, long high, struct entry** entries, int compareIndex, enum type *columnTypes);

//Comparing Values in an entry
int compareValue(union value *location1, union value *location2, enum type dataType);

//Output methods
void printMovieList(struct csv *csv, int compareIndex);

void printCSV(struct csv *csv, int compareIndex);

//Cleanup methods
void freeCSV(struct csv *csv);

//Utility Methods
char *addCharacterToString(char *string, char next, int position);
struct entry **addEntryToArray(struct entry **array, struct entry *entry, int position);
void setValue(union value *location, char *value, enum type dataType);
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
const unsigned int maxStringSize = 1000;

//Maximum allowed number of movies for input through scanf.
const unsigned int maxEntries = 10000000;


/*****
 * *
 * *	Define structures and function prototypes for your sorter
 * *
 * *
 * *
 * ******/


union value {
	int numVal;
	char *stringVal;
};

struct cell {
	char *columnName;
	union value val;
};

struct entry {
	struct cell *cells;
};

struct csv {
	struct entry *entries;
};

enum type {
	string,
	integer,
	decimal,
	error = -1
};



//Suggestion: define a struct that mirrors a record (row) of the data set
//
//
////Suggestion: prototype a mergesort function

struct csv *parseCSV();
enum type *getTypes();
enum type getTypeFromColumnName(char *name);
void mergesortMovieList(struct csv *list, char *query);
void printMovieList(struct csv *list);


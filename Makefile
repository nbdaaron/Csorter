all:
	gcc Sorter.c -Wall -Werror -fsanitize=address

clean:
	rm a.out

run:
	cat movie_metadata.csv | ./a.out -c color

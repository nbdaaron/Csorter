all:
	gcc Sorter.c -O3 -g

clean:
	rm a.out

run1:
	cat movie_metadata.csv | ./a.out -c color

run:
	cat smallTest.csv | ./a.out -c director_name

debug:
	gcc Sorter.c -Wall -Werror -fsanitize=address -g

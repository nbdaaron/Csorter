all:
	gcc Sorter.c -O3

clean:
	rm a.out

run:
	cat movie_metadata.csv | ./a.exe -c color

debug:
	gcc Sorter.c -Wall -Werror -fsanitize=address -g

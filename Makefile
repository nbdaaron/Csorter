all:
	gcc Sorter.c -O3 -o sorter

clean:
	rm sorter

run:
	cat movie_metadata.csv | ./sorter > results.csv -c director_name

debug:
	gcc Sorter.c -Wall -Werror -fsanitize=address -g -o sorter

ec1:
	gcc SorterEC1.c -O3 -o sorterec1

runec1:
	cat movie_metadata.csv | ./sorterec1

ec2:
	gcc SorterEC2.c -O3 -o sorterec2

runec2:
	cat movie_metadata_ec2.csv | ./sorterec2 > results_ec2.csv -c director_name
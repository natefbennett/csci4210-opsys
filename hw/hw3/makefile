all: compile1 compile2

compile1:
	gcc -o hw3-no-parallel -g -Wall -Werror -D NO_PARALLEL hw3-main.c hw3.c -pthread

compile2:
	gcc -o hw3-parallel -g -Wall -Werror -D PARALLEL hw3-main.c hw3.c -pthread

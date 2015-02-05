run:
	./demo.o

default: 
	gcc -fopenmp -o demo.o demo.c
	#gcc -o demo.o demo.c
	run

omp:
	gcc -fopenmp -o omp_test.o omp_test.c
	./omp_test.o

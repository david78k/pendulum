default: 
	gcc -fopenmp -o demo.o demo.c -lm
	./demo.o > demo.log
	tail -5 demo.log

omp:
	gcc -fopenmp -o omp_test.o omp_test.c
	./omp_test.o

demo:
	gcc -fopenmp -o demo_omp.o demo_omp.c
	./demo_omp.o

rundemo:
	./demo.o


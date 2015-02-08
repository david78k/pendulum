//#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
 
int main (int argc, char *argv[]) {
int FinalMaxSteps = 0;
int total = 10;
int bal = 0;
int i;
int balanced = 1;
  int th_id, nthreads;
  #pragma omp parallel private(th_id)
  {
    th_id = omp_get_thread_num();
    printf("Hello World from thread %d\n", th_id);
    #pragma omp barrier
    if ( th_id == 0 ) {
      nthreads = omp_get_num_threads();
      printf("There are %d threads\n",nthreads);
    }
  }

	//tic

// save statistics in log files
// record videos
#pragma omp parallel for
for (i = 0; i < total; i ++) {
        // write to file
        printf("Run %d:\n", i);
        //Cart_Pole_NN();

        if (balanced) {
                bal = bal + 1;
                //disp(['Balanced = ' num2str(bal)]);
        }
}

//toc

// report.m
printf("Final Max Steps: %d\n", FinalMaxSteps);
printf("Success rate: %f percent (%d/%d)\n", 100.0*bal/total, bal, total);


  return EXIT_SUCCESS;
}

#include <stdio.h>
#include <stdlib.h>
#define GNUPLOT "gnuplot -persist"
 
int main(int argc, char **argv)
{
        FILE *gp;
        gp = popen(GNUPLOT,"w"); /* 'gp' is the pipe descriptor */
        if (gp==NULL)
           {
             printf("Error opening pipe to GNU plot. Check if you have it! \n");
             exit(0);
           }
 
        fprintf(gp, "set terminal png\n");
        //fprintf(gp, "set samples 2000\n");
        fprintf(gp, "plot abs(sin(x))\n");
        fprintf(gp, "rep abs(cos(x))\n");

// read data from file
// 180k-fm200-sup1-sample1-r1.train

// left, right, r_hat[0], r_hat[1], h, h_dot, theta, theta_dot, force 
// 0 0 0.0113 0.0113 -0.0755 -0.0499 0.9063 0.6871 0.0000

        fclose(gp);
 
	return 0;
}

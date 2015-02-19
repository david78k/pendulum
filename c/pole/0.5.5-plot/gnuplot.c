/*
  v0.1 - 2/18/2015 @author Tae Seung Kang 

  Changelog
  - multiplot

  Todo list
  -
*/
#include <stdio.h>
#include <stdlib.h>

#define GNUPLOT "gnuplot -persist"
#define sample_size 500 

FILE *gp;
char *prefix = "180k-test1";
char *fname = "180k-fm200-sup1-sample1-r1.test1";
//char *fname = "180k-fm200-sup1-sample1-r1.train";
//char *prefix = "180k-train";
int sample_period = 100; // 1, 10, 100

//int sample_size = 500;
int lastlines = 180000 - sample_size;
char output[30];

// sample_loc: first -1, last 0, all 1
//void plot(int sample_loc, int col) {
void plot(int col) {
	char *colstr;
	char *type = " with lines ";
	switch(col) {
		case 1: colstr = "spikes"; type = ",\\"; break;
		//case 2: colstr = "R"; type = ""; break;
		case 3: colstr = "rhat_0"; break;
		case 4: colstr = "rhat_1"; break;
		case 5: colstr = "theta"; break;
		case 6: colstr = "theta_dot"; break;
		case 7: colstr = "h"; break;
		case 8: colstr = "h_dot"; break;
		case 9: colstr = "force"; break;
		default: break;
	}

	sprintf(output, "%s-%s.png", prefix, colstr);

	if(col != 2)
	        fprintf(gp, "set output '%s'\n", output);
	fprintf(gp, "set multiplot layout 3, 1\n");
	if(col == 1) {
        	fprintf(gp, "set yr [0:2.4]\n");
		colstr = "L";
	} else
	        fprintf(gp, "set autoscale\n");

	// all sampled
	fprintf(gp, "set xlabel \"(x100)\"\n");
       	fprintf(gp, "plot \"%s\" every %d using %d title '%s' %s\n", fname, sample_period, col, colstr, type);
	if(col == 1) 
       		fprintf(gp, "\"%s\" every %d using ($2 * 2) title 'R'\n", fname, sample_period);
	// first steps
	fprintf(gp, "unset xlabel\n");
        fprintf(gp, "plot \"<(sed -n '1,%dp' %s)\" using %d title '%s' %s\n", sample_size, fname, col, colstr, type);
	if(col == 1) 
        	fprintf(gp, "\"<(sed -n '1,%dp' %s)\" using ($2 * 2) title 'R'\n", sample_size, fname);
	// last steps
	fprintf(gp, "set xtics %d,180000\n", lastlines);
        fprintf(gp, "plot \"<(sed -n '%d,180000p' %s)\" using %d title '%s' %s\n", lastlines, fname, col, colstr, type);
	if(col == 1) 
        	fprintf(gp, "\"<(sed -n '%d,180000p' %s)\" using ($2 * 2) title 'R'\n", lastlines, fname);
	if(col != 2) printf("%s created\n", output);
	fprintf(gp, "unset multiplot\n");
}

int main(int argc, char **argv)
{
        gp = popen(GNUPLOT,"w"); /* 'gp' is the pipe descriptor */
        if (gp==NULL)
           {
             printf("Error opening pipe to GNU plot. Check if you have it! \n");
             exit(0);
           }
 
        fprintf(gp, "set terminal png\n");
	printf("source file: %s\n", fname);
	
	// left, right, r_hat[0], r_hat[1], theta, theta_dot, h, h_dot, force 
	// 0 0 0.0113 0.0113 -0.0755 -0.0499 0.9063 0.6871 0.0000

	// L/R for first, last, sampled steps
	plot(1);
	plot(3);
	plot(4);
	plot(9);
	plot(5);
	plot(6);
	plot(7);
	plot(8);

        fclose(gp);

	return 0;
}

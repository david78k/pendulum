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
void plot(int sample_loc, int col) {
	char *colstr;
	char *type = " with lines ";
	switch(col) {
		case 1: colstr = "spikes"; type = ",\\"; break;
		//case 2: colstr = "R"; type = ""; break;
		case 5: colstr = "theta"; break;
		case 6: colstr = "thetadot"; break;
		case 9: colstr = "force"; break;
		default: break;
	}

	if(sample_loc == -1)
		sprintf(output, "%s-%s-first%d.png", prefix, colstr, sample_size);
	else if (sample_loc == 0)
		sprintf(output, "%s-%s-last%d.png", prefix, colstr, sample_size);
	else
		sprintf(output, "%s-%s.png", prefix, colstr);

	if(col != 2)
	        fprintf(gp, "set output '%s'\n", output);
	if(col == 1) {
        	fprintf(gp, "set yr [0:2.4]\n");
		colstr = "L";
	} else
	        fprintf(gp, "set autoscale\n");

	if(sample_loc == -1) {
        	fprintf(gp, "plot \"<(sed -n '1,%dp' %s)\" using %d title '%s' %s\n", sample_size, fname, col, colstr, type);
		if(col == 1) 
        		fprintf(gp, "\"<(sed -n '1,%dp' %s)\" using ($2 * 2) title 'R'\n", sample_size, fname);
	} else if (sample_loc == 0) {
        	fprintf(gp, "plot \"<(sed -n '%d,180000p' %s)\" using %d title '%s' %s\n", lastlines, fname, col, colstr, type);
		if(col == 1) 
        		fprintf(gp, "\"<(sed -n '%d,180000p' %s)\" using ($2 * 2) title 'R'\n", lastlines, fname);
	} else {
        	fprintf(gp, "plot \"%s\" every %d using %d title '%s' %s\n", fname, sample_period, col, colstr, type);
		if(col == 1) 
        		fprintf(gp, "\"%s\" every %d using ($2 * 2) title 'R'\n", fname, sample_period);
	}
	if(col != 2) printf("%s created\n", output);
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
        //fprintf(gp, "set output 'gnuplot.png'\n");
        //fprintf(gp, "set samples 2000\n"); // optional

	printf("source file: %s\n", fname);

	int lines = sample_size;
	// L/R for first steps
	plot(-1, 1);
	plot(0, 1);
	plot(1, 1);

	// rhat_L
	// rhat_R

	// force for first, last, sampled steps
	plot(-1, 9);
	plot(0, 9);
	plot(1, 9);

	// theta and thetadot for sampled steps
	plot(-1, 5);
	plot(0, 5);
	plot(1, 5);

	plot(-1, 6);
	plot(0, 6);
	plot(1, 6);
	

/*
	// rhat_L
	// rhat_R
// left, right, r_hat[0], r_hat[1], theta, theta_dot, h, h_dot, force 
// 0 0 0.0113 0.0113 -0.0755 -0.0499 0.9063 0.6871 0.0000

	// theta and thetadot for sampled steps
	sprintf(output, "180k-train-theta.png");
        fprintf(gp, "set output '%s'\n", output);
        fprintf(gp, "plot \"%s\" every 100 using 5 title 'theta' with lines\n", fname);
	printf("%s created\n", output);

	sprintf(output, "180k-train-thetadot.png");
        fprintf(gp, "set output '%s'\n", output);
        fprintf(gp, "plot \"%s\" every 100 using 6 title 'theta_dot' with lines\n", fname);
	printf("%s created\n", output);
	
	// theta for first and last steps
	sprintf(output, "180k-train-theta-first%d.png", lines);
        fprintf(gp, "set output '%s'\n", output);
        fprintf(gp, "plot \"<(sed -n '1,%dp' %s)\" using 5 title 'theta' with lines\n", lines, fname);
	printf("%s created\n", output);

	sprintf(output, "180k-train-theta-last%d.png", lines);
        fprintf(gp, "set output '%s'\n", output);
        fprintf(gp, "plot \"<(sed -n '%d,180000p' %s)\" using 5 title 'theta' with lines\n", lastlines, fname);
	printf("%s created\n", output);

	// thetadot for first and last steps
	sprintf(output, "180k-train-thetadot-first%d.png", lines);
        fprintf(gp, "set output '%s'\n", output);
        fprintf(gp, "plot \"<(sed -n '1,%dp' %s)\" using 6 title 'theta_dot' with lines\n", lines, fname);
	printf("%s created\n", output);

	sprintf(output, "180k-train-thetadot-last%d.png", lines);
        fprintf(gp, "set output '%s'\n", output);
        fprintf(gp, "plot \"<(sed -n '%d,180000p' %s)\" using 6 title 'theta_dot' with lines\n", lastlines, fname);
	printf("%s created\n", output);
*/
        fclose(gp);

	return 0;
}

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
	char *type = "lines";
	switch(col) {
		case 1: colstr = "L"; type = ""; break;
		case 2: colstr = "R"; type = ""; break;
		case 5: colstr = "theta"; break;
		case 6: colstr = "thetadot"; break;
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
	if(col == 1) 
        	fprintf(gp, "set yr [0:2.4]\n");
	
	if(sample_loc == -1)
        	fprintf(gp, "plot \"<(sed -n '1,%dp' %s)\" using %d title '%s' %s\n", sample_size, fname, col, colstr, type);
	else if (sample_loc == 0)
        	fprintf(gp, "plot \"<(sed -n '%d,180000p' %s)\" using %d title '%s' %s\n", lastlines, fname, col, colstr, type);
	else
        	fprintf(gp, "plot \"%s\" every %d using %d title '%s' %s\n", fname, sample_period, col, colstr, type);

	if(col == 1) plot(sample_loc, 2);
	if(col != 1)
		printf("%s created\n", output);
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
	// L for first steps
	plot(-1, 1);
/*
	sprintf(output, "180k-train-first%d.png", lines);
        fprintf(gp, "set output '%s'\n", output);
        fprintf(gp, "set yr [0:2.4]\n");

        fprintf(gp, "plot \"<(sed -n '1,%dp' %s)\" using 1 title 'L', \\\n", lines, fname);
        fprintf(gp, "\"<(sed -n '1,%dp' %s)\" using ($2 * 2) title 'R'\n", lines, fname);
	printf("%s created\n", output);
	// R for last steps
	sprintf(output, "180k-train-last%d.png", lines);
        fprintf(gp, "set output '%s'\n", output);

        fprintf(gp, "plot \"<(sed -n '%d,180000p' %s)\" using 1 title 'L', \\\n", lastlines, fname);
        fprintf(gp, "\"<(sed -n '%d,180000p' %s)\" using ($2 * 2) title 'R'\n", lastlines, fname);
	printf("%s created\n", output);

	// rhat_L
	// rhat_R
// left, right, r_hat[0], r_hat[1], theta, theta_dot, h, h_dot, force 
// 0 0 0.0113 0.0113 -0.0755 -0.0499 0.9063 0.6871 0.0000

	// force for first steps
        fprintf(gp, "set autoscale\n");
	sprintf(output, "180k-train-first%d-force.png", lines);
        fprintf(gp, "set output '%s'\n", output);
        fprintf(gp, "plot \"<(sed -n '1,%dp' %s)\" using 9 title 'Force' with lines\n", lines, fname);
	printf("%s created\n", output);

	// force for last steps
	sprintf(output, "180k-train-last%d-force.png", lines);
        fprintf(gp, "set output '%s'\n", output);
        fprintf(gp, "plot \"<(sed -n '%d,180000p' %s)\" using 9 title 'Force' with lines\n", lastlines, fname);
	printf("%s created\n", output);

	// force for sampled steps
	sprintf(output, "180k-train-force.png");
        fprintf(gp, "set output '%s'\n", output);
        fprintf(gp, "plot \"%s\" every 100 using 9 title 'Force' with lines\n", fname);
	printf("%s created\n", output);

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
/*
	FILE *file; // = fopen(fname);
 if ((file = fopen(fname,"r")) == NULL) {
    printf("Couldn't open %s\n",fname);
    return;
  }
	int i, j;
	int left[180000];
	int right[180000];
	float rhat[180000][2];
	float h[180000];
	float hdot[180000];
	float theta[180000];
	float thetadot[180000];
	float force[180000];
// left, right, r_hat[0], r_hat[1], h, h_dot, theta, theta_dot, force 
// 0 0 0.0113 0.0113 -0.0755 -0.0499 0.9063 0.6871 0.0000

//  for(i = 0; i < 5; i++)
	for(j = 0; j < 10; j++) {
        	fscanf(file,"%d",&left[j]);
        	fscanf(file,"%d",&right[j]);
        	fscanf(file,"%f",&rhat[j][0]);
        	fscanf(file,"%f",&rhat[j][1]);
        	fscanf(file,"%f",&h[j]);
        	fscanf(file,"%f",&hdot[j]);
        	fscanf(file,"%f",&theta[j]);
        	fscanf(file,"%f",&thetadot[j]);
        	fscanf(file,"%f",&force[j]);
		//printf("%d %d %f %f %f\n", left[j], right[j], rhat[j][0], rhat[j][1], h[j]);
	//		, h[j], hdot[j], theta[j]);
		//printf("%d %d %.4f %.4f %.4f %.4f %.4f %.4f %.4f\n", left[j], right[j], rhat[j][0], rhat[j][1]
		//	, h[j], hdot[j], theta[j], thetadot[j], force[j]);
		//fprintf(gp, "%d %d %.4f %.4f %.4f %.4f %.4f %.4f %.4f\n", left[j], right[j], rhat[j][0], rhat[j][1]
		//	, h[j], hdot[j], theta[j], thetadot[j], force[j]);
	}
*/
        //fprintf(gp, "plot (sin(x))\n");
        //fprintf(gp, "rep abs(cos(x))\n");

        //fclose(gp);

	return 0;
}

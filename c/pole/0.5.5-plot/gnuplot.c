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
        //fprintf(gp, "set output 'gnuplot.png'\n");
        //fprintf(gp, "set samples 2000\n"); // optional
        //fprintf(gp, "plot abs(sin(x))\n");
        //fprintf(gp, "rep abs(cos(x))\n");

// read data from file
// 180k-fm200-sup1-sample1-r1.train
	FILE *file; // = fopen(fname);
	//char *fname = "180k-fm200-sup1-sample1-r1-first100.train";
	//char *fname = "180k-fm200-sup1-sample1-r1.train.raw";
	char *fname = "180k-fm200-sup1-sample1-r1.train";
 if ((file = fopen(fname,"r")) == NULL) {
    printf("Couldn't open %s\n",fname);
    return;
  }

	int lines = 500;
	int lastlines = 180000 - lines;
	//char *output = "180k-train-first500.png";
	char output[30];

	// first steps
	sprintf(output, "180k-train-first%d.png", lines);

	printf("file %s opened\n", fname);
        fprintf(gp, "set output '%s'\n", output);
        fprintf(gp, "set yr [0:2.4]\n");

        fprintf(gp, "plot \"<(sed -n '1,%dp' %s)\" using 1, \\\n", lines, fname);
        fprintf(gp, "\"<(sed -n '1,%dp' %s)\" using ($2 * 2) \n", lines, fname);
	printf("%s created\n", output);
        //fclose(gp);

	// last steps
        //gp = popen(GNUPLOT,"w"); /* 'gp' is the pipe descriptor */
	sprintf(output, "180k-train-last%d.png", lines);
        //fprintf(gp, "set terminal png\n");
        fprintf(gp, "set output '%s'\n", output);
        //fprintf(gp, "set yr [0:2.4]\n");

        fprintf(gp, "plot \"<(sed -n '%d,180000p' %s)\" using 1, \\\n", lastlines, fname);
        fprintf(gp, "\"<(sed -n '%d,180000p' %s)\" using ($2 * 2) \n", lastlines, fname);
	printf("%s created\n", output);
        fclose(gp);
/*
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

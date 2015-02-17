#include <stdio.h>
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
        fprintf(gp, "set samples 2000\n");
        fprintf(gp, "plot abs(sin(x))\n");
        fprintf(gp, "rep abs(cos(x))\n");
        fclose(gp);
 
return 0;
}

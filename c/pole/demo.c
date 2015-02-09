/*********************************************************************************
    This file contains a simulation of the cart and pole dynamic system and 
 a procedure for learning to balance the pole that uses multilayer connectionist 
 networks.  Both are described in Anderson, "Strategy Learning with Multilayer
 Connectionist Representations," GTE Laboratories TR87-509.3, 1988.  This is
 a corrected version of the report published in the Proceedings of the Fourth
 International Workshop on Machine Learning, Irvine, CA, 1987.  

    main takes six arguments: 
        the maximum number of trials (balancing attempts),
        the number of trials over which balancing steps are averaged and printed
        beta
        beta for hidden units
        rho
        rho for hidden units

    Please send questions and comments to anderson@cs.colostate.edu
*********************************************************************************/
#include <stdio.h>
//#include "xg.h"
#include <math.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <stdlib.h>

#define NHIDDEN 60
#define TARGET_STEPS	180000
#define randomdef                  ((float) random() / (float)((1 << 31) - 1))

int Graphics = 0;
int Delay = 20000;

#define Mc           1.0
#define Mp           0.1
#define l            0.5
#define g            9.8
#define tau          0.02
#define max_cart_pos 2.4
#define max_cart_vel 1.5
#define max_pole_pos 0.2094
#define max_pole_vel 2.01
#define Gamma        0.9
float Beta =  0.2;
float Beta_h = 0.05;
float Rho = 1.0;
float Rho_h = 0.2;
float LR_IH = 0.7;
float LR_HO = 0.07;

float state[4] = {0.0, 0.0, 0.0, 0.0};

float cart_mass = 1.;
float pole_mass = 0.1;
float pole_half_length = 0.5;
float force_mag = 10.;
float fric_cart = 0.0005;
float fric_pole = 0.000002;
int not_first = 0;
int trial_length = 0;

struct
{
  double           cart_pos;
  double           cart_vel;
  double           pole_pos;
  double           pole_vel;
} the_system_state;

int start_state, failure;
double a[5][5], b[5], c[5], d[5][5], e[5], f[5]; //, wih[4][NHIDDEN], who[NHIDDEN], h[NHIDDEN];
double x[5], x_old[5], y[5], y_old[5], v, v_old, z[5], p;
double r_hat, push, unusualness, sum_error = 0.0;
int bp_flag = 0, count_error = 0, total_count = 0;

/*** Prototypes ***/
float scale (float v, float vmin, float vmax, int devmin, int devmax);
void init_args(int argc, char *argv[]);
void updateweights();
void readweights(char *filename);
void writeweights();

float sign(float x) { return (x < 0) ? -1. : 1.;}

main(argc,argv)
     int argc;
     char *argv[];
{
  char a;
//  Xg_context *context;

  init_args(argc,argv);

  sleep(2);

  Run(atoi(argv[2]), atoi(argv[3]));

/*
  Run(context,atoi(argv[2]), atoi(argv[3]));
  if(bp_flag) {
    bp_flag = 2; // test backprop
    Run(context,atoi(argv[2]), atoi(argv[3]));
  } 
*/
}

/**********************************************************************
 *
 **********************************************************************/
void init_args(int argc, char *argv[])
{
  int runtimes;
  time_t tloc;
  time_t time();

  printf("Usage: %s g(raphics) num-trials trials-per-output \
weight-file(or - to init randomly) b bh r rh\n",
	 argv[0]);

  srandom((int)time(&tloc));

  if (argc < 5)
    exit(-1);
  if (strcmp(argv[4],"-") != 0)
    readweights(argv[4]);
  else
    SetRandomWeights();
  if (argc > 5)
    Beta = atof(argv[5]);
  if (argc > 6)
    Beta_h = atof(argv[6]);
  if (argc > 7)
    Rho = atof(argv[7]);
  if (argc > 8)
    Rho_h = atof(argv[8]);

}

SetRandomWeights()
{
  int i,j;

  for(i = 0; i < 5; i++)
    {
      for(j = 0; j < 5; j++)
	{
	  a[i][j] = randomdef * 0.2 - 0.1;
	  d[i][j] = randomdef * 0.2 - 0.1;
	}
      b[i] = randomdef * 0.2 - 0.1;
      c[i] = randomdef * 0.2 - 0.1;
      e[i] = randomdef * 0.2 - 0.1;
      f[i] = randomdef * 0.2 - 0.1;
    }
}

/****************************************************************/

/* If init_flag is zero, then calculate state of cart-pole system at time t+1
   by Euler's method, else set state of cart-pole system to random values.
*/

NextState(init_flag, push)
     int init_flag;
     double push;
{
  register double pv, ca, pp, pa, common;
  double sin_pp, cos_pp;
  
  if (init_flag)
    {
      the_system_state.cart_pos = randomdef * 2 * max_cart_pos - max_cart_pos;
      the_system_state.cart_vel = randomdef * 2 * max_cart_vel - max_cart_vel;
      the_system_state.pole_pos = randomdef * 2 * max_pole_pos - max_pole_pos;
      the_system_state.pole_vel = randomdef * 2 * max_pole_vel - max_pole_vel;

      start_state = 1;
      SetInputValues();
      failure = 0;
    }
  else
    {
      pv = the_system_state.pole_vel;
      pp = the_system_state.pole_pos;
  
      sin_pp = sin(pp);
      cos_pp = cos(pp);

      common = (push + Mp * l * pv * pv * sin_pp) / (Mc + Mp);
      pa = (g * sin_pp - cos_pp * common) /
        (l * (4.0 / 3.0 - Mp * cos_pp * cos_pp / (Mc + Mp)));
      ca = common - Mp * l * pa * cos_pp / (Mc + Mp);
  
      the_system_state.cart_pos += tau * the_system_state.cart_vel;
      the_system_state.cart_vel += tau * ca;
      the_system_state.pole_pos += tau * the_system_state.pole_vel;
      the_system_state.pole_vel += tau * pa;

      SetInputValues();

      start_state = 0;
      if ((fabs(the_system_state.cart_pos) > max_cart_pos) ||
	  (fabs(the_system_state.pole_pos) > max_pole_pos))
	failure = -1;
      else
	failure = 0;
    }
}

/****************************************************************/

SetInputValues()
{
  x[0] = (the_system_state.cart_pos + max_cart_pos) / (2 * max_cart_pos);
  x[1] = (the_system_state.cart_vel + max_cart_vel) / (2 * max_cart_vel);
  x[2] = (the_system_state.pole_pos + max_pole_pos) / (2 * max_pole_pos);
  x[3] = (the_system_state.pole_vel + max_pole_vel) / (2 * max_pole_vel);
  x[4] = 0.5;
}

/****************************************************************/

Run(num_trials, sample_period)
 int num_trials, sample_period;
{
  register int i, j, avg_length;

  sum_error = 0; count_error = 0; total_count = 0;

  NextState(1, 0.0);
  i = 0;
  j = 0;
  avg_length = 0;

  printf(" B= %g Bh= %g R= %g Rh= %g nt= %d bin= %d\n",
      Beta,Beta_h,Rho,Rho_h,num_trials,sample_period);

  //while (i < num_trials && j < 180000 && total_count < 2000) /* one hour at .02s per step */
  while (i < num_trials && j < TARGET_STEPS) /* one hour at .02s per step */
    {
      Cycle(1);
      j++;

      if (failure)
	{
	  avg_length += j;
	  i++;
	  if (!(i % sample_period))
	    {
 	      if(bp_flag == 0) 
	        printf("%6d %6d\n", i, avg_length / sample_period);
/*
      	      if(i % 100 == 0)
		printf("%6d %6d %.4f (%.1f/%6d)\n", i, avg_length / sample_period, sum_error / count_error, sum_error, count_error);
*/
	      avg_length = 0;
	    }
	  NextState(1, 0.0);
	  j = 0;
	}
    }
   printf("Final trial %d balanced for %d steps (%f hours).\n",
          i, j, (j * tau)/3600.0);

  if(bp_flag == 0)
    writeweights();

}

/****************************************************************/

double sgn(x)
     double x;
{
  if (x < 0.0)
    return -1.0;
  else if (x > 0.0)
    return 1.0;
  else
    return 0.0;
}

/****************************************************************/

Cycle(learn_flag)
     int learn_flag;
{
  int i, j, k;
  double sum, factor1, factor2, targetp;
  extern double exp();
  float state[4];

  /* output: state evaluation */
  for(i = 0; i < 5; i++)
    {
      sum = 0.0;
      for(j = 0; j < 5; j++)
	{
	  sum += a[i][j] * x[j];
	}
      y[i] = 1.0 / (1.0 + exp(-sum));
    }
  sum = 0.0;
  for(i = 0; i < 5; i++)
    {
      sum += b[i] * x[i] + c[i] * y[i];
    }
  v = sum;

  /* output: action */
  for(i = 0; i < 5; i++)
    {
      sum = 0.0;
      for (j = 0; j < 5; j++)
	sum += d[i][j] * x[j];
      z[i] = 1.0 / (1.0 + exp(-sum));
    }
  sum = 0.0;
  for(i = 0; i < 5; i++)
    sum += e[i] * x[i] + f[i] * z[i];
  p = 1.0 / (1.0 + exp(-sum));

  //push = (0.5 <= p) ? 10.0 : -10.0;
  push = (randomdef <= p) ? 10.0 : -10.0;
  unusualness = (push > 0) ? 1.0 - p : -p;

  /* preserve current activities in evaluation network. */
  v_old = v;

  for (i = 0; i< 5; i++)
  {
    x_old[i] = x[i];
    y_old[i] = y[i];
  }

  /* Apply the push to the pole-cart */
  NextState(0, push);

  /* Calculate evaluation of new state. */
  for(i = 0; i < 5; i++)
    {
      sum = 0.0;
      for(j = 0; j < 5; j++)
	{
	  sum += a[i][j] * x[j];
	}
      y[i] = 1.0 / (1.0 + exp(-sum));
    }
  sum = 0.0;
  for(i = 0; i < 5; i++)
    {
      sum += b[i] * x[i] + c[i] * y[i];
    }
  v = sum;

  /* action evaluation */
  if (start_state)
    r_hat = 0.0;
  else
    if (failure)
      r_hat = failure - v_old;
    else
      r_hat = failure + Gamma * v - v_old;

  /* modification */
  if (learn_flag)
    {
	updateweights();
   }
}

/**********************************************************************/
void updateweights() {
  int i, j;
  double factor1, factor2;
      for(i = 0; i < 5; i++)
	{
	  factor1 = Beta_h * r_hat * y_old[i] * (1.0 - y_old[i]) * sgn(c[i]);
	  factor2 = Rho_h * r_hat * z[i] * (1.0 - z[i]) * sgn(f[i]) *
	    unusualness;
	  for(j = 0; j < 5; j++)
	    {
	      a[i][j] += factor1 * x_old[j];
	      d[i][j] += factor2 * x_old[j];
	    }
	  b[i] += Beta * r_hat * x_old[i];
	  c[i] += Beta * r_hat * y_old[i];
	  e[i] += Rho * r_hat * unusualness * x_old[i];
	  f[i] += Rho * r_hat * unusualness * z[i];
	}
    }

/**********************************************************************/

void readweights(filename)
char *filename;
{
  int i,j;
  FILE *file;

  if ((file = fopen(filename,"r")) == NULL) {
    printf("Couldn't open %s\n",filename);
    return;
  }

  for(i = 0; i < 5; i++)
      for(j = 0; j < 5; j++)
	  fscanf(file,"%lf",&a[i][j]);

  for(i = 0; i < 5; i++)
    fscanf(file,"%lf",&b[i]);

  for(i = 0; i < 5; i++)
    fscanf(file,"%lf",&c[i]);


  for(i = 0; i < 5; i++)
      for(j = 0; j < 5; j++)
	  fscanf(file,"%lf",&d[i][j]);

  for(i = 0; i < 5; i++)
    fscanf(file,"%lf",&e[i]);

  for(i = 0; i < 5; i++)
    fscanf(file,"%lf",&f[i]);

  fclose(file);

  printf("Read weights from %s\n",filename);
}


void writeweights()
{
  char *filename = "latest.weights";
  int i,j;
  FILE *file;

  if ((file = fopen(filename,"w")) == NULL) {
    printf("Couldn't open %s\n",filename);
    return;
  }

  for(i = 0; i < 5; i++)
      for(j = 0; j < 5; j++)
	  fprintf(file," %f",a[i][j]);

  fprintf(file, "\n");

  for(i = 0; i < 5; i++)
    fprintf(file," %f",b[i]);

  fprintf(file, "\n");

  for(i = 0; i < 5; i++)
    fprintf(file," %f",c[i]);

  fprintf(file, "\n");

  for(i = 0; i < 5; i++)
      for(j = 0; j < 5; j++)
	  fprintf(file," %f",d[i][j]);

  fprintf(file, "\n");

  for(i = 0; i < 5; i++)
    fprintf(file," %f",e[i]);

  fprintf(file, "\n");

  for(i = 0; i < 5; i++)
    fprintf(file," %f",f[i]);

  fclose(file);

  printf("Wrote current weights to %s\n",filename);
}

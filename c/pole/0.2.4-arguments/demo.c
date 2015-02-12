/* 
   v0.2.4 - 2/12/2015
   Changelog
   - change the input arguments to take TEST_RUNS, TARGET_STEPS, DEBUG, LAST_STEPS
   - tic-toc elapsed time
   - 1output with 2actions(L/R)
   - test runs only when balanced
   - continuous force
   - taking only the last few steps doesn't work
     limits the number of steps balanced: 1k->1k, 10k->10k
   - 494 runs take 2h 36min for 180k steps

   Todo list
   - find best Fmax among 1 to 10. Fmax over 10 is no good
   - Get last steps working
   - 1output with 2actions(L/R) to 2outputs(L/R) with 3actions L/R/0
*/
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
//#include <sys/timeb.h>
#include <time.h>
#include <stdlib.h>

//#define DEBUG 		0
//#define TEST_RUNS	10
//#define TARGET_STEPS	50000
//#define TARGET_STEPS	18000
//#define TARGET_STEPS	180000
#define randomdef       ((float) random() / (float)((1 << 31) - 1))

int Graphics = 0;
int Delay = 20000;

#define Mc           1.0
#define Mp           0.1
#define l            0.5
#define g            9.8
#define dt          0.02
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
int balanced = 0;
int DEBUG = 0;
int TEST_RUNS = 10;
int TARGET_STEPS = 5000;

struct
{
  double           cart_pos;
  double           cart_vel;
  double           pole_pos;
  double           pole_vel;
} the_system_state;

int start_state, failure;
double a[5][5], b[5], c[5], d[5][5], e[5], f[5]; 
double x[5], x_old[5], y[5], y_old[5], v, v_old, z[5], p;
double r_hat, push, unusualness; 
int test_flag = 0, total_count = 0;

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

  setbuf(stdout, NULL);

  init_args(argc,argv);

  printf("balanced %d test_flag %d\n", balanced, test_flag);

  int i = 0;
  //if(balanced && test_flag) {
  if(test_flag) {
    int trials, sumTrials = 0, maxTrials = 1, minTrials = 100, success = 0, maxSteps = 0;
    time_t start, stop, istart, istop;
    printf("TEST_RUNS = %d\n", TEST_RUNS);
    for(i = 0; i < TEST_RUNS; i ++) {
      printf("------------- Test Run %d -------------\n", i + 1);
      //tic
      time(&start);
      init_args(argc,argv);
      trials = Run(atoi(argv[2]), atoi(argv[3]));
      sumTrials += trials;
      if(trials > maxTrials) maxTrials = trials;
      if(trials < minTrials) minTrials = trials;
      if(trials <= 100) success ++;
      //if(trials == 1) success ++;
      // toc
      time(&stop);
      printf("Elapsed time: %.0f seconds\n", difftime(stop, start));
    }
    printf("\n=============== SUMMARY ===============\n");
    printf("Trials: %.2f\% (%d/%d) avg %d max %d min %d\n", 
	100.0*success/TEST_RUNS, success, TEST_RUNS, sumTrials/TEST_RUNS, maxTrials, minTrials);
  } else { 
    i = 0;
    while(!balanced) {
      printf("Run %d: ", ++i);
      Run(atoi(argv[2]), atoi(argv[3]));
      init_args(argc,argv);
    }
  }
}

/**********************************************************************
 *
 **********************************************************************/
void init_args(int argc, char *argv[])
{
  int runtimes;
  //time_t tloc, time();
  struct timeval current;

  gettimeofday(&current, NULL);
//  printf("current time: %d\n", current.tv_usec);
/*
  printf("Usage: %s g(raphics) num-trials trials-per-output \
weight-file(or - to init randomly) b bh r rh\n",
	 argv[0]);
*/
  //srandom((int)time(&tloc));
  srandom(current.tv_usec);

  if (argc < 5)
    exit(-1);
  if (strcmp(argv[4],"-") != 0) {
    readweights(argv[4]); test_flag = 1;
  } else {
    SetRandomWeights(); test_flag = 0;
  }
  if (argc > 5)
    TEST_RUNS = atof(argv[5]);
  if (argc > 6)
    TARGET_STEPS = atof(argv[6]);
  if (argc > 7)
    DEBUG = atof(argv[7]);
/*
  if (argc > 8)
    Rho_h = atof(argv[8]);
*/
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
  
      the_system_state.cart_pos += dt * the_system_state.cart_vel;
      the_system_state.cart_vel += dt * ca;
      the_system_state.pole_pos += dt * the_system_state.pole_vel;
      the_system_state.pole_vel += dt * pa;

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
// returns the number of trials before failure
int Run(num_trials, sample_period)
 int num_trials, sample_period;
{
  register int i, j, avg_length, max_length = 0;

  total_count = 0;

  NextState(1, 0.0);
  i = 0;   j = 0;
  avg_length = 0;

//  if(!test_flag)
//    printf(" B= %g Bh= %g R= %g Rh= %g nt= %d bin= %d\n",
//        Beta,Beta_h,Rho,Rho_h,num_trials,sample_period);

  //while (i < num_trials && j < 180000 && total_count < 2000) /* one hour at .02s per step */
  while (i < num_trials && j < TARGET_STEPS) /* one hour at .02s per step */
    {
      Cycle(1, j);
      if (DEBUG && j % 1000 == 0)
        printf("Episode %d step %d rhat %.4f\n", i, j, r_hat);
      j++;

      if (failure)
	{
	  avg_length += j;
	  i++;
	  if (!(i % sample_period))
	    {
 	      if(DEBUG) 
	        printf("Episode%6d %6d\n", i, avg_length / sample_period);
	      avg_length = 0;
	    }
	  NextState(1, 0.0);
   	  max_length = (max_length < j ? j : max_length);
	  j = 0;
	}
    }
   if(i >= num_trials) {
     balanced = 0;
     printf("Trial %d not balanced. Max %d steps (%f hours).\n",
            i, max_length, (max_length * dt)/3600.0);
            //i + 1, j, (j * dt)/3600.0);
   } else {
     printf("Trial %d balanced for %d steps (%f hours).\n",
            i, j, (j * dt)/3600.0);
     balanced = 1;
   }

  if(!test_flag && balanced)
    writeweights();
  
  return i + 1;
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

Cycle(learn_flag, step)
     int learn_flag, step;
{
  int i, j, k;
  double sum, factor1, factor2, t;
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

  double q; 
  push = (randomdef <= p) ? 1.0 : -1.0;
  //push = (randomdef <= p) ? 10.0 : -10.0;
  //push = (0.67 <= p) ? 10.0 : ((0.33 <= p) ? 0: -10.0);
  //push = (0.67 <= p) ? 1.0 : ((0.33 <= p) ? -1.0:0);
/*
  if(0.6667 <= p) {
    push = 1.0; q = 1.0;
  } else if (0.3333 <= p < 0.6667) { 
    push = -1.0; q = 0.5;
  } else {
    push = 0; q = 0;
  }
*/
//  unusualness = q - p;
  //unusualness = (push > 0) ? 1.0 - p : ((push < 0) ? 0.5-p : -p);
  unusualness = (push > 0) ? 1.0 - p : -p;

  sum = 0.0;
  //for(i = 0; i < (step > 100 ? 100 : step) ; i++) {
  for(i = 0; i < step; i++) {
    t = (step - i) * dt;
    sum += t * exp(-t);
  }
  push *= sum;
 // push = (push > 0) ? 10.0 : (push < 0 ? -10.0 : 0);

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

  if(!test_flag)
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

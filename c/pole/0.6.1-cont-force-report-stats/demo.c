/* 
   v0.6.1 - 2/13/2015
   Continuous force version

   Changelog
   - bug found: pushes[step] was missing in integrating force 
     not working after fix
   - last 400 steps for force: 300(fast)-500(more accurate)
   - report stats: firing rates (L/R), rhats (L/R), state (4), force
     writes the data to a file latest.dat
   - 1output with 2actions(L/R) to 2outputs(L/R) with 3actions L/R/0
   - two outputs for both networks
   - td-backprop code for evaluation network combined: multiple outputs
   - tau added
   - changed the input arguments to take TEST_RUNS, TARGET_STEPS, DEBUG
   - tic-toc elapsed time
   - 1output with 2actions(L/R)
   - test runs only when balanced
   - taking only the last few steps doesn't work
     limits the number of steps balanced: 1k->1k, 10k->10k
   - 494 runs take 2h 36min for 180k steps

   Todo list
   - change the input arguments to take Fmax, LAST_STEPS
   - find best Fmax among 1 to 10. Fmax over 10 is no good
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

#define MAX_UNITS 5  /* maximum total number of units (to set array sizes) */
#define randomdef       ((float) random() / (float)((1 << 31) - 1))

int Graphics = 0;
int Delay = 20000;

#define Mc           1.0	// cart mass
#define Mp           0.1	// pole mass
#define l            0.5	// pole half length
#define g            9.8 
//#define dt          0.001	// 1ms step size. 3.6M steps 
#define dt          0.02	// 20ms step size. 180k steps
#define max_cart_pos 2.4	
#define max_cart_vel 1.5
#define max_pole_pos 0.2094
#define max_pole_vel 2.01

#define Gamma        0.9
#define ALPHA	     0.2 	/* 1st layer learning rate (typically 1/n) */
#define BETA	     0.2   	/* 2nd layer learning rate (typically 1/num_hidden) */
#define GAMMA 	     0.9  	/* discount-rate parameter (typically 0.9) */
#define LAMBDA	     0.8 	/* trace decay parameter (should be <= gamma) */
float Beta =  0.2;
float Beta_h = 0.05;
float Rho = 1.0;
float Rho_h = 0.2;
float LR_IH = 0.7;
float LR_HO = 0.07;

float state[4] = {0.0, 0.0, 0.0, 0.0};

float Fm = 1000.;		// max force magnitude. 1000 not working
int balanced = 0;
int DEBUG = 0; 			// 0: no, 1: light, 2: heavy
int TEST_RUNS = 10;
int TARGET_STEPS = 5000;
float tau = 0.02; // 0.5/1.0/2.0 working. 0.1/0.2 not working
int rspikes, lspikes;

struct
{
  double           cart_pos;
  double           cart_vel;
  double           pole_pos;
  double           pole_vel;
} the_system_state;

int start_state, failure;
double a[5][5], b[5][2], c[5][2], d[5][5], e[5][2], f[5][2]; 
double x[5], x_old[5], y[5], y_old[5], v[2], v_old[2], z[5], p[2], pushes[180000];
double r_hat[2], push, unusualness[2]; 
int test_flag = 0;

char *datafilename = "latest.train"; // latest.test1
FILE *datafile;

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
  if(test_flag) {
    int trials, sumTrials = 0, maxTrials = 1, minTrials = 100, success = 0, maxSteps = 0;
    time_t start, stop, istart, istop;
    printf("TEST_RUNS = %d\n", TEST_RUNS);
    for(i = 0; i < TEST_RUNS; i ++) {
      printf("------------- Test Run %d -------------\n", i + 1);
      //datafilename = "latest.test" + i;
      if ((datafile = fopen(datafilename,"w")) == NULL) {
        printf("Couldn't open %s\n",datafilename);
        return;
      }
      //tic
      time(&start);
      trials = Run(atoi(argv[2]), atoi(argv[3]));
      sumTrials += trials;
      if(trials > maxTrials) maxTrials = trials;
      if(trials < minTrials) minTrials = trials;
      if(trials <= 100) success ++;
      // toc
      time(&stop);
      printf("Elapsed time: %.0f seconds\n", difftime(stop, start));
      init_args(argc,argv);
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

  fclose(datafile);
  printf("Wrote current data to %s\n",datafilename);  
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
/*
  printf("Usage: %s g(raphics) num-trials trials-per-output \
weight-file(or - to init randomly) b bh r rh\n",
	 argv[0]);
*/
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
  if (argc > 8)
    tau = atof(argv[8]);
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
      for(j = 0; j < 2; j++) {
        b[i][j] = randomdef * 0.2 - 0.1;
        c[i][j] = randomdef * 0.2 - 0.1;
        e[i][j] = randomdef * 0.2 - 0.1;
        f[i][j] = randomdef * 0.2 - 0.1;
      }
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
// Normalize to [0, 1]
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
  lspikes = 0; rspikes = 0;

  NextState(1, 0.0);
  i = 0;   j = 0;
  avg_length = 0;

//  if(!test_flag)
//    printf(" B= %g Bh= %g R= %g Rh= %g nt= %d bin= %d\n",
//        Beta,Beta_h,Rho,Rho_h,num_trials,sample_period);

  if ((datafile = fopen(datafilename,"w")) == NULL) {
    printf("Couldn't open %s\n",datafilename);
    return;
  }
  setbuf(datafile, NULL);

  while (i < num_trials && j < TARGET_STEPS) /* one hour at .02s per step */
    {
      Cycle(1, j);
      if ((DEBUG == 2) && (j % 1000 == 0))
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
	  j = 0; lspikes = 0; rspikes = 0;
  	  fclose(datafile);
     	  if ((datafile = fopen(datafilename,"w")) == NULL) {
      	    printf("Couldn't open %s\n",datafilename);
      	    return;
          }
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

  if(!test_flag && balanced) {
    writeweights();
    double tt = j*dt; // total time
    fprintf(datafile,"%f spikes/sec (L:%f R:%f)\n", (lspikes + rspikes)/(tt), lspikes/(tt), rspikes/(tt));
    fprintf(datafile,"%f spikes/step (L:%f R:%f)\n", ((double)(lspikes + rspikes))/(double)j, lspikes/(double)j, rspikes/(double)j);
    fprintf(datafile,"%d spikes (L:%d R:%d), j = %d, dt = %.2f\n", (lspikes + rspikes), lspikes, rspikes, j, dt);
  }
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
  for (j = 0; j< 2; j++) {
    sum = 0.0;
    for(i = 0; i < 5; i++)
    {
      sum += b[i][j] * x[i] + c[i][j] * y[i];
    }
    v[j] = sum;
  }

  /* output: action */
  for(i = 0; i < 5; i++)
    {
      sum = 0.0;
      for (j = 0; j < 5; j++)
	sum += d[i][j] * x[j];
      z[i] = 1.0 / (1.0 + exp(-sum));
    }
  for (j = 0; j < 2; j++) {
    sum = 0.0;
    for(i = 0; i < 5; i++)
      sum += e[i][j] * x[i] + f[i][j] * z[i];
    p[j] = 1.0 / (1.0 + exp(-sum));
  }

  int left = 0, right = 0;
  if(randomdef <= p[0]) {
    left = 1; lspikes ++;
    unusualness[0] = 1 - p[0];
  } else 
    unusualness[0] = -p[0];

  if(randomdef <= p[1]) { 
    right = 1; rspikes ++;
    unusualness[1] = 1 - p[1];
  } else 
    unusualness[1] = -p[1];

  if(left == 1 && right == 0) {
    push = 1.0; 
  } else if (left == 0 && right == 1) {
    push = -1.0; 
  } else { 
    push = 0; 
  }
  pushes[step] = push;

  sum = 0.0;
  int upto = (step > 400 ? 400 : step);
  for(i = 1; i < upto ; i++) {
  //for(i = 0; i < step; i++) {
    //t = (step - i) * dt;
    t = i * dt;
    //sum += t * exp(-t/tau);
    sum += pushes[step - i] * t * exp(-t/tau);
  }
  //push *= sum;
  //push = sum;
  push = Fm*sum;
  if (DEBUG) printf("step %d L %d R %d push %f\n", step, left, right, push);

  //push *= 100.0; //  < 50 runs for 10k
  //push *= 75.0; // < 20 runs for 10k, 120 runs for 50k
  //push *= 50.0; // good, fast < 20 runs for 50k
  //push *= 25.0; // < 50 runs for 10k
  //push *= 10.0; // no good, slow < 100 runs for 10k

  /* preserve current activities in evaluation network. */
  for (i = 0; i< 2; i++)
    v_old[i] = v[i];

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
  for (j = 0; j< 2; j++) {
    sum = 0.0;
    for(i = 0; i < 5; i++)
    {
      sum += b[i][j] * x[i] + c[i][j] * y[i];
    }
    v[j] = sum;
  }

  /* action evaluation */
  for(i = 0; i < 2; i++) {
    if (start_state)
      r_hat[i] = 0.0;
    else
      if (failure)
        r_hat[i] = failure - v_old[i];
      else
        r_hat[i] = failure + Gamma * v[i] - v_old[i];
  }

  /* report stats */
  if(step % 100 == 0)
    fprintf(datafile,"%d %d %.4f %.4f %.4f %.4f %.4f %.4f %.4f\n", left, right, r_hat[0], r_hat[1], 
			the_system_state.cart_pos, the_system_state.cart_vel,
			the_system_state.pole_pos, the_system_state.pole_vel, 
 			push);

  /* modification */
  if (learn_flag)
	updateweights();
}

/**********************************************************************/
void updateweights() {
  int i, j, k;
  double factor1, factor2;
      for(i = 0; i < 5; i++)
	{
	  for(j = 0; j < 5; j++)
	    {
	      for(k = 0; k < 2; k++) {
   	        factor1 = Beta_h * r_hat[k] * y_old[i] * (1.0 - y_old[i]) * sgn(c[i]);
	        factor2 = Rho_h * r_hat[k] * z[i] * (1.0 - z[i]) * sgn(f[i]) * unusualness[k];
   	        a[i][j] += factor1 * x_old[j];
	        d[i][j] += factor2 * x_old[j];
              }
	    }
	  for(j = 0; j < 2; j++) {
	    b[i][j] += Beta * r_hat[j] * x_old[i];
	    c[i][j] += Beta * r_hat[j] * y_old[i];
	    e[i][j] += Rho * r_hat[j] * unusualness[j] * x_old[i];
	    f[i][j] += Rho * r_hat[j] * unusualness[j] * z[i];
  	  }
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
      for(j = 0; j < 2; j++)
        fscanf(file,"%lf",&b[i][j]);

  for(i = 0; i < 5; i++)
      for(j = 0; j < 2; j++)
        fscanf(file,"%lf",&c[i][j]);


  for(i = 0; i < 5; i++)
      for(j = 0; j < 5; j++)
	  fscanf(file,"%lf",&d[i][j]);

  for(i = 0; i < 5; i++)
      for(j = 0; j < 2; j++)
        fscanf(file,"%lf",&e[i][j]);

  for(i = 0; i < 5; i++)
      for(j = 0; j < 2; j++)
       fscanf(file,"%lf",&f[i][j]);

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
      for(j = 0; j < 2; j++)
       fprintf(file," %f",b[i][j]);

  fprintf(file, "\n");

  for(i = 0; i < 5; i++)
      for(j = 0; j < 2; j++)
        fprintf(file," %f",c[i][j]);

  fprintf(file, "\n");

  for(i = 0; i < 5; i++)
      for(j = 0; j < 5; j++)
	  fprintf(file," %f",d[i][j]);

  fprintf(file, "\n");

  for(i = 0; i < 5; i++)
      for(j = 0; j < 2; j++)
        fprintf(file," %f",e[i][j]);

  fprintf(file, "\n");

  for(i = 0; i < 5; i++)
      for(j = 0; j < 2; j++)
       fprintf(file," %f",f[i][j]);

  fclose(file);

  printf("Wrote current weights to %s\n",filename);
}

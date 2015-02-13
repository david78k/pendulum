/* 
   v0.5.0 - 2/12/2015
   Changelog
   - 1output with 2actions(L/R) to 2outputs(L/R) with 3actions L/R/0
   - two outputs for both networks
   - td-backprop code for evaluation network combined: multiple outputs
   - tau added
   - changed the input arguments to take TEST_RUNS, TARGET_STEPS, DEBUG
   - tic-toc elapsed time
   - 1output with 2actions(L/R)
   - test runs only when balanced
   - continuous force
   - taking only the last few steps doesn't work
     limits the number of steps balanced: 1k->1k, 10k->10k
   - 494 runs take 2h 36min for 180k steps

   Todo list
   - change the input arguments to take Fmax, LAST_STEPS
   - find best Fmax among 1 to 10. Fmax over 10 is no good
   - Get last steps working
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
#define ALPHA	     0.2  /* 1st layer learning rate (typically 1/n) */
#define BETA	     0.2   /* 2nd layer learning rate (typically 1/num_hidden) */
#define GAMMA 	     0.9  /* discount-rate parameter (typically 0.9) */
#define LAMBDA	     0.8 /* trace decay parameter (should be <= gamma) */
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
float tau = 1; // 0.5/1.0/2.0 working. 0.1/0.2 not working

struct
{
  double           cart_pos;
  double           cart_vel;
  double           pole_pos;
  double           pole_vel;
} the_system_state;

int start_state, failure;
double a[5][5], b[5][2], c[5][2], d[5][5], e[5][2], f[5][2]; 
double x[5], x_old[5], y[5], y_old[5], v[2], v_old[2], z[5], p[2];
float  wih[MAX_UNITS][MAX_UNITS]; /* weights I-H or x-h */
double r_hat[2], push, unusualness[2]; 
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
      trials = Run(atoi(argv[2]), atoi(argv[3]));
      sumTrials += trials;
      if(trials > maxTrials) maxTrials = trials;
      if(trials < minTrials) minTrials = trials;
      if(trials <= 100) success ++;
      //if(trials == 1) success ++;
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
    InitNetwork();
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
      //tdbp();
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
  //Response();
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

  double q, pp; 
  int left = 0, right = 0;
  if(randomdef <= p[0]) {
    left = 1; 
    unusualness[0] = 1 - p[0];
  } else 
    unusualness[0] = -p[0];

  if(randomdef <= p[1]) { 
    right = 1;
    unusualness[1] = 1 - p[1];
  } else 
    unusualness[1] = -p[1];

  if(left == 1 && right == 0) {
    push = 1.0; q = 1.0; pp = p[1];
  } else if (left == 0 && right == 1) {
    push = -1.0; q = 0.5; pp = p[0];
  } else { 
    push = 0; q = 0; pp = 0;
  }

  //push = (randomdef <= p) ? 1.0 : -1.0;
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
  //unusualness = q - pp;
  //unusualness = (push > 0) ? 1.0 - p : ((push < 0) ? 0.5-p : -p);
  //unusualness = (push > 0) ? 1.0 - p : -p;

  sum = 0.0;
  //for(i = 0; i < (step > 100 ? 100 : step) ; i++) {
  for(i = 0; i < step; i++) {
    t = (step - i) * dt;
    sum += t * exp(-t/tau);
    //sum += t * exp(-t);
  }
  push *= sum;
 // push = (push > 0) ? 10.0 : (push < 0 ? -10.0 : 0);

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
  /* modification */
  if (learn_flag)
    {
	updateweights();
   }
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

/************************************************************************

Nonlinear TD/Backprop pseudo C-code

Written by Allen Bonde Jr. and Rich Sutton in April 1992. 
Updated in June and August 1993. 
Copyright 1993 GTE Laboratories Incorporated. All rights reserved. 
Permission is granted to make copies and changes, with attribution,
for research and educational purposes.

This pseudo-code implements a fully-connected two-adaptive-layer network
learning to predict discounted cumulative outcomes through Temporal
Difference learning, as described in Sutton (1988), Barto et al. (1983),
Tesauro (1992), Anderson (1986), Lin (1992), Dayan (1992), et alia. This
is a straightforward combination of discounted TD(lambda) with
backpropagation (Rumelhart, Hinton, and Williams, 1986). This is vanilla
backprop; not even momentum is used. See Sutton & Whitehead (1993) for
an argument that backprop is not the best structural credit assignment
method to use in conjunction with TD. Discounting can be eliminated for
absorbing problems by setting GAMMA=1. Eligibility traces can be
eliminated by setting LAMBDA=0. Setting both of these parameters to 0
should give standard backprop except where the input at time t has its
target presented at time t+1. 

This is pseudo code: before it can be run it needs I/O, a random
number generator, library links, and some declarations.  We welcome
extensions by others converting this to immediately usable C code.
 
The network is structured using simple array data structures as follows:


                    OUTPUT
   
                  ()  ()  ()  y[k]
                 /  \/  \/  \      k=0...m-1
     ew[j][k]   /   w[j][k]  \
               /              \
              ()  ()  ()  ()  ()  h[j]
               \              /        j=0...num_hidden
   ev[i][j][k]  \   v[i][j]  /
                 \  /\  /\  /
                  ()  ()  ()  x[i]
                                   i=0...n
                     INPUT


where x, h, and y are (arrays holding) the activity levels of the input,
hidden, and output units respectively, v and w are the first and second
layer weights, and ev and ew are the eligibility traces for the first
and second layers (see Sutton, 1989). Not explicitly shown in the figure
are the biases or threshold weights. The first layer bias is provided by
a dummy nth input unit, and the second layer bias is provided by a dummy
(num-hidden)th hidden unit. The activities of both of these dummy units
are held at a constant value (BIAS).

In addition to the main program, this file contains 4 routines:

    InitNetwork, which initializes the network data structures.

    Response, which does the forward propagation, the computation of all 
        unit activities based on the current input and weights.

    TDlearn, which does the backpropagation of the TD errors, and updates
        the weights.

    UpdateElig, which updates the eligibility traces.

These routines do all their communication through the global variables
shown in the diagram above, plus old_y, an array holding a copy of the
last time step's output-layer activities.

For simplicity, all the array dimensions are specified as MAX_UNITS, the 
maximum allowed number of units in any layer.  This could of course be
tightened up if memory becomes a problem.

REFERENCES

Anderson, C.W. (1986) Learning and Problem Solving with Multilayer
Connectionist Systems, UMass. PhD dissertation, dept. of Computer and
Information Science, Amherts, MA 01003.

Barto, A.G., Sutton, R.S., & Anderson, C.W. (1983) "Neuron-like adaptive
elements that can solve difficult learning control problems," IEEE
Transactions on Systems, Man, and Cybernetics SMC-13: 834-846.

Dayan, P. "The convergence of TD(lambda) for general lambda,"
Machine Learning 8: 341-362.

Lin, L.-J. (1992) "Self-improving reactive agents based on reinforcement
learning, planning and teaching," Machine Learning 8: 293-322.

Rumelhart, D.E., Hinton, G.E., & Williams, R.J. (1986) "Learning
internal representations by error propagation," in D.E. Rumehart & J.L.
McClelland (Eds.), Parallel Distributed Processing: Explorations in the
Microstructure of Cognition, Volume 1: Foundations, 318-362. Cambridge,
MA: MIT Press.

Sutton, R.S. (1988) "Learning to predict by the methods of temporal
differences," Machine Learning 3: 9-44.

Sutton, R.S. (1989) "Implementation details of the TD(lambda) procedure
for the case of vector predictions and backpropagation," GTE
Laboratories Technical Note TN87-509.1, May 1987, corrected August 1989.
Available via ftp from ftp.gte.com as 
/pub/reinforcement-learning/sutton-TD-backprop.ps

Sutton, R.S., Whitehead, S.W. (1993) "Online learning with random
representations," Proceedings of the Tenth National Conference on
Machine Learning, 314-321. Soon to be available via ftp from ftp.gte.com
as /pub/reinforcement-learning/sutton-whitehead-93.ps.Z

Tesauro, G. (1992) "Practical issues in temporal difference learning,"
Machine Learning 8: 257-278.
************************************************************************/

/* Experimental Parameters: */

int    n = 5, num_hidden = 5, m = 2; /* number of inputs, hidden, and output units */
//int    MAX_UNITS = 5;  /* maximum total number of units (to set array sizes) */
int    time_steps;  /* number of time steps to simulate */
float  BIAS;   /* strength of the bias (constant input) contribution */

/* Network Data Structure: */

//float  x[time_steps][MAX_UNITS]; /* input data (units) */
float  h[MAX_UNITS]; /* hidden layer */
//float  y[MAX_UNITS]; /* output layer */
float  w[MAX_UNITS][MAX_UNITS]; /* weights H-O or h-y */
//float  wih[MAX_UNITS][MAX_UNITS]; /* weights I-H or x-h */

/* Learning Data Structure: */

float  old_y[MAX_UNITS];
float  ev[MAX_UNITS][MAX_UNITS][MAX_UNITS]; /* hidden trace */
float  ew[MAX_UNITS][MAX_UNITS]; /* output trace */
float  r[MAX_UNITS]; /* reward */
float  error[MAX_UNITS];  /* TD error */
//int    t;  /* current time step */

tdbp()
{
int k;
//InitNetwork();

//t=0;                   /* No learning on time step 0 */
//Response();            /* Just compute old response (old_y)...*/
//for (k=0;k<m;k++)
//   old_y[k] = y[k];
//UpdateElig();          /* ...and prepare the eligibilities */

//for (t=1;t<=time_steps;t++)  /* a single pass through time series data */
//  {
   Response();         /* forward pass - compute activities */

  /* Apply the push to the pole-cart */
  NextState(0, push);

   for (k=0;k<m;k++)
     error[k] = r[k] + GAMMA*y[k] - old_y[k]; /* form errors */
   TDlearn();          /* backward pass - learning */
   Response();         /* forward pass must be done twice to form TD errors */
   for (k=0;k<m;k++)
     old_y[k] = y[k];  /* for use in next cycle's TD errors */
   UpdateElig();       /* update eligibility traces */
//  } /* end t */
} /* end main */

/*****
 * InitNetwork()
 *
 * Initialize weights and biases
 *
 *****/

InitNetwork(void)

{
int s,j,k,i;

//for (s=0;s<time_steps;s++)
//  x[s][n]=BIAS;
//h[num_hidden]=BIAS;
//for (j=0;j<=num_hidden;j++)
for (j=0;j<num_hidden;j++)
  {
  for (k=0;k<m;k++)
    {
    w[j][k]= randomdef * 0.2 - 0.1; // some small random value
    ew[j][k]=0.0;
    old_y[k]=0.0;
    }
  //for (i=0;i<=n;i++)
  for (i=0;i<n;i++)
    {
    wih[i][j]= randomdef * 0.2 - 0.1; //some small random value
    for (k=0;k<m;k++)
      {
      ev[i][j][k]=0.0;
      }
    }
  }
}/* end InitNetwork */

/*****
 * Response()
 *
 * Compute hidden layer and output predictions
 *
 *****/
Response(void)
{
int i,j,k;

//h[num_hidden]=BIAS;
//x[t][n]=BIAS;

for (j=0;j<num_hidden;j++)
  {
  h[j]=0.0;
  for (i=0;i<=n;i++)
    {
    h[j]+=x[i]*wih[i][j];
    }
  h[j]=1.0/(1.0+exp(-h[j])); /* asymmetric sigmoid */
  }
for (k=0;k<m;k++)
  {
  y[k]=0.0;
  for (j=0;j<=num_hidden;j++)
    {
    y[k]+=h[j]*w[j][k];
    }
  y[k]=1.0/(1.0+exp(-y[k])); /* asymmetric sigmoid (OPTIONAL) */
  }
}/* end Response */

/*****
 * TDlearn()
 *
 * Update weight vectors
 *
 *****/

TDlearn(void)

{
int i,j,k;

for (k=0;k<m;k++)
  {
  for (j=0;j<=num_hidden;j++)
    {
    w[j][k]+=BETA*error[k]*ew[j][k];
    for (i=0;i<=n;i++)
      wih[i][j]+=ALPHA*error[k]*ev[i][j][k];
    }
  }
}/* end TDlearn */

/*****
 * UpdateElig()
 *
 * Calculate new weight eligibilities
 *
 *****/

UpdateElig(void)

{
int i,j,k;
float temp[MAX_UNITS];

for (k=0;k<m;k++)
  temp[k]=y[k]*(1-y[k]);

for (j=0;j<=num_hidden;j++)
  {
  for (k=0;k<m;k++)
    {
    ew[j][k]=LAMBDA*ew[j][k]+temp[k]*h[j];
    for (i=0;i<=n;i++)
      {
      ev[i][j][k]=LAMBDA*ev[i][j][k]+temp[k]*w[j][k]*h[j]*(1-h[j])*x[i];
      }
    }
  }
}/* end UpdateElig */


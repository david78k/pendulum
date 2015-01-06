/******************************************************************************
 *  This file contains a simulation of the cart and pole dynamic system
 * and a procedure for learning to balance the pole that uses multilayer
 * connectionist networks.  Both are described in Anderson, "Strategy
 * Learning with Multilayer Connectionist Representations," GTE
 * Laboratories TR87-509.3, 1988.  This is a corrected version of the
 * report published in the Proceedings of the Fourth International
 * Workshop on Machine Learning, Irvine, CA, 1987.
 * 
 *   main takes six arguments: 
 *       the maximum number of trials (balancing attempts),
 *       the number of trials over which balancing steps are averaged
 *       beta
 *       beta for hidden units
 *       rho
 *       rho for hidden units
 * 
 *     Please send questions and comments to anderson@cs.colostate.edu
 *****************************************************************************/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

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

float acgrnd();
void init_acgrnd_by_time();

/****************************************************************/

main(argc, argv)
     int argc;
     char *argv[];
{
  int runtimes;

  init_acgrnd_by_time();

  char** usage = "usage: command arg1 arg2 arg3 arg4 arg5 arg6";

  if (argc < 3) {
    printf("%s\n", usage);
    exit(-1);
  }
  SetRandomWeights();
  if (argc > 3)
    Beta = atof(argv[3]);
  if (argc > 4)
    Beta_h = atof(argv[4]);
  if (argc > 5)
    Rho = atof(argv[5]);
  if (argc > 6)
    Rho_h = atof(argv[6]);

  Run(atoi(argv[1]), atoi(argv[2]));
}

/****************************************************************/

SetRandomWeights()
{
  int i,j;

  for(i = 0; i < 5; i++)
    {
      for(j = 0; j < 5; j++)
	{
	  a[i][j] = acgrnd() * 0.2 - 0.1;
	  d[i][j] = acgrnd() * 0.2 - 0.1;
	}
      b[i] = acgrnd() * 0.2 - 0.1;
      c[i] = acgrnd() * 0.2 - 0.1;
      e[i] = acgrnd() * 0.2 - 0.1;
      f[i] = acgrnd() * 0.2 - 0.1;
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
      the_system_state.cart_pos = acgrnd() * 2 * max_cart_pos - max_cart_pos;
      the_system_state.cart_vel = acgrnd() * 2 * max_cart_vel - max_cart_vel;
      the_system_state.pole_pos = acgrnd() * 2 * max_pole_pos - max_pole_pos;
      the_system_state.pole_vel = acgrnd() * 2 * max_pole_vel - max_pole_vel;
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

  NextState(1, 0.0);
  i = 0;
  j = 0;
  avg_length = 0;

  printf(" B= %g Bh= %g R= %g Rh= %g nt= %d bin= %d\n",
      Beta,Beta_h,Rho,Rho_h,num_trials,sample_period);

  while (i < num_trials && j < 180000) /* one hour at .02s per step */
    {
      Cycle(1);
      j++;

      if (failure)
	{
	  avg_length += j;
	  i++;
	  if (!(i % sample_period))
	    {
	      printf("%6d %6d\n", i, avg_length / sample_period);
	      avg_length = 0;
	    }
	  NextState(1, 0.0);
	  j = 0;
	}
    }
   printf("Final trial %d balanced for %d steps (%f hours).\n",
          i, j, (j * tau)/3600.0);


/*
   for (i = 0; i<5; i++ )
     for (j = 0; j< 5; j++ )
      printf( "%3.4f ", d[i][j] );
   for ( i=0; i< 5; i++ )
     printf ( "%3.4f ", e[i] );
   for ( i=0; i< 5; i++ )
     printf ( "%3.4f ", f[i] );
*/	
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
  double sum, factor1, factor2;
  extern double exp();

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
  push = (acgrnd() <= p) ? 10.0 : -10.0;
  unusualness = (push > 0) ? 1.0 - p : -p;  /* unusualness = q - p */

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
}

/**********************************************************************/

#include <sys/types.h>
#include <sys/time.h>

/*
		Addititive congruential  random number generator
		adapted from  Hill's Computer Graphics. Tests for 
		independence of sequence (Big-R) and uniformity of 
		distribution (Chi-square) are output. The histogram 
		dist. of random numbers is also output (3/22/91). 
		A floating point number between 0 - 1 is returned.
*/
 
/* global quantities for random number generator */
#define MODULUS 32767
#define TRUE 1
#define FALSE 0
#define LENGTH 17

int   the_seed;
int   ranlist[LENGTH]; /* global list */
int   i_ran, j_ran;/* global indices */

/*--------------------------------------------------------------*/

void
init_acgrnd_by_time()
/* loads random values into global array ranlist[] */
{
	int   i;
	int   all_even;

	the_seed = time(NULL);
/*	the_seed = 0;*/

	all_even  = TRUE;
	while ( all_even ) /* try until one or more values are odd */
	  for ( i=0; i < LENGTH; i++ ) /* load array */
	    {
		the_seed = (the_seed * 2743 + 5923 ) % MODULUS;
		/* adjust if value "overflowed" capacity of an integer */
		if ( the_seed < 0 ) the_seed = the_seed + MODULUS;
		if ( the_seed % 2 != 0 ) all_even = FALSE;
		ranlist[i] = the_seed;
	    }
	i_ran = LENGTH - 1; j_ran = 5;
}

/*-------------------------------------------------------------*/

float acgrnd()
/* generate a random number in the range 0.0 to 1.0 */
/* use global ranlist, i_ran, and j_ran */
{
   int   t, randnum;
   float rand;

   randnum = ( ranlist[i_ran] + ranlist[j_ran] ) % MODULUS;
   if ( randnum < 0 ) randnum = randnum + MODULUS;
   ranlist[i_ran] = randnum;
   i_ran = ( i_ran == 0 ) ? LENGTH - 1 : i_ran--;
   j_ran = ( j_ran == 0 ) ? LENGTH - 1 : j_ran--;
   rand = (float) randnum / MODULUS;
   return rand;
} 

/****************************************************************************
*****************************************************************************
Example results.

First argument is maximum number of trials (failures).
Second argument is number of trails to average the balancing time (steps) over.
Next four are  beta, beta_h, rho, rho_h.

on Sun IPC:
>cc -O -o mpole multilayer-pole.c -lm
>mpole 10000 1000 .2 .05 1 .2
 B=0.2 Bh=0.05 R=1 Rh=0.2 nt=10000 bin=1000
  1000     13
  2000     17
  3000     15
  4000     28
Final trial 4638 balanced for 180000 steps (1.000000 hours).

*****************************************************************************/


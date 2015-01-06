/*----------------------------------------------------------------------
 This file contains the driver for a simulation of the pole balancing
 problem.  The code was originally written by Rich Sutton and Chuck
 Anderson, and Claude Sammut translated parts from FORTRAN to C.
 Originally, their code included the controller process which decides
 which actions to take.

 I have removed their controller code so that this driver may be
 compiled with different controllers to compare their performance on a
 standard control task.  The controller should return a value of 0 or
 1 for the system state represented by (x, x_dot, theta, theta_dot); 0
 corresponds to a left push of 10.0 Newtons, and a 1 corresponds to a
 rightward push of 10.0 Newtons.

 I have converted the functions to ANSI C style.

 I have also added code which makes the problem more challenging.  If
 JUPITER_GRAV is set, cart_and_pole uses a gravitational constant of
 30.0 instead of 9.8.  If TILTED is set, the pole is initialized with
 a tilt of six degrees, with the direction of tilt---left or
 right---chosen randomly.  JUPITER_GRAV and TILTED are both turned off
 by default.  (By the way, I have no idea what Jupiter's gravitational
 acceleration really is, but I just liked the name JUPITER_GRAV).

 This code must be compiled with a separate controller module.  The 
 controller must implement the following functions:

    (int) get_action(float x, float x_dot, 
                     float theta, float theta_dot, float reinforcement);  

    void reset_controller(void);

 "get_action" returns the next action of the system: 0 (left push), or
 1 (right push).  The parameter "reinforcement" supplies feedback to
 the controller, which allows this driver program to be used with
 controllers which learn.  But the driver makes no assumptions about
 the nature of the controller.  "reset_controller" performs whatever
 action is necessary for the controller to be ready for a new trial.

 Here are two other cart-pole programs I know of:

    The Barto, Sutton, and Anderson cart-pole simulation.  Available
    by anonymous ftp from ftp.gte.com, as
    /pub/reinforcement-learning/pole.c.  This solution for the
    adaptive controller is described in Barto, Sutton, and Anderson,
    "Neuronlike Adaptive Elements That Can Solve Difficult Learning
    Control Problems," IEEE Trans. Syst., Man, Cybern., Vol. SMC-13,
    pp. 834--846, Sept.--Oct. 1983, and in Sutton, "Temporal Aspects
    of Credit Assignment in Reinforcement Learning", PhD Dissertation,
    Department of Computer and Information Science, University of
    Massachusetts, Amherst, 1984.  Contact: Rich Sutton
    (sutton@gte.com).  "pole.c" was my base for this code.

    Chuck Anderson's simulation of the cart-pole problem with a
    learning controller that uses multilayer connectionist networks.
    URL: http://www.cs.colostate.edu/~anderson/code/multilayer-pole.c
    This simulation is described in Anderson, "Strategy Learning with
    Multilayer Connectionist Representations," GTE Laboratories
    TR87-509.3, 1988; this is a corrected version of the report
    published in the Proceedings of the Fourth International Workshop
    on Machine Learning, Irvine, CA, 1987.  Contact Chuck Anderson
    (anderson@cs.colostate.edu).
 
-- David Finton, November 1994
   finton@cs.wisc.edu
   
$Log: driver.c,v $
 * Revision 2.0  1994/11/17  19:45:38  finton
 * Cleaned-up cart-pole driver.
 *
----------------------------------------------------------------------*/
                          
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define JUPITER_GRAV 0             /* If set, use bigger gravity const */
#define TILTED 0                   /* If set, pole is given an initial tilt */
#define MAX_FAILURES    3000       /* Termination criterion */
#define MAX_STEPS       100000     /* about 33 minutes of balancing */

int ECHO_STATE = 0;                /* save state parameters to a file */
FILE *echo_file = NULL; 
int RND_SEED = 0;

static char rcs_driver_id[] = "$Id: driver.c,v 2.0 1994/11/17 19:45:38 finton Exp $";

int main(int argc, char *argv[])
{
   float x,                         /* cart position, meters */
         x_dot,                     /* cart velocity */
         theta,                     /* pole angle, radians */
         theta_dot;                 /* pole angular velocity */
   int action;                      /* 0 for push-left, 1 for push-right */
   int steps = 0;                   /* duration of trial, in 0.02 sec steps */
   int failures = 0;                /* number of failed trials */
   int best_steps = 0;              /* number of steps in best trial */
   int best_trial = 0;              /* trial number of best trial */

   void reset_state(float *x, float *x_dot, float *theta, float *theta_dot);
   void cart_pole(int action, float *x, float *x_dot, 
                  float *theta, float *theta_dot);
   int fail(float x, float x_dot, float theta, float theta_dot);
   extern int get_action(float x, float x_dot, float theta, float theta_dot,
                         float reinforcement);
   extern void reset_controller(void);
/* extern void print_controller_info(); */

   printf("Driver: %s\n", rcs_driver_id);
   if (TILTED)
      printf("Pole will have tilted reset,");
   else
      printf("Pole has normal reset,");
   if (JUPITER_GRAV)
      printf(" and \"Jupiter\" gravity.\n");
   else
      printf(" and normal gravity.\n");

   if (ECHO_STATE)
   {
      echo_file = fopen("poledata", "w");
      if (echo_file == NULL)
         printf("ERROR: Cannot open \"poledata\" for output.\n");
   }

   if (argc > 1)
      RND_SEED = atoi(argv[1]);
   else
      RND_SEED = 0;

   reset_state(&x, &x_dot, &theta, &theta_dot);

   /*--- Iterate through the action-learn loop. ---*/
   while (steps++ < MAX_STEPS && failures < MAX_FAILURES)
   {
      action = get_action(x, x_dot, theta, theta_dot, 0.0);  
      
      /*--- Apply action to the simulated cart-pole ---*/
      cart_pole(action, &x, &x_dot, &theta, &theta_dot);

      if (fail(x, x_dot, theta, theta_dot))
      {
	  failures++;
	  printf("Trial %d was %d steps.\n", failures, steps);
          if (steps > best_steps)
          {
             best_steps = steps;
             best_trial = failures;
          }

          /* Call controller with negative feedback for learning */
          get_action(x, x_dot, theta, theta_dot, -1.0);

          reset_controller();
          reset_state(&x, &x_dot, &theta, &theta_dot);
	  steps = 0;
      }
   }

   /* Diagnose result */
   if (failures == MAX_FAILURES) 
   {
      printf("Pole not balanced. Stopping after %d failures.\n",failures);
      printf("High water mark: %d steps in trial %d.\n\n", 
             best_steps, best_trial);
   }
   else
    printf("Pole balanced successfully for at least %d steps in trial %d.\n\n",
            steps - 1, failures + 1);

/* print_controller_info();*/
   if (echo_file != NULL)
      fclose(echo_file);
   return 0;
}


/*--------------------------------------------------------------------*/

void reset_state(float *x, float *x_dot, float *theta, float *theta_dot)
{
#define SIX_DEGREES     0.1047198
#define SEVEN_DEGREES   0.1221730
#define TEN_DEGREES     0.1745329

   float plus_or_minus(float val);

   *x = 0.0;
   *x_dot = 0.0;
   *theta_dot = 0.0;
   if (TILTED)
      *theta = plus_or_minus(SIX_DEGREES);
   else
      *theta = 0.0;
}


/*---------------------------------------------------------------------+
| plus_or_minus: takes a value and randomly returns either that value  |
|     or its negation                                                  |
+---------------------------------------------------------------------*/

float plus_or_minus(float val)
{
   long random(void);            /* system random number generator */

/* if RAND_MAX is undefined, try (random() / (float) ((1 << 31) - 1)) */
   if ((random() / (float)RAND_MAX)  >  0.5)
      return val;
   else
      return -val;
}


 
/*----------------------------------------------------------------------
   cart_pole:  Takes an action (0 or 1) and the current values of the
 four state variables and updates their values by estimating the state
 TAU seconds later.
----------------------------------------------------------------------*/

/*** Parameters for simulation ***/
#if JUPITER_GRAV
#define GRAVITY 30.0
#else
#define GRAVITY 9.8
#endif
#define MASSCART 1.0
#define MASSPOLE 0.1
#define TOTAL_MASS (MASSPOLE + MASSCART)
#define LENGTH 0.5		  /* actually half the pole's length */
#define POLEMASS_LENGTH (MASSPOLE * LENGTH)
#define FORCE_MAG 10.0
#define TAU 0.02		  /* seconds between state updates */
#define FOURTHIRDS 1.3333333333333


void cart_pole(int action, float *x, float *x_dot, 
               float *theta, float *theta_dot)
{
    float xacc,thetaacc,force,costheta,sintheta,temp;
    int fail(float x, float x_dot, float theta, float theta_dot);

    force = (action>0)? FORCE_MAG : -FORCE_MAG;  
    costheta = cos(*theta);
    sintheta = sin(*theta);

    temp = (force + POLEMASS_LENGTH * *theta_dot * *theta_dot * sintheta)
		         / TOTAL_MASS;

    thetaacc = (GRAVITY * sintheta - costheta* temp)
	       / (LENGTH * (FOURTHIRDS - MASSPOLE * costheta * costheta
                                              / TOTAL_MASS));

    xacc  = temp - POLEMASS_LENGTH * thetaacc* costheta / TOTAL_MASS;

/*** Update the four state variables, using Euler's method. ***/

    *x  += TAU * *x_dot;
    *x_dot += TAU * xacc;
    *theta += TAU * *theta_dot;
    *theta_dot += TAU * thetaacc;
}


/*----------------------------------------------------------------------
       fail:    returns 1 if the pole falls more than 12 degrees
                from vertical, or if the cart moves beyond the
                limits of its track, 2.4 meters in either direction
                from its starting position
----------------------------------------------------------------------*/

int fail(float x, float x_dot, float theta, float theta_dot)
{
   float twelve_degrees = 0.2094384;

   if ( theta < -twelve_degrees || theta > twelve_degrees ||
        x < -2.4 || x > 2.4 )
              return 1;  /* fail == true */
   else
              return 0;
}

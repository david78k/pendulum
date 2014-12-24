#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <process.h>
#include <conio.h>
#include <time.h>

#define GRAVITY 9.8
#define MASSCART 1.0
#define MASSPOLE 0.1
#define TOTAL_MASS (MASSPOLE + MASSCART)
#define LENGTH 0.5		  /* actually half the pole's length */
#define POLEMASS_LENGTH (MASSPOLE * LENGTH)
#define FORCE_MAG 10.0
#define TAU 0.02		  /* seconds between state updates */
#define FOURTHIRDS 1.3333333333333
#define sqr(x) ( (x) * (x) )

static double ALPHA = 0.05;              /* learning rate parameter */
static double BETA = 0.01;               /* magnitude of noise added to choice */
static double GAMMA = 0.8;            /* discount factor for future reinf */

static double q_left, q_right;      /* state-action values */
static first_time = 1;
static int cur_action, prev_action;
static int cur_state, prev_state;
static double x, x_dot, theta, theta_dot,reinf,predicted_value;
/* Define network variables */
#define L2 14
#define L1 (10+L2)   /* 9 neurons for state, one for action */
static double W1[L2][L1], W2[L2], B1[L2], B2, Y1[L2], Y2 ,X[L1];

int get_action(double,double,double,double,double);
void cart_pole(int,double*,double*,double*,double*);
double rnd(double, double);
int get_box(double, double, double, double);  /*state*/
void reset_controller(void);    /* reset state/action before new trial */
double elman(int state,int action);
void BP(int state,int action,double reinf);
double sigmod(double x);
void initial_network(void);
double maxium(double,double);

long best=0;

void main()
{
	int action,box,i,j,c=0;
	long success,trial,count;
	FILE *fptr;
	srand((unsigned)time(NULL));    /* initialize random number generator */
	initial_network();
	x=x_dot=theta=theta_dot=rnd(-BETA,BETA);
	reset_controller();
	success=0;
	count=0;
	trial=1;
	reinf=0.0;
	while (success<1000000)    /* If the pole doesn't fall during 1-million trials,assume it succcess.*/
	{
		action=get_action(x,x_dot,theta,theta_dot,reinf);
		cart_pole(action,&x,&x_dot,&theta,&theta_dot);
		box=get_box(x,x_dot,theta,theta_dot);
		if (box==-1)
		{	reinf=-1.0+rnd(-0.01,0);
			BP(prev_state,prev_action,reinf);
			if (success>best)
				best=success;
			trial++;
			c++;
			if (c>1000)
			{	printf("At %d success ,try %d trials,Best is %d; Q is %g  %g\n",success,trial,best,q_left,q_right);
				c=0;
				/* Write current network ot file */
				fptr=fopen("elman.txt","w");
				fprintf(fptr,"W1= \n");
				for (i=0;i<L2;i++)
				{	for (j=0;j<L1;j++)
					{	fprintf(fptr,"%g  ",W1[i][j]);
					}
					fprintf(fptr,"\n");
				}
				fprintf(fptr,"W2= \n");
				for (i=0;i<L2;i++)
				{	fprintf(fptr,"%g  ",W2[i]);
				}
				fprintf(fptr,"\nB1=\n");
				for (i=0;i<L2;i++)
				{	fprintf(fptr,"%g  ",B1[i]);
				}
				fprintf(fptr,"\nB2= %g",B2);
				fclose(fptr);
			}
			reset_controller();
			x=theta=x_dot=theta_dot=rnd(-BETA,BETA);
			/*x=rnd(-2,2);
			theta=rnd(-0.2,0.2);*/
			success=0;
			count=0;
		}else{
			  success++;
			  reinf=0.0;
			  if ( (prev_action>=0) & (prev_state>=0) )
			  {		BP(prev_state,prev_action,reinf);
			  }
			}
	}
	printf("Success at %d trials \n",trial);
}

/*----------------------------------------------------------------------
   cart_pole:  Takes an action (0 or 1) and the current values of the
 four state variables and updates their values by estimating the state
 TAU seconds later.
----------------------------------------------------------------------*/

void cart_pole(int action,double *x,double *x_dot,double *theta,double *theta_dot)
{
    double xacc,thetaacc,force,costheta,sintheta,temp;

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

/*** Q-learning part ***/
/*----------------------------------------------------------------------------+
|   get_action : returns either 0 or 1 as action choice;                      |
|                accepts five "black box" inputs, the first four of which are |
|                   system variables, and the last is a reinforcement signal. |
|                Note that reinf is the result of the previous state and      |
|                   action.                                                   |
+----------------------------------------------------------------------------*/

int get_action(double x,            /* system variables == state information */
               double x_dot, 
               double theta, 
               double theta_dot, 
               double reinf)        /* reinforcement signal */
{
   int i,j,action,state;
   double f,prob_left,prob_right;
	prev_state = cur_state;
	prev_action = cur_action;
	cur_state=get_box(x,x_dot,theta,theta_dot);

   state=get_box(x,x_dot,theta,theta_dot);

   q_left=elman(state,0);
   q_right=elman(state,1);
   /* Now determine best action */
   /* Use SAS */
   /*T=Tmin+ beta*(T-Tmin);
   f=exp(q_left+q_right);
   prob_left=( exp(q_left)/T )/f;
   prob_right=( exp(q_right)/T )/f; */
   if (q_left +rnd(-0.01,0.01) <= q_right)
	   cur_action = 1;
   else
      cur_action = 0;

   return cur_action;
}


double rnd(double low_bound, double hi_bound)
/* rnd scales the output of the system random function to the
 * range [low_bound, hi_bound].
 */
{
   double highest = 99999;
   return (rand() / highest) * (hi_bound - low_bound) + low_bound;
}


void reset_controller(void)
{
   cur_state = prev_state = -1;
   cur_action = prev_action = -1;   /* "null" action value */
}


/* The following routine was written by Rich Sutton and Chuck Anderson,
      with translation from FORTRAN to C by Claude Sammut  */

/*----------------------------------------------------------------------
   get_box:  Given the current state, returns a number from 0 to 359
  designating the region of the state space encompassing the current state.
  Returns a value of -1 if a failure state is encountered.
----------------------------------------------------------------------*/

#define one_degree 0.0174532	/* 2pi/360 */
#define six_degrees 0.1047192
#define twelve_degrees 0.2094384
#define nine_degrees 0.15707963267949
#define fifty_degrees 0.87266
#define ninty_two__degrees 1.60570291183478

int get_box(double x, double x_dot, double theta, double theta_dot)
{
  int box=0;

  if (x < -2.4 ||
      x > 2.4  ||
      theta < -twelve_degrees ||
      theta > twelve_degrees)          return(-1); /* to signal failure */

  if (x < -0.8)  		       box = 0;
  else if (x < 0.8)     	       box = 1;
  else		    	               box = 2;

  if (x_dot < -0.5) 		       ;
  else if (x_dot < 0.5)                box += 3;
  else 			               box += 6;

  if (theta < -nine_degrees) 	       ;
  if (theta < -six_degrees) 	       box += 9;
  else if (theta < -one_degree)        box += 18;
  else if (theta < 0) 				   box += 27;
  else if (theta < one_degree) 	       box += 36;
  else if (theta < six_degrees)        box += 45;
  else if (theta < nine_degrees)        box += 54;
  else	    						   box += 63;

  if (theta_dot < -ninty_two__degrees) 	;
  else if (theta_dot < -fifty_degrees)		box += 72;
  else if (theta_dot < fifty_degrees)		box += 144;
  else if (theta_dot < ninty_two__degrees)  box += 216;
  else		                                box += 288;

  return(box);
}

static int Bin_vector[8]={1,2,4,8,16,32,64,128};
double elman(int state,int action)
{
	static double Q;
	int i,j,r;
	/* Generate input value */
	/* Convert state to binary */
	r=state;
	for (i=(L1-L2-2);i>=0;i--)
	{	X[i]=0.0;
		if( r - Bin_vector[i] >= 0)
		{	r -= Bin_vector[i];
			X[i]=1.0;
		}
	}
	if (action==0)
		X[L1-L2-1]= 0.0;  /* 0 ~ 1 */
	else
		X[L1-L2-1]= 1.0;

	for (i=(L1-L2);i<L1;i++)
		X[i]=Y1[i-(L1-L2)];  /* recurren feed back */
	
	/* Forward get value */
	for (i=0;i<L2;i++)
	{	Y1[i]=0.0;
		for (j=0;j<L1;j++)
		{	Y1[i]+=W1[i][j]*X[j];
		}
		Y1[i]= sigmod( Y1[i]+B1[i] );
	}
	Y2=0.0;
	for (i=0;i<L2;i++)
		Y2 += W2[i]*Y1[i];
	Y2+=B2;
	Q=Y2;
	return Q;
}

#define learning_rate 1
void BP(int state,int action,double reinf)
{
	int c;
	int i,j;
	double q_target,output,E,delta_o,delta_y[L2] ;
	/* Get q_target */
	q_left=elman(cur_state,0);
	q_right=elman(cur_state,1);
	q_target = reinf + GAMMA * maxium(q_left,q_right);
		output=elman(state,action);
		/* Calculate error signal */
		E= 0.5*sqr(q_target-output);
		delta_o = (q_target - output) ;
		for (i=0;i<L2;i++)
			delta_y[i] = (W2[i] * delta_o * (1-Y1[i]) * Y1[i]) ;
		/* Adjust Weight */
		for (i=0;i<L2;i++)
		{	W2[i]+= (learning_rate * delta_o * Y1[i]);
		}
		B2+= learning_rate * delta_o;
		for (i=0;i<L2;i++)
		{	for (j=0;j<L1;j++)
			{	W1[i][j]+= (learning_rate * delta_y[i] * X[j]);
			}
			B1[i]+=(learning_rate * delta_y[i]);
		}
	
	if (get_box(x,x_dot,theta,theta_dot)==-1) /* if now the cart-pole fail */
	{	
		state=cur_state;
		action=cur_action;
		q_target = (1-ALPHA)*elman(state,action) + ALPHA*reinf;
			output=elman(state,action);
			/* Calculate error signal */
			delta_o = (q_target - output) ;
			for (i=0;i<L2;i++)
				delta_y[i] = (W2[i] * delta_o * (1-Y1[i]) * Y1[i]) ;
			/* Adjust Weight */
			for (i=0;i<L2;i++)
			{	W2[i]+= (learning_rate * delta_o * Y1[i]);
			}
			B2+= learning_rate * delta_o;
			for (i=0;i<L2;i++)
			{	for (j=0;j<L1;j++)
				{	W1[i][j]+= (learning_rate * delta_y[i] * X[j]);
				}
				B1[i]+=(learning_rate * delta_y[i]);
			}
	}

}


double sigmod(double x)
{
	return 1/(1+exp(-x)) ;
}

void initial_network()
{
	int i,j;
	for (i=0;i<L2;i++)
	{	W2[i]=rand()/(double)RAND_MAX-0.5;
		B1[i]=rand()/(double)RAND_MAX-0.5;
		Y1[i]=rand()/(double)RAND_MAX-0.5;
	}

	for (i=0;i<L2;i++)
		for (j=0;j<L1;j++)
			W1[i][j]=rand()/(double)RAND_MAX-0.5;

	B2=rand()/(double)RAND_MAX-0.5; 
}

double maxium(double a,double b)
{
	if (a>b)
		return a;
	else 
		return b;
}


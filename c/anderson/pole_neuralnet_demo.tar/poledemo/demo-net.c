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
#include "xg.h"
#include <math.h>
#include <sys/types.h>
#include <sys/timeb.h>

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

float state[4] = {0.0, 0.0, 0.0, 0.0};

float cart_mass = 1.;
float pole_mass = 0.1;
float pole_half_length = 0.5;
float force_mag = 10.;
/*float tau = 0.02;*/
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
double a[5][5], b[5], c[5], d[5][5], e[5], f[5], w[4];
double x[5], x_old[5], y[5], y_old[5], v, v_old, z[5], p;
double r_hat, push, unusualness;

int statep = 1;		     /* t=update the state each step */
int perfp = 1;		     /*t=update the performance curve each step */

float act = 0.;  /* SET BY BUTTON PRESSES */

/*** Prototypes ***/
void update_pole(float state[4], float act);
void calc_derivs (float state[4], float act, float derivs[4]);
int force_direction ();
void show_state (Xg_context *context, float state[4], float act);
void draw_state (Xg_context *context, float state[4], float act);
void draw_track (Xg_context *context);
float scale (float v, float vmin, float vmax, int devmin, int devmax);
void reset_perf_curve ();
void draw_performance (Xg_context *context);
void line (Xg_context *context, float x1, float y1, float x2, float y2);
void rectangle (Xg_context *context, float x1, float y1, float x2, float y2);
void redraw(Widget w, Xg_context *context, caddr_t call_data);
void released(Widget w, Xg_context *context, XEvent *event );
void pressed(Widget w, Xg_context *context, XEvent *event );
void reset(Xg_context *context);
void init_args(int argc, char *argv[]);

Xg_color *black;
Xg_color *yellow;

float sign(float x) { return (x < 0) ? -1. : 1.;}

main(argc,argv)
     int argc;
     char *argv[];
{
  char a;
  Xg_context *context;
  XGCValues values;

  /*  printf("Delay (something like 4)? ");
      scanf("%d",&Delay); */

  if(*argv[1] == 'g') {
    context = xg_init_context("X",NULL, &argc, argv, "test");

    black = xg_color_name_xor(context,"black");
    yellow = xg_color_name_xor(context,"yellow");

    xg_set_viewport(context,0.,0.,1.,1.);
    xg_set_window(context,-2.5,-2.5,2.5,2.5);

    xg_add_redraw_function(context,redraw,(caddr_t) context);

    draw_track(context);
  }

  init_args(argc,argv);

  sleep(2);

  Run(context,atoi(argv[2]), atoi(argv[3]));
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
  if (*argv[1] == 'g')
    Graphics = 1;
  else
    Graphics = 0;
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

/**********************************************************************
 *
 **********************************************************************/
void update_pole(float state[4], float act)
{
  float derivs[4];
  int i;
 
  calc_derivs(state,act,derivs);

  for (i=0; i<4; i++)
    state[i] += tau * derivs[i];
}

/**********************************************************************
 *
 **********************************************************************/
void calc_derivs (float state[4], float act, float derivs[4])
{
  float force = force_mag * act;
  float xd = state[1];
  float th = state[2];
  float thd = state[3];
  float sinth = sin(th);
  float costh = cos(th);
  float total_mass = cart_mass + pole_mass;
  float thdsq = thd * thd;
  float thdd, xdd;
  float common;

      common = (force + pole_mass * pole_half_length * thdsq * sinth) 
	/ total_mass;
      thdd = (9.8 * sinth - costh * common) /
        (pole_half_length * (4./3. - pole_mass * costh * costh / total_mass));
      xdd = common - pole_mass * pole_half_length * thdd * costh / total_mass;

/**
  thdd =  ( 9.8 * sinth + 
	   + costh * (- force - pole_mass * pole_half_length *
		      thdsq * sinth +
		      fric_cart * sign(xd)) / total_mass
	   - fric_pole * thd / (pole_mass * pole_half_length))
          /
      (pole_half_length * ( 4./3. - pole_mass * costh * costh / total_mass));

  xdd = (force + pole_mass * pole_half_length * (thdsq * sinth - thdd * costh)
	 - fric_cart * sign(xd))  /   total_mass;
**/

  derivs[0] = xd;
  derivs[1] = xdd;
  derivs[2] = thd;
  derivs[3] = thdd;
}

/**********************************************************************
 *
 **********************************************************************/
void reset(Xg_context *context)
{
  int i;
  for (i=0; i<4; i++)
    state[i] = 0.;

  not_first = 0;

  draw_track(context);
}

/**********************************************************************
 *
 **********************************************************************/

float old_state[4];
float old_act;

void show_state (Xg_context *context, float state[4], float act)
{
  int i;

  if (not_first) {
    draw_state(context,old_state, old_act);
/*    draw_track(context);*/
  }

  draw_state(context,state, act);

  for (i=0; i<4; i++)
    old_state[i] = state[i];

  old_act = act;
  not_first = 1;
}

/**********************************************************************
 *
 **********************************************************************/

float pole_length = 1.5;
float arrow_length = 0.2;

void draw_state (Xg_context *context, float state[4], float act)
{
  float x = state[0];
  float th = state[2];
  float arrow_tip = x + arrow_length * sign(act);
  float arrow_dx = -.05 * sign(act);

  /* Draw the cart */
  xg_fill_rectangle(context, x-0.2, -0.2, x+0.2, 0., yellow);

  /* Now draw a vector showing applied force.*/
  if (act != 0.) {
    xg_draw_line(context, x,-0.1, arrow_tip, -0.1, yellow);
    xg_draw_line(context, arrow_tip, -0.1, arrow_tip+arrow_dx, -0.1 + arrow_dx,
		 yellow);
    xg_draw_line(context, arrow_tip, -0.1, arrow_tip+arrow_dx, -0.1 - arrow_dx,
		 yellow);
  }

  /* Now the pole */
  xg_draw_line(context, x, 0., 
	       x + sin(th) * pole_length, pole_length * cos(th), yellow);
  xg_flush(context);
}

/**********************************************************************
 *
 **********************************************************************/
void draw_track (Xg_context *context)
{
  Xg_point points[8] = 
    {-2.5, -0.4,  2.5, -0.4,  2.5, 0.,  2.3, 0.,  2.3, -0.2,
       -2.3, -0.2,  -2.3, 0.,  -2.5, 0.};

  xg_clear(context);

  xg_fill_polygon(context,points,8,xg_color_name(context,"black"));

  not_first = 0;
}

/**********************************************************************
 * This function is called whenever a window associated with a particular
 * context is re_exposed or has been resized.  It must redraw the skeleton
 * of your dynamic display.
 **********************************************************************/
void redraw(Widget w, Xg_context *context, caddr_t call_data)
{
  draw_track(context);
}

/***-----***/
/****************************************************************/

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

  for(i = 0; i < 4; i ++) {
    w[i] = randomdef * 0.2 - 0.1;
  }
}

/**********************************************************************/

readweights(filename)
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


writeweights()
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

/*      the_system_state.cart_pos = 0.;
      the_system_state.cart_vel = 0.;
      the_system_state.pole_pos = 0.;
      the_system_state.pole_vel = 0.;
*/
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

Run(context, num_trials, sample_period)
    Xg_context *context;
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
      if (Graphics)
	if (fmod((float)j,500.0) == 0)
	  xg_process_events(context);

      Cycle(context,1,1);
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

  writeweights();

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

Cycle(context, learn_flag, bp_learn)
     Xg_context *context;
     int learn_flag;
     int bp_learn;
{
  int i, j, k;
  double sum, factor1, factor2;
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

  state[0] = the_system_state.cart_pos;
  state[1] = the_system_state.cart_vel;
  state[2] = the_system_state.pole_pos;
  state[3] = the_system_state.pole_vel;

  /*printf("p %.1f s %.2f %.2f %.2f %.2f\n",push,state[0],state[1],state[2],state[3]); */

  if(Graphics) {
    int delay;
    
    show_state(context,state,sign(push));
    usleep(10000);
    /*   for (delay=0; delay<10^Delay; delay++) ;*/
  }

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
  if(bp_learn) {
    backprop(push);
  }
}

backprop(double target_push) {
  int i = 0;
  double push = 0.0, sum = 0.0;
  
  // forward prop
  for(i = 0; i < 4; i ++) {
    sum += w[i]*state[i];
  }
  push = 1.0 / (1.0 + exp(-sum));
  double error = (push - target_push);
  double gradient = 0.0, learning_rate = 0.1;

  // backward prop
  double factor;
  for(i = 0; i< 4; i ++) {
    factor = learning_rate * gradient;
    w[i] += factor;
  }
}

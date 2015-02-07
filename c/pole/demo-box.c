#include "xg.h"
#include "math.h"

float state[4] = {0.0, 0.0, 0.0, 0.0};

float cart_mass = 1.;
float pole_mass = 0.1;
float pole_half_length = 0.5;
float force_mag = 10.;
float tau = 0.005;
float fric_cart = 0.00005;
float fric_pole = 0.005;
int not_first = 0;
int trial_length = 0;

int statep = 1;		     /* t=update the state each step */
int perfp = 1;		     /*t=update the performance curve each step */
int step = 0;

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

float sign(float x) { return (x < 0) ? -1. : 1.;}

Xg_color *black;
Xg_color *yellow;

main(argc,argv)
     int argc;
     char *argv[];
{
  char a;
  Xg_context *context;
  XGCValues values;
  int ii;

  context = xg_init_context("X",NULL, &argc, argv, "test");

  xg_set_background_name(context,"slateblue");
  black = xg_color_name_xor(context,"black");
  yellow = xg_color_name_xor(context,"yellow");
  xg_set_color_name_xor(context, "black");

/*
  XtAddEventHandler(context->canvas,
		    ButtonReleaseMask, FALSE, released, context );
  XtAddEventHandler(context->canvas,
		    ButtonPressMask, FALSE, pressed, context );
*/

/*  xg_set_viewport(context,0.,0.,1.,1.);*/
  xg_set_window(context,-2.5,-2.5,2.5,2.5);


  xg_add_redraw_function(context,redraw,(caddr_t) context);

/*  XtAppMainLoop(context_>app_context);*/

  draw_track(context);

/*
  XGrabButton(XtDisplay(context->canvas), AnyButton, AnyModifier,
	      XtWindow(context->canvas), TRUE, 
	      ButtonPressMask | ButtonMotionMask | ButtonReleaseMask,
	      GrabModeAsync, GrabModeAsync,
	      XtWindow(context->canvas),
	      NULL);
*/

  sleep(2);
  xg_process_events(context);

  while(1) {
    Window root, child;
    int x, y, wx, wy, i;
    unsigned int mask;

    usleep(10000);
    /*    for (ii=0; ii< 10000000; ii++);*/

    if (fmod((float)step,500.0) == 0)
      xg_process_events(context);

    step++;

    XQueryPointer(XtDisplay(context->canvas), XtWindow(context->canvas),
		  &root, &child, &x,&y, &wx, &wy, &mask);

    act = 0.;
    if (mask & Button1Mask)
      act = -1.;
    else if (mask & Button3Mask)
      act = 1.;
    else if (mask & Button2Mask) {
      reset(context);
      step = 0;
    }
   
    update_pole(state,act);
    show_state(context,state,act);
}


/*
  while(TRUE) {

    xg_process_events(context);

    update_pole(state,act);
    show_state(context,state,act);    
  }
*/
}
/**********************************************************************
 *
 **********************************************************************/
void pressed(Widget w, Xg_context *context, XEvent *event )
{

  switch (event->xbutton.button) {
  case 1: act = -1; break;
  case 2: reset(context); break;
  case 3: act = 1; break;
  default: act = 0;
  }
}  
/**********************************************************************
 *
 **********************************************************************/
void released(Widget w, Xg_context *context, XEvent *event )
{
  act = 0;
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

  common = (force + pole_mass * pole_half_length * thdsq * sinth
	    - fric_cart * sign(xd))
    / total_mass;

  thdd = (9.8 * sinth - costh * common
	  - fric_pole * thd / (pole_mass * pole_half_length)) /
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
  xg_fill_rectangle(context, x-0.2, -0.2,
		    x+0.2, 0., yellow);

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
}

/**********************************************************************
 *
 **********************************************************************/
void draw_track (Xg_context *context)
{
  Xg_point points[8] = 
    {-2.5, -0.4,  2.5, -0.4,  2.5, 0.,  2.3, 0.,  2.3, -0.2,
       -2.3, -0.2,  -2.3, 0.,  -2.5, 0.};

  char *s = "Buttons:  Left - Push Left, Middle - PANIC, Right - Push Right";
  int dx,dy;

  xg_clear(context);

  xg_fill_polygon(context,points,8,xg_color_name(context,"black"));

  dx = xg_dev_x(context,-1.5);
  dy = xg_dev_y(context,-1.8);

  XDrawString(XtDisplay(context->canvas),XtWindow(context->canvas),
	      context->gc, dx,dy, s, strlen(s));

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


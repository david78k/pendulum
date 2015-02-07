#include "xg.h"
#include <Xm/DrawingA.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <X11/Xutil.h>
#include <math.h>

Xg_color black;

/**********************************************************************
 * This function is called whenever a window associated with a particular
 * context is re-exposed or has been resized.  It must redraw the skeleton
 * of your dynamic display.
 **********************************************************************/
void redraw(Xg_context *context)
{
   xg_clear(context);
   xg_draw_line(context,.1, .1, .9, .9,xg_color_name(context,"black"));
 
   xg_draw_circle(context,.4,.4,.2,xg_color_name(context,"black"));
   xg_fill_circle(context,.2,.2,.1,xg_color_name(context,"black"));
   xg_fill_circle(context,.8,.8,.1,xg_color_name(context,"white"));

}

void redraw2(Xg_context *context)
{
  double data[] = {.1,.2,.5,.4,.3,.3,.6,.7,.9,.5};

  xg_draw_text(context,.5,.5,"Hello!!",xg_font_name(context,"lucidasanstypewriter-bold-24"));
  xg_draw_line(context,0., 0. , .9, .9,xg_color_rgb(context,100,0,100));

  xg_draw_corner(context,.2,.2,.1,90,xg_color_name(context,"black"));
  xg_draw_corner(context,.2,.2,.3,90,xg_color_name(context,"black"));
  xg_fill_circle(context,.0,.0,.2,xg_color_rgb(context,255,0,0));
  xg_fill_circle(context,.8,.8,.1,xg_color_rgb(context,255,255,0));

  xg_set_window(context,0.,0.,11.,1.);
  xg_ploty(context,data,10,1,xg_color_rgb(context,255,0,255));

}

main(argc,argv)
     int argc;
     char *argv[];
 {
  Xg_context *context,*context2,*context3,*context4;
  int   j,k,l,junk;
  double i;
  int   sign[2],inc;
  double xoff[101],yoff[101];

  if (argc>1 && 0 == strcmp(argv[1],"X")) {
    context = xg_init_context("X", NULL,  &argc, argv, "test");
  } else if (argc>1 && 0 == strcmp(argv[1],"idraw")) {
    context = xg_init_context("idraw", NULL,  &argc, argv, "test");
  } else {
    printf("Usage:   %s X\n",argv[0]);
    exit(1);
  }
  xg_set_viewport(context,0.5,0.5,1.,1.);
  redraw2(context);
  xg_flush(context);

  while(getchar()!=' ');

  xg_close_context(context);


/*
  XMapWindow(XtDisplay(context->canvas),XtWindow(context->canvas));
*/
}

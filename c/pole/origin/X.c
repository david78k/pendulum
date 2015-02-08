/**********************************************************************
 **********************************************************************
 *      X.c:  Device routines for X11.  Uses Xt Intrinsics and Xlib.
 **********************************************************************
 **********************************************************************/

#include "xg.h"
#include <Xm/DrawingA.h>
#include <Xm/MainW.h>

/***prototypes***/
void X_init(Xg_context *context, XtPointer device_info,
		int *argc, char *argv[], char *name);
void X_init_within_widget(Xg_context *context, Widget widget,
			  int *argc, char *argv[], char *name);
void X_close(Xg_context *context);
void X_get_max_width_height(Xg_context *context, Dimension *width, 
			    Dimension *height) ;
void X_install_viewport(Xg_context *context);
void X_clear(Xg_context *context);
void X_draw_point(Xg_context *context, int dx, int dy, Xg_color *color);
void X_draw_line(Xg_context *context, int dx1, int dy1, int dx2, int dy2,
		 Xg_color *color);
void X_draw_rectangle(Xg_context *context, int dx1, int dy1, int dx2, int dy2,
		 Xg_color *color);
void X_fill_rectangle(Xg_context *context, int dx1, int dy1, int dx2, int dy2,
		 Xg_color *color);
void X_draw_polygon(Xg_context *context, XPoint *dev_points, int n, 
		    Xg_color *color);
void X_fill_polygon(Xg_context *context, XPoint *dev_points, int n, 
		    Xg_color *color);
void X_draw_corner(Xg_context *context, int dx, int dy, int dradius, 
		   int angle, Xg_color *color);
void X_draw_circle(Xg_context *context, int dx, int dy, int dradius, 
		       Xg_color *color);
void X_fill_circle(Xg_context *context, int dx, int dy, int dradius, 
		       Xg_color *color);
void X_draw_text(Xg_context *context, int dx, int dy, char *txt, Xg_font *fontinfo);
void X_prolog(FILE *file, char *s);

void xg_load_X_functions(Xg_context *context)
{
  context->init = &X_init;
  context->new_canvas = &X_init_within_widget;
  context->close = &X_close;
  context->get_max_width_height = &X_get_max_width_height;
  context->install_viewport = &X_install_viewport;
  context->clear = &X_clear;
  context->draw_point = &X_draw_point;
  context->draw_line = &X_draw_line;
  context->draw_rectangle = &X_draw_rectangle;
  context->fill_rectangle = &X_fill_rectangle;
  context->draw_polygon = &X_draw_polygon;
  context->fill_polygon = &X_fill_polygon;
  context->draw_corner = &X_draw_corner;
  context->draw_circle = &X_draw_circle;
  context->fill_circle = &X_fill_circle;
  context->draw_text = &X_draw_text;
}  

void X_init(Xg_context *context, XtPointer device_info,
		int *argcp, char *argv[], char *name)
{
  Xg_context      *old_context;
  int             n = 0;
  XGCValues       values;
  XColor          color;
  Arg 		  wargs[20];

  XSetWindowAttributes setwinattr; /* page 93, O'Reilly Vol. 1 */
  XEvent          event;

  if (device_info == NULL) {

    context->toplevel =
      XtAppInitialize(&(context->app_context),
		      name, NULL, 0, argcp, argv, NULL, NULL, 0);


   n = 0;
   XtSetArg(wargs[n],XmNx,0); n++;
   XtSetArg(wargs[n],XmNy,0); n++;
   XtSetArg(wargs[n],XmNwidth,400); n++;
   XtSetArg(wargs[n],XmNheight,300); n++;
   XtSetArg(wargs[n],XmNmarginWidth,0); n++;
   XtSetArg(wargs[n],XmNmarginHeight,0); n++;

  context->parent_widget = XtCreateManagedWidget("holder",
					    xmMainWindowWidgetClass,
					    context->toplevel,
					  wargs,n);

  n = 0;
  XtSetArg(wargs[n],XmNx,0); n++;
  XtSetArg(wargs[n],XmNy,0); n++;
  XtSetArg(wargs[n],XmNwidth,600); n++;
  XtSetArg(wargs[n],XmNheight,600); n++;

  context->canvas = XtCreateManagedWidget("canvas",
					    xmDrawingAreaWidgetClass,
					    context->parent_widget,
					  wargs,n);



  XtAddCallback(context->canvas, XmNresizeCallback, 
		(XtCallbackProc)xg_calc_transform_on_expose_event, context);

  n = 0;
  XtSetArg(wargs[n],XmNforeground,&(color.pixel)); n++;
  XtGetValues(context->canvas, wargs,n); 
  values.foreground = color.pixel;

  context->gc = XCreateGC(XtDisplay(context->canvas),
		 DefaultRootWindow(XtDisplay(context->canvas)),
		 GCForeground, &values);

  XtRealizeWidget(context->toplevel);

  setwinattr.backing_store = WhenMapped;
  XChangeWindowAttributes(XtDisplay(context->canvas),
			  XtWindow(context->canvas),
			  CWBackingStore, &setwinattr);

    while(!XCheckTypedWindowEvent(XtDisplay(context->canvas),
				XtWindow(context->canvas),12,&event));
}
  else
 {
    old_context = (Xg_context *) device_info;
    context->toplevel = old_context->toplevel;
    context->parent_widget = old_context->parent_widget;
    context->canvas = old_context->canvas;
    context->app_context = old_context->app_context;
    n = 0;
    XtSetArg(wargs[n],XmNforeground,&(color.pixel)); n++;
    XtGetValues(context->canvas, wargs,n); 
    values.foreground = color.pixel;

    context->gc = XCreateGC(XtDisplay(context->canvas),
			    DefaultRootWindow(XtDisplay(context->canvas)),
			    GCForeground, &values);

  }
}	

void X_init_within_widget(Xg_context *context, Widget widget, int *argcp,
				 char *argv[], char *name)
{
  int             n = 0;
  XGCValues       values;
  XColor          color;
  unsigned int    width,height;
  Arg 		  wargs[20];
  
  XSetWindowAttributes setwinattr; /* page 93, O'Reilly Vol. 1 */
  XEvent          event;
  
  if (widget==NULL) return;

  context->parent_widget = widget;

  width=DisplayWidth(XtDisplay(widget),0);
  height=DisplayHeight(XtDisplay(widget),0);

  n = 0;
  XtSetArg(wargs[n],XmNx,0); n++;
  XtSetArg(wargs[n],XmNy,0); n++;
  XtSetArg(wargs[n],XmNwidth,width); n++;
  XtSetArg(wargs[n],XmNheight,height); n++;
  
  context->canvas = XtCreateManagedWidget("new_canvas",
					  xmDrawingAreaWidgetClass,
					  context->parent_widget,
					  wargs,n);

  XtAddCallback(context->canvas, XmNresizeCallback, 
		(XtCallbackProc)xg_calc_transform_on_expose_event, context);

  
  n = 0;
  XtSetArg(wargs[n],XmNforeground,&(color.pixel)); n++;
  XtGetValues(context->canvas, wargs,n); 
  values.foreground = color.pixel;
  
  context->gc = XCreateGC(XtDisplay(context->canvas),
			  DefaultRootWindow(XtDisplay(context->canvas)),
			  GCForeground, &values);
  
  setwinattr.backing_store = WhenMapped;
  XChangeWindowAttributes(XtDisplay(context->canvas),
			  XtWindow(context->canvas),
			  CWBackingStore, &setwinattr);

  while(!XCheckTypedWindowEvent(XtDisplay(context->canvas),
				XtWindow(context->canvas),12,&event));
  
}	

void X_close(Xg_context *context)
{
/* free the widget */
}

void X_get_max_width_height(Xg_context *context, Dimension *width, 
			    Dimension *height) 
{
  XtVaGetValues(context->canvas, XmNwidth, width, 
		XmNheight, height, NULL);
}

void X_install_viewport(Xg_context *context)
{
  double y1=context->viewport.y1;

    context->vwidth=context->viewport.x2-context->viewport.x1;
    context->vheight=context->viewport.y2-context->viewport.y1;
    
    context->viewport.x1=context->viewport.x1;
    context->viewport.x2=context->viewport.x2;
  /* Flip sense of y for X */
    context->viewport.y1=1-context->viewport.y1;
    context->viewport.y2=1-context->viewport.y2;

}


void X_clear(Xg_context *context)
{
  XClearWindow(XtDisplay(context->canvas), XtWindow(context->canvas));
}


void X_draw_point(Xg_context *context, int dx, int dy, Xg_color *color)
{
  Widget          canvas;
  GC              gc = (color == NULL) ? context->gc : color->gc;
  Dimension       x, y, width, height;

  canvas = context->canvas;

  XDrawPoint(XtDisplay(canvas), XtWindow(context->canvas), gc,
	     (Dimension) dx, (Dimension) dy);
}

void X_draw_line(Xg_context *context, int dx1, int dy1, int dx2, int dy2,
		 Xg_color *color)
{
  Widget          canvas = context->canvas;
  GC              gc = (color == NULL) ? context->gc : color->gc;

  XDrawLine(XtDisplay(canvas), XtWindow(canvas), gc,
	dx1, dy1, dx2, dy2);
}

void X_draw_rectangle(Xg_context *context, int dx1, int dy1, int dx2, int dy2,
		    Xg_color *color)
{
  Widget          canvas = context->canvas;
  GC              gc = (color == NULL) ? context->gc : color->gc;
  static int tmp;

  if (dy2 < dy1) {
    tmp = dy2;
    dy2 = dy1;
    dy1 = tmp;
  }
  if (dx2 < dx1) {
    tmp = dx2;
    dx2 = dx1;
    dx1 = tmp;
  }
  XDrawRectangle(XtDisplay(canvas), XtWindow(canvas), gc,
	       dx1, dy1, dx2-dx1, dy2-dy1);
}

void X_fill_rectangle(Xg_context *context,  int dx1, int dy1, int dx2, int dy2,
		    Xg_color *color)
{
  Widget          canvas = context->canvas;
  GC              gc = (color == NULL) ? context->gc : color->gc;
  static int tmp;

  if (dy2 < dy1) {
    tmp = dy2;
    dy2 = dy1;
    dy1 = tmp;
  }
  if (dx2 < dx1) {
    tmp = dx2;
    dx2 = dx1;
    dx1 = tmp;
  }
  XFillRectangle(XtDisplay(canvas), XtWindow(canvas), gc,
	       dx1, dy1, dx2-dx1, dy2-dy1);
}

void X_draw_polygon(Xg_context *context, XPoint *dev_points, int n, 
		    Xg_color *color)
{
  Widget          canvas = context->canvas;
  GC              gc = (color == NULL) ? context->gc : color->gc;

  XDrawLines(XtDisplay(canvas), XtWindow(canvas), gc,
	       dev_points, n, CoordModeOrigin);
}

void X_fill_polygon(Xg_context *context, XPoint *dev_points, int n, 
		    Xg_color *color)
{
  Widget          canvas = context->canvas;
  GC              gc = (color == NULL) ? context->gc : color->gc;

  XFillPolygon(XtDisplay(canvas), XtWindow(canvas), gc,
	       dev_points, n, Complex, CoordModeOrigin);
}

void X_draw_corner(Xg_context *context, int dx, int dy, int dradius, 
		   int angle, Xg_color *color)
{
  Widget          canvas = context->canvas;
  GC              gc = (color == NULL) ? context->gc : color->gc;
  int             startx, starty, diffx, diffy;

  startx = dx - dradius;
  starty = dy - dradius;
  diffx = 2 * dradius;
  diffy = diffx;

  XDrawArc(XtDisplay(canvas), XtWindow(canvas), gc, startx, starty, 
	   diffx, diffy, 64 * angle, 64 * 90);
}

void X_draw_circle(Xg_context *context, int dx, int dy, int dradius, 
		   Xg_color *color)
{
  Widget          canvas = context->canvas;
  GC              gc = (color == NULL) ? context->gc : color->gc;
  int             startx, starty, diffx, diffy;

  startx = dx - dradius;
  starty = dy - dradius;
  diffx = 2 * dradius;
  diffy = diffx;

  XDrawArc(XtDisplay(canvas), XtWindow(canvas), gc, startx, starty, 
	   diffx, diffy, 0, 64 * 360);
}

void X_fill_circle(Xg_context *context, int dx, int dy, int dradius, 
		   Xg_color *color)
{
  Widget          canvas = context->canvas;
  GC              gc = (color == NULL) ? context->gc : color->gc;
  int             startx, starty, diffx, diffy;

  startx = dx - dradius;
  starty = dy - dradius;
  diffx = 2 * dradius;
  diffy = diffx;

  XFillArc(XtDisplay(canvas), XtWindow(canvas), gc, startx, starty, 
	   diffx, diffy, 0, 64 * 360);
}


void X_draw_text(Xg_context *context, int dx, int dy,char *txt, Xg_font *fontinfo)
{
  XmString str;

  str=XmStringCreate(txt,"ISO8859-1");

  XmStringDraw(XtDisplay(context->canvas),
	       XtWindow(context->canvas),
	       fontinfo->fontlist,str,fontinfo->gc,dx,dy,context->vwidth,
	       XmSTRING_DIRECTION_L_TO_R,XmALIGNMENT_BEGINNING,NULL);
}







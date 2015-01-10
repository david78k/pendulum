/**********************************************************************
 * xg.h: declarations for xg routines
 **********************************************************************/
#ifndef _XG_H
#define _XG_H

#include <Xm/Xm.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <math.h>

#define XG_COLORLIST_SIZE 10

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct corners {
  double           x1, y1, x2, y2;
} Xg_corners;

typedef struct Xg_point_struct {
  double           x;
  double           y;
} Xg_point;

typedef struct Xg_color_struct {
  GC              gc;
  double           r;
  double           g;
  double           b;
} Xg_color;

typedef struct Xg_font_struct {
  GC              gc;
  XmFontList      fontlist;
  char            *name;
} Xg_font;

typedef enum devices_defn {Xg_X, Xg_idraw} Xg_device;

typedef struct Xg_context_struct {
  XtAppContext  app_context;
  Widget        canvas;
  Widget        parent_widget;
  Widget        toplevel;
  GC            gc;
  double         fr;
  double         fg;
  double         fb;
  double         br;
  double         bg;
  double         bb;
  Position      x;                        /* upper left corner */
  Position      y;                        /* in terms of parents coords */
  Dimension     width;                    /* width of window  */
  Dimension     height;                   /* height of window */
  double         vwidth;
  double         vheight;
  double         offset_x;
  double         offset_y;
  double         scale_x;
  double         scale_y;
  Position      start_mouse_x;
  Position      start_mouse_y;
  Position  	x1;
  Position     	y1;
  Dimension     width1;
  Dimension     height1;
  Xg_corners    viewport;
  Xg_corners    window;
  XtPointer     redraw_data;
  Boolean       reinitialized;
  Xg_color      *colorlist[XG_COLORLIST_SIZE];
  FILE		*xgout;
  int	 	page;
  int           fonts;
  char          *name;
  char 		*device;
  XtPointer	device_info;
  Xg_device	dev;		           /* for fast checks */

  void          (*redraw) ();

  void		(*init)();                 /*Xg_context *context, char *device, 
				             XtPointer device_info, int *argc, char *argv[], 
				             char *name);*/
  void		(*new_canvas)();           /*Xg_context *context, char *device, 
				             XtPointer device_info, int *argc, char *argv[], 
				             char *name);*/
  void		(*close)();                /*(Xg_context *context);*/
  void		(*get_max_width_height)(); /*Xg_context *context, Dimension *width, 
					      Dimension *height);*/
  void		(*install_viewport)();     /*Xg_context *context);*/
  void		(*clear)();                /*Xg_context *context);*/
  void		(*draw_point)();           /*Xg_context *context, int dx, int dy, 
				             Xg_color *color);*/
  void		(*draw_line)();            /*Xg_context *context, int dx1, int dy1, int dx2, 
				             int dy2, Xg_color *color);*/
  void		(*draw_rectangle)();            /*Xg_context *context, int dx1, int dy1, int dx2, 
				             int dy2, Xg_color *color);*/
  void		(*fill_rectangle)();            /*Xg_context *context, int dx1, int dy1, int dx2, 
				             int dy2, Xg_color *color);*/
  void		(*draw_polygon)();         /*Xg_context *context, XPoint *dev_points, 
				             int n, Xg_color *color);*/
  void		(*fill_polygon)();         /*Xg_context *context, XPoint *dev_points, 
				             int n, Xg_color *color);*/
  void		(*draw_corner)();          /*Xg_context *context, int dx, int dy,
				             int dradius, int angle, Xg_color *color);*/
  void		(*draw_circle)();          /*Xg_context *context, int dx, int dy,
				             int dradius, Xg_color *color);*/
  void		(*fill_circle)();          /*Xg_context *context, int dx, int dy, 
				             int dradius, Xg_color *color);*/
  void		(*draw_text)();            /*Xg_context *context, int dx1, int dy1,char *txt
					     Xg_font *fontinfo */
}  Xg_context;

struct xg_moving_rect_struct {
   Position  x;
   Position  y;
   Dimension width;
   Dimension height;
   GC        gc;
};

/* Prototypes */

Xg_context *xg_init_context(char* device, XtPointer device_info,
		int *argc, char *argv[], char *name);

Xg_context *xg_init_context_within_widget(char *device, Widget widget,
		int *argc, char *argv[], char *name);

void xg_copy_context(Xg_context *old_context, Xg_context *new_context);

void xg_set_reinitialized(Xg_context *context);

Boolean xg_reinitialized(Xg_context *context);

void xg_set_background_name(Xg_context *context, char *name);

void xg_add_redraw_function(Xg_context *context, void (*func)(), XtPointer data);

void xg_dummy_redraw(Widget w, XtPointer data, XtPointer call_data);

void xg_calc_transform(Xg_context *context);

void xg_calc_transform_on_expose_event(Widget w, Xg_context *context, 
				  XtPointer call_data);

void xg_redraw(Xg_context *context);

void xg_process_events(Xg_context *context);

void xg_process_events_loop(Xg_context *context);

void xg_flush(Xg_context *context);

int xg_dev_x(Xg_context *context, double x);

int xg_dev_y(Xg_context *context, double y);

void xg_dev_point(Xg_context *context, double x, double y, int *dx, int *dy);

void xg_dev_diffs(Xg_context *context, double x_diff, double y_diff,
		     int *dx_diff, int *dy_diff);

int xg_dev_x_diff(Xg_context *context, double x_diff);

int xg_dev_y_diff(Xg_context *context, double y_diff);

double xg_window_x(Xg_context *context, int dx);

double xg_window_y(Xg_context *context, int dy);

void xg_window_point(Xg_context *context, int dx, int dy, double *x, double *y);

void xg_window_diffs(Xg_context *context, int dx_diff, int dy_diff,
		     double *x_diff, double *y_diff);

int xg_window_x_diff(Xg_context *context, int dx_diff);

int xg_window_y_diff(Xg_context *context, int dy_diff);

void xg_set_viewport(Xg_context *context,double x1, double y1, double x2, double y2);

void xg_install_viewport(Xg_context *context);

void xg_set_window(Xg_context *context, double x1, double y1, double x2, double y2);

void xg_set_thickness(Xg_context *context, Xg_color *color, int thickness);

void xg_set_color_rgb(Xg_context *context, int r, int g, int b);

void xg_set_color_name(Xg_context *context, char *name);

void xg_set_color_name_xor(Xg_context *context, char *name);

Xg_font *xg_font_name(Xg_context *context, char *name);

Xg_color *xg_color_rgb(Xg_context *context, int r, int g, int b);

Xg_color *xg_color_name(Xg_context *context, char *name);

Xg_color *xg_color_name_xor(Xg_context *context, char *name);

void xg_clear(Xg_context *context);

void xgd_draw_text(Xg_context *context, int dx, int dy, char *txt,
		    Xg_font *fontinfo);

void xg_draw_text(Xg_context *context, double dx, double dy, char *txt,
		  Xg_font *fontinfo);

void xg_draw_point(Xg_context *context, double x, double y, Xg_color *color);
void xgd_draw_point(Xg_context *context, int dx, int dy, Xg_color *color);

void xg_draw_line(Xg_context *context, double x1, double y1, double x2, double y2,
	     Xg_color *color);

void xgd_draw_line(Xg_context *context, int dx1, int dy1, int dx2, int dy2,
	      Xg_color *color);

void xg_draw_rectangle(Xg_context *context, double x1, double y1, 
		       double x2, double y2, Xg_color *color);

void xgd_draw_rectangle(Xg_context *context, int dx1, int dy1, int dx2, int dy2,
			Xg_color *color);

void xg_fill_rectangle(Xg_context *context, double x1, double y1, double x2, double y2,
		       Xg_color *color);

void xgd_fill_rectangle(Xg_context *context, int dx1, int dy1, int dx2, int dy2,
			Xg_color *color);

void xg_draw_polygon(Xg_context *context, Xg_point *points, int n,
		     Xg_color *color);

void xgd_draw_polygon(Xg_context *context, XPoint *dev_points, int n,
		      Xg_color *color);

void xg_fill_polygon(Xg_context *context, Xg_point *points, int n, 
		     Xg_color *color);

void xgd_fill_polygon(Xg_context *context, XPoint *dev_points, int n, 
		      Xg_color *color);

void xg_draw_corner(Xg_context *context, double x, double y, double radius, 
	       int angle, Xg_color *color);

void xg_draw_circle(Xg_context *context, double x, double y, double radius, 
		    Xg_color *color);

void xgd_draw_circle(Xg_context *context, int dx, int dy, int dradius, 
		     Xg_color *color);

void xg_fill_circle(Xg_context *context, double x, double y, double radius, 
		    Xg_color *color);

void xgd_fill_circle(Xg_context *context, int dx, int dy, int dradius, 
		     Xg_color *color);

#define not_X(context)    ((context)->dev != Xg_X)


void xg_ploty(Xg_context *context, double  *points, 
	      int n, int ncurves, Xg_color *color);

#ifdef	__cplusplus
}
#endif


#endif

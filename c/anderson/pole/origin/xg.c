/**********************************************************************
 **********************************************************************
 *      xg.c:  xg routines, without device-specific routines.
 **********************************************************************
 **********************************************************************/

#include "xg.h"

/**********************************************************************
 * xg_init_context: Defines new toplevel window and a DrawingArea widget.
 *                Creates new xg_context structure for these widgets.
 *
 * Usage:
 *
 *	xg_context context;
 *
 *      context = xg_init_context("X",NULL, &argc,argv,"test");
 *      context = xg_init_context("X",parent_widget, &argc,argv,"test");
 *      context = xg_init_context("idraw","filename.idraw", NULL,NULL,NULL);
 *      context = xg_init_context("ps","filename.ps", NULL,NULL,NULL);
 **********************************************************************/
Xg_context *
xg_init_context(char* device, XtPointer device_info,
		int *argc, char *argv[], char *name)
{
  Xg_context      *context;

  context = (Xg_context *) malloc(sizeof(Xg_context));
  context->name = strdup(name);
  context->device = strdup(device);

  if (strcmp(device,"X") == 0)
    {
      context->dev = Xg_X;
      context->fr=context->fg=context->fb=0.00000;
      context->br=context->bg=context->bb=1.00000;
      xg_load_X_functions(context);
    } 
  else if (strcmp(device,"idraw") == 0)
    {
      context->dev = Xg_idraw;
      context->fr=context->fg=context->fb=0.00000;
      context->br=context->bg=context->bb=1.00000;
      xg_load_idraw_functions(context);
    } 
  else
    {
    printf("Don't know how do deal with device %s.",device);
    return;
  }
  
  (*context->init)(context,device_info,argc,argv,name);
  /*
   * Default viewport is entire bulletin board. Default window is 0 0 to 1 1
   */


  /* Build list of colors to be used by xgploty */

  
  if (XG_COLORLIST_SIZE > 0)
    context->colorlist[0] = xg_color_rgb(context,0,0,255);
  if (XG_COLORLIST_SIZE > 1)
    context->colorlist[1] = xg_color_rgb(context,0,127,0);
  if (XG_COLORLIST_SIZE > 2)
    context->colorlist[2] = xg_color_rgb(context,255,0,0);
  if (XG_COLORLIST_SIZE > 3)
    context->colorlist[3] = xg_color_rgb(context,0,191,191);
  if (XG_COLORLIST_SIZE > 4)
    context->colorlist[4] = xg_color_rgb(context,191,0,191);
  if (XG_COLORLIST_SIZE > 5)
    context->colorlist[5] = xg_color_rgb(context,191,191,0);
  if (XG_COLORLIST_SIZE > 6)
    context->colorlist[6] = xg_color_rgb(context,61,61,61);

  if (XG_COLORLIST_SIZE > 7)
    context->colorlist[7] = xg_color_rgb(context,0,0,255);
  if (XG_COLORLIST_SIZE > 8)
    context->colorlist[8] = xg_color_rgb(context,0,127,0);
  if (XG_COLORLIST_SIZE > 9)
    context->colorlist[9] = xg_color_rgb(context,255,0,0);
  if (XG_COLORLIST_SIZE > 10)
    context->colorlist[10] = xg_color_rgb(context,0,191,191);
  if (XG_COLORLIST_SIZE > 11)
    context->colorlist[11] = xg_color_rgb(context,191,0,191);
  if (XG_COLORLIST_SIZE > 12)
    context->colorlist[12] = xg_color_rgb(context,191,191,0);
  if (XG_COLORLIST_SIZE > 13)
    context->colorlist[13] = xg_color_rgb(context,61,61,61);

  /* Redraw function must be set before setting viewport and window. */

  /* xg_set_redraw_function(context,xg_dummy_redraw,NULL); */

  xg_set_window(context, 0., 0., 1., 1.);
  xg_set_viewport(context, 0., 0., 1., 1.);
  xg_set_reinitialized(context);
  return (context);
}

Xg_context *xg_init_context_within_widget(char *device,	Widget widget,
					  int *argcp, char *argv[], char *name)
{
  Xg_context      *context;

  context = (Xg_context *) malloc(sizeof(Xg_context));
  context->name = strdup(name);
  context->device = strdup(device);

  if (strcmp(device,"X") == 0)
    {
      context->dev = Xg_X;
      context->fr=context->fg=context->fb=0.00000;
      context->br=context->bg=context->bb=1.00000;
      xg_load_X_functions(context);
      (*context->new_canvas)(context,widget,argcp,argv,name);
    } 
  else
    {
    printf("Don't know how do deal with device %s.",device);
    return;
  }
  
  xg_set_window(context, 0., 0., 1., 1.);
  xg_set_viewport(context, 0., 0., 1., 1.);
  xg_set_reinitialized(context);

  return (context);
}

/**********************************************************************
 * xg_close_context: 
 **********************************************************************/
void xg_close_context(Xg_context *context)
{
  xg_flush(context);
  (*context->close)(context);

  free(context);
}

/**********************************************************************
 * xg_set_reinitialized: Sets this flag to TRUE.
 * Usage: xg_set_reinitialized(context)
 **********************************************************************/
void 
xg_set_reinitialized(Xg_context *context)
{
  context->reinitialized = TRUE;
}

/**********************************************************************
 * xg_reinitialized: Returns value of this flag.
 * Usage: if (xg_reinitialized(context)) { ... }
 **********************************************************************/
Boolean 
xg_reinitialized(Xg_context *context)
{
  return (context->reinitialized);
}

/**********************************************************************
 * xg_set_background_name: Changes the color of the background.
 * Usage: xg_set_background_name(context,"black");
 **********************************************************************/
void 
xg_set_background_name(Xg_context *context, char *name)
{
  Widget          canvas = context->canvas;
  Display        *display;
  int             screen;
  Colormap        cmap;
  XColor          color, exact;

  if (not_X(context)) return;

  display = XtDisplay(canvas);
  screen = DefaultScreen(display);
  cmap = DefaultColormap(display, screen);

  if (!XAllocNamedColor(display, cmap, name, &color, &exact)) {
    printf("Warning:  Couldn't allocate requested color\n");
    color.pixel = BlackPixel(display, screen);
  }
  context->br=(double)color.red/65535;
  context->bg=(double)color.green/65535;
  context->bb=(double)color.blue/65535;
  XtVaSetValues(canvas, XmNbackground, color.pixel, NULL);
}
/**********************************************************************
 * xg_add_redraw_function: Adds function to be Called when widget gets
 * 			expose event.
 * Usage: xg_add_redraw_function(context,func_name,func_data)
 **********************************************************************/
void 
xg_add_redraw_function(Xg_context *context, void (*func)(), XtPointer data)
{
  if (not_X(context)) return;

  context->redraw = func;
  context->redraw_data = data;
/*  XtAddCallback(context->canvas, XmNexposeCallback, func, data);*/
/* commented out because XmNexposeCallback doesn't exist??? (had used
   XmNexposeCallback */
}
/**********************************************************************
 * xg_dummy_redraw: Just a place holder.
 **********************************************************************/
void 
xg_dummy_redraw(Widget w, XtPointer data, XtPointer call_data)
{
}
/**********************************************************************
 * xg_calc_transform: Calculate scaling and translation
 * Usage: xg_calc_transform(context)
 **********************************************************************/
void 
xg_calc_transform(Xg_context *context)
{
  Dimension       width, height;

  (*(context->get_max_width_height))(context,&width,&height);

  context->width = width;
  context->height = height;

  context->scale_x = (double)context->vwidth*width/(context->window.x2-context->window.x1);
  context->scale_y = (double)context->vheight*height/(context->window.y2-context->window.y1);

  context->offset_x= (context->viewport.x1) * width;
  context->offset_y= (context->viewport.y1) * height;

}

/**********************************************************************
 * xg_calc_transform_on_expose_event
 **********************************************************************/
void 
xg_calc_transform_on_expose_event(Widget w,
                                  Xg_context *context, 
                                  XtPointer call_data)
{
  /* printf("Handling expose for context %s\n",context->name); */

  xg_install_viewport(context);
}

/**********************************************************************
 * xg_redraw: Clears drawing area, then redraws.
 * Usage: xg_redraw(context);
 **********************************************************************/
void 
xg_redraw(Xg_context *context)
{
  if (not_X(context)) return;

  (*context->redraw) (context->canvas, context->redraw_data, NULL);
}

/**********************************************************************
 * xg_process_events: Handles all pending events, then returns.
 *  Usage:  while(TRUE) {
 *              xg_process_events();
 *               <do your stuff>
 *          }
**********************************************************************/
void 
xg_process_events(Xg_context *context)
{
  if (not_X(context)) return;

  while (XtPending()) {
    XEvent          event;
    XtNextEvent(&event);
    XtDispatchEvent(&event);
  }
}
/**********************************************************************
 * xg_process_events_loop:  Just calls XtMainLoop().  Doesn't return.
 *  Usage:  xg_process_events_loop();
**********************************************************************/
void 
xg_process_events_loop(Xg_context *context)
{
  if (not_X(context)) return;

  XtAppMainLoop(context->app_context);
}
/**********************************************************************
* xg_dev_x: Converts normalized x coordinate into device x coordinate.
* Usage: dx = xgd_dev_x(context,x);
**********************************************************************/
int 
xg_dev_x(Xg_context *context, double x)
{
  return ((int) (0.5 + context->scale_x * (x-context->window.x1))+
	 context->offset_x);
}

/**********************************************************************
* xg_dev_y: Converts normalized y coordinate into device y coordinate.
* Usage: dy = xgd_dev_y(context,y);
**********************************************************************/
int 
xg_dev_y(Xg_context *context, double y)
{
  if (not_X(context)) {
    return (context->offset_y+(int)(0.5+context->scale_y*(y-context->window.y1)));
  }
 return (context->offset_y-(int)(0.5+context->scale_y*(y-context->window.y1)));

}

/**********************************************************************
* xg_dev_point: Converts window point into device point
* Usage: xg_dev_point(context,x,y,&dx,&dy)
**********************************************************************/
void 
xg_dev_point(Xg_context *context, double x, double y, int *dx, int *dy)
{
  *dx = xg_dev_x(context, x);
  *dy = xg_dev_y(context, y);
}
/**********************************************************************
xg_dev_x_diff: Converts normalized x difference into device x difference.

Usage:  dx_diff = xgd_dev_x_diff(context,x_diff);
**********************************************************************/
int 
xg_dev_x_diff(Xg_context *context, double x_diff)
{
  return ((int) (0.5 + x_diff * context->scale_x));
}
/**********************************************************************
xg_dev_y_diff: Converts normalized y difference into device y difference.

Usage:  dy_diff = xgd_dev_y_diff(context,y_diff);
**********************************************************************/
int 
xg_dev_y_diff(Xg_context *context, double y_diff)
{
  return ((int) (0.5 + y_diff * context->scale_y));
}
/**********************************************************************
xg_window_x_diff: Converts device x difference into window x difference.

Usage:  x_diff = xg_window_x_diff(context,dx_diff);
**********************************************************************/
int 
xg_window_x_diff(Xg_context *context, int dx_diff)
{
  return ((int)(dx_diff / context->scale_x));
}
/**********************************************************************
xg_window_y_diff: Converts device y difference into window y difference.

Usage:  y_diff = xg_window_y_diff(context,dy_diff);
**********************************************************************/
int 
xg_window_y_diff(Xg_context *context, int dy_diff)
{
  return ((int)(dy_diff / context->scale_y));
}
/**********************************************************************
 * xg_set_viewport: Repositions the context within the parent.
 * Usage:    xg_set_viewport(context,x1,y1,x2,y2);
 *	All values between -1 and 1.
 **********************************************************************/
void 
xg_set_viewport(Xg_context *context, double x1, double y1, double x2, double y2)
{

  context->vwidth=x2-x1;
  context->vheight=x2-y1;

  context->viewport.x1 = x1;
  context->viewport.y1 = y1;
  context->viewport.x2 = x2;
  context->viewport.y2 = y2;

  xg_install_viewport(context);
}
/**********************************************************************
 * xgd_set_viewport: Repositions the context within the parent.
 * Usage:    xgd_set_viewport(context,dx1,dy1,dx2,dy2);
 **********************************************************************/
void 
xgd_set_viewport(Xg_context *context, int dx1, int dy1, int dx2, int dy2)
{

  context->viewport.x1 = dx1 / (double) context->width;
  context->viewport.y1 = dy1 / (double) context->height;
  context->viewport.x2 = dx2 / (double) context->width;
  context->viewport.y2 = dy2 / (double) context->height;

  xg_install_viewport(context);
}
/**********************************************************************
 * xg_install_viewport: Sets geometry of canvas widget based on viewport.
 * Usage: xg_install_viewport(context)
**********************************************************************/
void 
xg_install_viewport(Xg_context *context)
{

  (*(context->install_viewport))(context);

  /*
   * printf("Intalling viewport for %s x=%d width=%d y=%d height=%d\n",
   * context->name, (Dimension) (width * x1), (Dimension) (width * (x2 -
   * x1)), (Dimension) (height * (1 - y2)), (Dimension) (height * (y2 -
   * y1)));
   */
  xg_calc_transform(context);
}
/**********************************************************************
 * xg_set_window: Sets scales
 * Usage:    xg_set_window(context,x1,y1,x2,y2);
 **********************************************************************/
void 
xg_set_window(Xg_context *context, double x1, double y1, double x2, double y2)
{
  context->window.x1 = x1;
  context->window.y1 = y1;
  context->window.x2 = x2;
  context->window.y2 = y2;

  xg_calc_transform(context);

  /* xg_redraw(context); */
}
/**********************************************************************
 * xg_set_thickness: Changes the thickness in the gc.
 * Usage: xg_set_thickness(context,thickness);
 **********************************************************************/
void
xg_set_thickness(Xg_context *context, Xg_color *color, int thickness)
{
/*  int devx = xg_dev_x_diff(context,thickness);
  int devy = xg_dev_y_deff(context,thickness);
  int dev_thick = devx < devy ? devx : devy;
*/

  if (not_X(context)) return;

  XSetLineAttributes(XtDisplay(context->canvas), color->gc, thickness,
		     LineSolid, CapButt, JoinMiter);
}
/**********************************************************************
* xg_color_by_rgb: Sets the color of the graphics context
* Usage: xg_color_by_rgb(context,r,g,b)
**********************************************************************/
void 
xg_set_color_rgb(Xg_context *context, int r, int g, int b)
{
  Display        *display;
  int             screen;
  Colormap        cmap;
  XColor          color;
  GC              gc;
  XGCValues       values;

  context->fr=r/255;
  context->fg=g/255;
  context->fb=b/255;

  if (not_X(context)) return;

  display = XtDisplay(context->canvas);
  screen = DefaultScreen(display);
  cmap = DefaultColormap(display, screen);
  gc = context->gc;

  color.red = (unsigned short) context->fr *65535;
  color.green = (unsigned short) context->fg *65535;
  color.blue = (unsigned short) context->fb *65535;

  if (!XAllocColor(display, cmap, &color)) {
    printf("Warning:  Couldn't allocate requested color\n");
    color.pixel = BlackPixel(display, screen);
  }
  values.foreground = color.pixel;
  values.function = GXcopy;

  XChangeGC(display, gc, GCForeground | GCFunction, &values);
}

/**********************************************************************
* xg_set_color_name: Sets the color of the graphics context
* Usage: xg_set_color_name(context,name)
**********************************************************************/
void 
xg_set_color_name(Xg_context *context, char *name)
{
  Display        *display;
  int             screen;
  Colormap        cmap;
  XColor          color, exact;
  GC              gc;
  XGCValues       values;

  if (not_X(context)) 
    {
      printf("Must have rgb values to set idraw color\n");
      return;
    }

  display = XtDisplay(context->canvas);
  screen = DefaultScreen(display);
  cmap = DefaultColormap(display, screen);
  gc = context->gc;

  if (!XAllocNamedColor(display, cmap, name, &color, &exact)) {
    printf("Warning:  Couldn't allocate requested color\n");
    color.pixel = BlackPixel(display, screen);
  }
  values.foreground = color.pixel;
  values.function = GXcopy;

  context->fr=(double)color.red/65535;
  context->fg=(double)color.green/65535;
  context->fb=(double)color.blue/65535;

  XChangeGC(display, gc, GCForeground | GCFunction, &values);
}
/**********************************************************************
* xg_set_color_name_xor: Sets the drawing mode to exclusive-or
* Usage: xg_set_color_name_xor(context,name)
**********************************************************************/
void 
xg_set_color_name_xor(Xg_context *context, char *name)
{
  Widget          canvas;
  Display        *display;
  int             screen;
  Colormap        cmap;
  XColor          color, exact;
  GC              gc;
  int             back;
  XGCValues       values;

  if (not_X(context)) return;

  canvas = context->canvas;
  display = XtDisplay(canvas);
  screen = DefaultScreen(display);
  cmap = DefaultColormap(display, screen);
  gc = context->gc;

  if (!XAllocNamedColor(display, cmap, name, &color, &exact)) {
    printf("Warning:  Couldn't allocate requested color\n");
    color.pixel = BlackPixel(display, screen);
  }
  XtVaGetValues(canvas, XmNbackground, &back, NULL);

  /* So when given color xor'ed with background, will appear as color */
  values.foreground = color.pixel ^ back;
  values.function = GXxor;

  context->fr=(double)color.red/65535;
  context->fg=(double)color.green/65535;
  context->fb=(double)color.blue/65535;

  XChangeGC(display, gc, GCForeground | GCFunction, &values);
}

/**********************************************************************
* xg_color_by_rgb: Sets the color of the new graphics context
* Usage: xg_color_by_rgb(context,r,g,b)
**********************************************************************/
Xg_color *
xg_color_rgb(Xg_context *context, int r, int g, int b)
{
  Widget          canvas;
  Display        *display;
  int             screen;
  Colormap        cmap;
  XColor          color;
  GC              gc;
  int             back;
  XGCValues       values;
  Xg_color        *new_color;


  new_color = (Xg_color *) malloc(sizeof(Xg_color));

  new_color->r=r/255;
  new_color->g=g/255;
  new_color->b=b/255;

  if (not_X(context)) return (new_color);

  display = XtDisplay(context->canvas);
  screen = DefaultScreen(display);
  cmap = DefaultColormap(display, screen);
  gc = context->gc;

  color.red = (unsigned short) new_color->r *65535;
  color.green = (unsigned short) new_color->g *65535;
  color.blue = (unsigned short) new_color->b *65535;

  if (!XAllocColor(display, cmap, &color))
    {
      printf("Warning:  Couldn't allocate requested color\n");
      color.pixel = BlackPixel(display, screen);
    }
  XtVaGetValues(context->canvas, XmNbackground, &back, NULL);

  values.foreground = color.pixel;

  new_color->gc =
    XCreateGC(display, XtWindow(context->canvas), GCForeground, &values);

  new_color->r=(double)color.red/65535;
  new_color->g=(double)color.green/65535;
  new_color->b=(double)color.blue/65535;

  return (new_color);
}

/**********************************************************************
 * xg_color_name: Returns gc with given color in default mode.
 * Usage: Xg_color color;
 *        color = xg_color_name(context,name)
 **********************************************************************/
Xg_color *
xg_color_name(Xg_context *context, char *name)
{
  Widget          canvas;
  Display        *display;
  int             screen;
  Colormap        cmap;
  XColor          color, exact;
  GC              gc;
  int             back;
  XGCValues       values;
  Xg_color        *new_color;

  if (not_X(context))
    {
      printf("Cannot allocate idraw color by name--must use rgb values\n");
      return NULL;
    }
      
  canvas = context->canvas;
  display = XtDisplay(canvas);
  screen = DefaultScreen(display);
  cmap = DefaultColormap(display, screen);
  gc = context->gc;

  if (name == NULL)
    XtVaGetValues(canvas, XmNforeground, &(color.pixel), NULL);
  else if (!XAllocNamedColor(display, cmap, name, &color, &exact)) {
    printf("Warning:  Couldn't allocate requested color\n");
    color.pixel = BlackPixel(display, screen);
  }
  XtVaGetValues(canvas, XmNbackground, &back, NULL);

  values.foreground = color.pixel;

  new_color = (Xg_color *) malloc(sizeof(Xg_color));

  new_color->gc =
    XCreateGC(display, XtWindow(canvas), GCForeground, &values);

  new_color->r=(double)exact.red/65535;
  new_color->g=(double)exact.green/65535;
  new_color->b=(double)exact.blue/65535;

  return (new_color);
}
/**********************************************************************
 * xg_color_name_xor: Returns gc with given color in xor mode.
 * Usage: Xg_color *color;
 *        color = xg_color_name_xor(context,name)
 *        color = xg_color_name_xor(context,NULL);   <- foreground color
 **********************************************************************/
Xg_color *
xg_color_name_xor(Xg_context *context, char *name)
{
  Widget          canvas;
  Display        *display;
  int             screen;
  Colormap        cmap;
  XColor          color, exact;
  GC              gc;
  int             back;
  XGCValues       values;
  Xg_color        *new_color;

  if (not_X(context)) return NULL;

  canvas = context->canvas;
  display = XtDisplay(canvas);
  screen = DefaultScreen(display);
  cmap = DefaultColormap(display, screen);
  gc = context->gc;

  if (name == NULL)
    XtVaGetValues(canvas, XmNforeground, &(color.pixel), NULL);
  else if (!XAllocNamedColor(display, cmap, name, &color, &exact)) {
    printf("Warning:  Couldn't allocate requested color\n");
    XtVaGetValues(canvas, XmNforeground, &(color.pixel), NULL);
  }

  XtVaGetValues(canvas, XmNbackground, &back, NULL);

  /* So when given color xor'ed with background, will appear as color */
  values.foreground = color.pixel ^ back;
  values.function = GXxor;

  new_color = (Xg_color *) malloc(sizeof(Xg_color));

  new_color->gc =
    XCreateGC(display, XtWindow(canvas), GCForeground | GCFunction, &values);

  new_color->r=(double)color.red/65535;
  new_color->g=(double)color.green/65535;
  new_color->b=(double)color.blue/65535;

  return (new_color);
}
/**********************************************************************
 * xg_clear: Clears the drawing area.
 * Usage: xg_clear(context);
 **********************************************************************/
void 
xg_clear(Xg_context *context)
{
  (*(context->clear))(context);
  context->reinitialized = FALSE;
}

Xg_font *
xg_font_name(Xg_context *context, char *fontname)
{
  Xg_font        *fontinfo;
  XFontStruct     *font;
  XGCValues       values;

  if (not_X(context))
    {
      fontinfo= (Xg_font *) malloc(sizeof(Xg_font));
      fontinfo->name=(char *) malloc(strlen(fontname)+1);
      strcpy(fontinfo->name,fontname);
      return(fontinfo);
    }

  font=XLoadQueryFont(XtDisplay(context->canvas),fontname);

  if (font==NULL)
      return NULL;
  else
    {
      values.font=font->fid;
      fontinfo= (Xg_font *) malloc(sizeof(Xg_font));
      
      fontinfo->gc = XCreateGC(XtDisplay(context->canvas),
                         XtWindow(context->canvas),
                         GCFont, &values);
      fontinfo->fontlist=XmFontListCreate(font,"ISO8859-1");
      fontinfo->fontlist=XmFontListAdd(fontinfo->fontlist,font,"ISO8859-1");
      return(fontinfo);
    }
}


/**********************************************************************
 * xg_flush: Makes all graphics visible.
 * Usage: xg_flush(context);
 **********************************************************************/
void 
xg_flush(Xg_context *context)
{
  if (not_X(context)) return;

  XFlush(XtDisplay(context->canvas));
}

/**********************************************************************
 * xgd_draw_text: Draws text in device coordinates.
 * Usage:  xgd_draw_text(context,dx,dy,txt,fontinfo);
 **********************************************************************/
void 
xgd_draw_text(Xg_context *context, int dx, int dy, char *txt,
	      Xg_font *fontinfo)
{
  (*(context->draw_text))(context,dx,dy,txt,fontinfo);
  context->reinitialized = FALSE;
}

/**********************************************************************
 * xgd_draw_text: Draws text in device coordinates.
 * Usage:  xg_draw_text(context,x,y,txt,fontinfo);
 **********************************************************************/
void
xg_draw_text(Xg_context *context, double x, double y, char *txt,
	     Xg_font *fontinfo) 
{
  int dx,dy;

  xg_dev_point(context, x, y, &dx, &dy);
  xgd_draw_text(context,dx,dy,txt,fontinfo);
}

/**********************************************************************
 * xg_draw_point: Draws point in window coordinates
 * Usage: xg_draw_point(context,x,y,color)
 **********************************************************************/
void 
xg_draw_point(Xg_context *context, double x, double y, Xg_color *color)
{
  int dx, dy;

  xg_dev_point(context, x, y, &dx, &dy);
  xgd_draw_point(context, dx, dy, color);
}
/**********************************************************************
 * xgd_draw_point: Draws a point in device coordinates.
 * Usage:  xgd_draw_point(context,dx,dy,color);
 **********************************************************************/
void 
xgd_draw_point(Xg_context *context, int dx, int dy, Xg_color *color)
{
  (*(context->draw_point))(context,dx,dy,color);
  context->reinitialized = FALSE;
}
/**********************************************************************
 * xg_draw_line: Draws line in window coordinates
 * Usage: xg_draw_line(context,x1,y1,x2,y2,color)
 **********************************************************************/
void 
xg_draw_line(Xg_context *context, double x1, double y1, double x2, double y2,
	     Xg_color *color)
{
  int             dx1, dy1, dx2, dy2;

  xg_dev_point(context, x1, y1, &dx1, &dy1);
  xg_dev_point(context, x2, y2, &dx2, &dy2);
  xgd_draw_line(context, dx1, dy1, dx2, dy2, color);
}
/**********************************************************************
 * xgd_draw_line: Draws line in device coordinates.
 * Usage:  xgd_draw_line(context,dx1,dy1,dx2,dy2,color);
 **********************************************************************/
void 
xgd_draw_line(Xg_context *context, int dx1, int dy1, int dx2, int dy2,
	      Xg_color *color)
{
  (*(context->draw_line))(context,dx1,dy1,dx2,dy2,color);
  context->reinitialized = FALSE;
}

/**********************************************************************
 * xg_draw_rectangle: Draws rectangle in window coordinates
 * Usage: xg_draw_rectangle(context,x1,y1,x2,y2,color)
 **********************************************************************/
void 
xg_draw_rectangle(Xg_context *context, double x1, double y1, double x2, double y2,
	     Xg_color *color)
{
  int             dx1, dy1, dx2, dy2;

  xg_dev_point(context, x1, y1, &dx1, &dy1);
  xg_dev_point(context, x2, y2, &dx2, &dy2);
/*  printf("xg_draw_rectangle %f %f %f %f to %d %d %d %d\n",x1,y1,x2,y2,dx1,dy1,dx2,dy2);*/
  xgd_draw_rectangle(context, dx1, dy1, dx2, dy2, color);
}
/**********************************************************************
 * xgd_draw_rectangle: Draws rectangle in device coordinates.
 * Usage:  xgd_draw_rectangle(context,dx1,dy1,dx2,dy2,color);
 **********************************************************************/
void 
xgd_draw_rectangle(Xg_context *context, int dx1, int dy1, int dx2, int dy2,
	      Xg_color *color)
{
/***
  XPoint  devPoints[4];

  devPoints[0].x = dx1;
  devPoints[0].y = dy1;
  devPoints[1].x = dx2;
  devPoints[1].y = dy1;
  devPoints[2].x = dx2;
  devPoints[2].y = dy2;
  devPoints[3].x = dx1;
  devPoints[3].y = dy2;

  xgd_fill_polygon(context,devPoints,4,color);
***/

  (*(context->draw_rectangle))(context,dx1,dy1,dx2,dy2,color);
  context->reinitialized = FALSE;
}

/**********************************************************************
 * xg_fill_rectangle: Draws filled rectangle in window coordinates
 * Usage: xg_fill_rectangle(context,x1,y1,x2,y2,color)
 **********************************************************************/
void 
xg_fill_rectangle(Xg_context *context, double x1, double y1, double x2, double y2,
		  Xg_color *color)
{
  int             dx1, dy1, dx2, dy2;

  xg_dev_point(context, x1, y1, &dx1, &dy1);
  xg_dev_point(context, x2, y2, &dx2, &dy2);
  xgd_fill_rectangle(context, dx1, dy1, dx2, dy2, color);
}
/**********************************************************************
 * xgd_fill_rectangle: Draws filled rectangle in device coordinates.
 * Usage:  xgd_fill_rectangle(context,dx1,dy1,dx2,dy2,color);
 **********************************************************************/
void 
xgd_fill_rectangle(Xg_context *context, int dx1, int dy1, int dx2, int dy2,
	      Xg_color *color)
{
/***
  XPoint  devPoints[4];

  devPoints[0].x = dx1;
  devPoints[0].y = dy1;
  devPoints[1].x = dx2;
  devPoints[1].y = dy1;
  devPoints[2].x = dx2;
  devPoints[2].y = dy2;
  devPoints[3].x = dx1;
  devPoints[3].y = dy2;

  xgd_fill_polygon(context,devPoints,4,color);
***/

  (*(context->fill_rectangle))(context,dx1,dy1,dx2,dy2,color);
  context->reinitialized = FALSE;
}
/**********************************************************************
* xg_draw_polygon: Draws a polygon in window coordinates
* Usage: xg_draw_polygon(context,points,num_points,color)
**********************************************************************/
void 
xg_draw_polygon(Xg_context *context, Xg_point *points, int n, Xg_color *color)
{
  XPoint         *dev_points;
  int             i, dx, dy;

  dev_points = (XPoint *) malloc(sizeof(XPoint) * n);

  for (i = 0; i < n; i++) {
    xg_dev_point(context, (points+i)->x, (points+i)->y,
		 &dx, &dy);
    /*
     * Done this way because xg_dev_point expects int addresses as last two
     * arguments.
     */
    dev_points[i].x = dx;
    dev_points[i].y = dy;
  }

  xgd_draw_polygon(context, dev_points, n, color);

  free((char *) dev_points);
}
/**********************************************************************
 * xgd_draw_polygon: Draws a polygon in device coordinates.
 * Usage:  xgd_draw_polygon(context,dev_points,n)
 **********************************************************************/
void 
xgd_draw_polygon(Xg_context *context, XPoint *dev_points, int n, Xg_color *color)
{
  (*(context->draw_polygon))(context,dev_points,n,color);
  context->reinitialized = FALSE;
}

/**********************************************************************
* xg_fill_polygon: Draws a filled polygon in window coordinates
* Usage: xg_fill_polygon(context,points,num_points,color)
**********************************************************************/
void 
xg_fill_polygon(Xg_context *context, Xg_point *points, int n, Xg_color *color)
{
  XPoint         *dev_points;
  int             i, dx, dy;

  dev_points = (XPoint *) malloc(sizeof(XPoint) * n);

  for (i = 0; i < n; i++) {
    xg_dev_point(context, (points+i)->x, (points+i)->y,
		 &dx, &dy);
    /*
     * Done this way because xg_dev_point expects int addresses as last two
     * arguments.
     */
    dev_points[i].x = dx;
    dev_points[i].y = dy;
  }

  xgd_fill_polygon(context, dev_points, n, color);

  free((char *) dev_points);
}
/**********************************************************************
 * xgd_fill_polygon: Draws a filled polygon in device coordinates.
 * Usage:  xgd_fill_polygon(context,dev_points,n)
 **********************************************************************/
void 
xgd_fill_polygon(Xg_context *context, XPoint *dev_points, int n, Xg_color *color)
{
  (*(context->fill_polygon))(context,dev_points,n,color);
  context->reinitialized = FALSE;
}

/**********************************************************************
 * xgd_draw_circle: Draws outline of in circle in device coordinates
 * Usage: xgd_draw_circle(context,dx,dy,dradius,color)
**********************************************************************/
void 
xgd_draw_corner(Xg_context *context, int dx, int dy, int dradius, 
		int angle, Xg_color *color)
{
  (*(context->draw_corner))(context,dx,dy,dradius,angle,color);
  context->reinitialized = FALSE;
}


/**********************************************************************
 * xg_draw_circle: Draws outline of circle in window coordinates.
 * Usage: xg_draw_circle(context,x,y,radius,color)
 **********************************************************************/
void 
xg_draw_corner(Xg_context *context, double x, double y, double radius, 
	       int angle, Xg_color *color)
{
  int             dx, dy, dradius, d1, d2;

  xg_dev_point(context, x, y, &dx, &dy);
  d1 = xg_dev_x_diff(context, radius);
  d2 = xg_dev_y_diff(context, radius);
  dradius = d1 < d2 ? d1 : d2;

  xgd_draw_corner(context, dx, dy, dradius, angle, color);
}

/**********************************************************************
 * xg_draw_circle: Draws outline of circle in window coordinates.
 * Usage: xg_draw_circle(context,x,y,radius,color)
 **********************************************************************/
void 
xg_draw_circle(Xg_context *context, double x, double y, double radius, 
	       Xg_color *color)
{
  int             dx, dy, dradius, d1, d2;

  xg_dev_point(context, x, y, &dx, &dy);
  d1 = xg_dev_x_diff(context, radius);
  d2 = xg_dev_y_diff(context, radius);
  dradius = d1 < d2 ? d1 : d2;

  xgd_draw_circle(context, dx, dy, dradius, color);
}
/**********************************************************************
 * xgd_draw_circle: Draws outline of in circle in device coordinates
 * Usage: xgd_draw_circle(context,dx,dy,dradius,color)
**********************************************************************/
void 
xgd_draw_circle(Xg_context *context, int dx, int dy, int dradius, 
		Xg_color *color)
{
  (*(context->draw_circle))(context,dx,dy,dradius,color);
  context->reinitialized = FALSE;
}
/**********************************************************************
 * xg_fill_circle: Draws filled circle in window coordinates.
 * Usage: xg_fill_circle(context,x,y,radius,color)
 **********************************************************************/
void 
xg_fill_circle(Xg_context *context, double x, double y, double radius, 
	       Xg_color *color)
{
  int             dx, dy, dradius, d1, d2;

  xg_dev_point(context, x, y, &dx, &dy);
  d1 = xg_dev_x_diff(context, radius);
  d2 = xg_dev_y_diff(context, radius);
  dradius = d1 < d2 ? d1 : d2;

  xgd_fill_circle(context, dx, dy, dradius, color);
}
/**********************************************************************
 * xgd_fill_circle: Draws filled in circle in device coordinates
 * Usage: xgd_fill_circle(context,dx,dy,dradius,color)
**********************************************************************/
void 
xgd_fill_circle(Xg_context *context, int dx, int dy, int dradius, 
		Xg_color *color)
{
  (*(context->fill_circle))(context,dx,dy,dradius,color);
  context->reinitialized = FALSE;
}
/**********************************************************************
* xg_ploty: Plots data in window coordinates
* Usage: xg_ploty(context,yvalues,num_points,num_curves,color)
**********************************************************************/
void 
xg_ploty(Xg_context *context, double  *points, int n, int ncurves, Xg_color *color)
{
  XPoint         *dev_points;
  int             i, dx, dy, curve, colori;

  dev_points = (XPoint *) malloc(sizeof(XPoint) * n);

  for (curve=0; curve < ncurves; curve++) {

    for (i = 0; i < n; i++) {
      //      printf("(%f %f)-> ",(double)i+1,points[curve*n+i]);
      xg_dev_point(context, (double)i+1, points[curve*n+i], &dx, &dy);
      /*
       * Done this way because xg_dev_point expects int addresses as last two
       * arguments.
       */
      dev_points[i].x = dx;
      dev_points[i].y = dy;
      //      printf("(%d %d)\n",dx,dy);
    }

    colori = curve%XG_COLORLIST_SIZE;
    xgd_draw_polygon(context, dev_points, n, context->colorlist[colori]);
  }

  free((char *) dev_points);
}






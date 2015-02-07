/**********************************************************************
 *   idraw.c:  Device routines for producing files readable by idraw.
 **********************************************************************/

#include "xg.h"

/***prototypes***/
void idraw_init(Xg_context *context, XtPointer device_info,
		int *argc, char *argv[], char *name);
void idraw_close(Xg_context *context);
void idraw_get_max_width_height(Xg_context *context, Dimension *width, 
			    Dimension *height) ;
void idraw_install_viewport(Xg_context *context);
void idraw_clear(Xg_context *context);
void idraw_draw_point(Xg_context *context, int dx, int dy, Xg_color *color);
void idraw_draw_line(Xg_context *context, int dx1, int dy1, int dx2, int dy2,
		     Xg_color *color);
void idraw_draw_polygon(Xg_context *context, XPoint *dev_points, int n, 
		    Xg_color *color);
void idraw_fill_polygon(Xg_context *context, XPoint *dev_points, int n, 
		    Xg_color *color);
void idraw_draw_corner(Xg_context *context, int dx, int dy, int dradius, 
		       int angle, Xg_color *color);
void idraw_draw_rectangle(Xg_context *context, int dx1, int dy1, 
			  int dx2, int dy2, Xg_color *color);
void idraw_fill_rectangle(Xg_context *context, int dx1, int dy1, 
			  int dx2, int dy2, Xg_color *color);
void idraw_draw_circle(Xg_context *context, int dx, int dy, int dradius, 
		       Xg_color *color);
void idraw_fill_circle(Xg_context *context, int dx, int dy, int dradius, 
		       Xg_color *color);
void idraw_draw_text(Xg_context *context, int dx, int dy, char *txt, Xg_font *fontinfo);
void idraw_prolog(FILE *file, char *s,int fill, Xg_color *color);

/**********************************************************************
 *
 **********************************************************************/
void xg_load_idraw_functions(Xg_context *context)
{
  context->init = &idraw_init;
  context->close = &idraw_close;
  context->get_max_width_height = &idraw_get_max_width_height;
  context->install_viewport = &idraw_install_viewport;
  context->clear = &idraw_clear;
  context->draw_point = &idraw_draw_point;
  context->draw_line = &idraw_draw_line;
  context->fill_polygon = &idraw_fill_polygon;
  context->draw_polygon = &idraw_draw_polygon;
  context->draw_corner = &idraw_draw_corner;
  context->draw_rectangle = &idraw_draw_rectangle;
  context->fill_rectangle = &idraw_fill_rectangle;
  context->draw_circle = &idraw_draw_circle;
  context->fill_circle = &idraw_fill_circle;
  context->draw_text = &idraw_draw_text;
}  

/**********************************************************************
 *
 **********************************************************************/
void idraw_init(Xg_context *context, XtPointer device_info,
		int *argc, char *argv[], char *name)
{
  FILE *file;
  if (device_info!=NULL) 
  {
     context->xgout=((Xg_context *)device_info)->xgout;
     context->fonts = ((Xg_context*)device_info)->fonts;
     return;
   }

  if (strcmp((char *)name,"stdout") != 0) {
    if ((file = fopen((char *)name, "w")) == NULL) {
      printf("Unable to open %s.", (char *)name);
      return;
    }
  } else
    file = stdout;
  
  context->fonts = 0;
  context->xgout = file;

  fprintf(file,"%%I Idraw 7\n");
  fprintf(file,"\nBegin\n");
  fprintf(file,"%%I b 65535\n1 0 0 [] 0 SetB\n");

/*  fprintf(file,"%%I cbg White\n");
  fprintf(file,"%%I cfg Black\n");*/

/*  fprintf(file,"%%I cbg c%.2f%.2f%.2f\n%f %f %f SetCBg\n",
	  1.,1.,1.,1.,1.,1.);*/
/*  fprintf(file,"1 1 1 SetCBg\n"); /* added Dec, 94 */
  fprintf(file,"%%I cfg u\n");
  fprintf(file,"%%I cbg u\n");
  fprintf(file,"%%I f u\n");
  fprintf(file,"%%I p u\n");
  fprintf(file,"%%I t u\n");
}

/**********************************************************************
 *
 **********************************************************************/
void idraw_close(Xg_context *context)
{
  FILE *file=context->xgout;

  fprintf(file,"\nEnd %%I eop\n\nshowpage\n\n%%%%Trailer\n\nend");
  /*  fclose(context->xgout); */  /* other contexts may not be done with file*/
}

/**********************************************************************
 *
 **********************************************************************/
void idraw_get_max_width_height(Xg_context *context, Dimension *width, 
			    Dimension *height) 
{
  *width = 612.0;
  *height = 792.0;
}

/**********************************************************************
 *
 **********************************************************************/
void idraw_install_viewport(Xg_context *context)
{
  printf("idraw_install_viewport does nothing. \n");
    /* maybe should do something about bounding box??*/
  return;
}

/**********************************************************************
 *
 **********************************************************************/
void idraw_clear(Xg_context *context)
{
  FILE *file = context->xgout;

  context->page++;
  fprintf(file,"%%%%Page: %d %d\n",context->page,context->page);

}

/**********************************************************************
 *
 **********************************************************************/
void idraw_draw_point(Xg_context *context, int dx, int dy, Xg_color *color)
{
  idraw_draw_line(context,dx,dy,dx,dy,color);
}

/**********************************************************************
 *
 **********************************************************************/
void idraw_draw_line(Xg_context *context, int dx1, int dy1, int dx2, int dy2,
		     Xg_color *color)
{
  FILE *file = context->xgout;

  idraw_prolog(file,"Line",0, color);
  fprintf(file,"%%I\n%d %d %d %d Line\nEnd\n",dx1,dy1,dx2,dy2);
}

/**********************************************************************
 *
 **********************************************************************/
void idraw_fill_polygon(Xg_context *context, XPoint *dev_points, int n, 
		    Xg_color *color)
{
  FILE *file = context->xgout;
  int i;

  idraw_prolog(file,"Poly",1, color);
  fprintf(file,"%%I %d\n",n);
  for (i=0; i<n; i++)
    fprintf(file,"%d %d\n",dev_points[i].x,dev_points[i].y);
  fprintf(file,"%d Poly\nEnd\n",n);
}

/**********************************************************************
 *
 **********************************************************************/
void idraw_draw_polygon(Xg_context *context, XPoint *dev_points, int n, 
		    Xg_color *color)
{
  FILE *file = context->xgout;
  int i;

  idraw_prolog(file,"Poly",0, color);
  fprintf(file,"%%I %d\n",n);
  for (i=0; i<n; i++)
    fprintf(file,"%d %d\n",dev_points[i].x,dev_points[i].y);
  fprintf(file,"%d Poly\nEnd\n",n);
}

/**********************************************************************
 *
 **********************************************************************/
void idraw_draw_corner(Xg_context *context, int dx, int dy, int dradius, 
		       int angle, Xg_color *color)
{
  int startx, starty, endx, endy, midx, midy;
  double radang=(double)(3.1415927 * angle/180.0);
  FILE *file = context->xgout;

  startx=dx+(int)(cos(radang)* (double) dradius);
  starty=dy+(int)(sin(radang)* (double) dradius);
  midx=dx+(int)(cos(radang+0.78539816)* (double) dradius);
  midy=dy+(int)(sin(radang+0.78539816)* (double) dradius);
  endx=dx+(int)(cos(radang+1.5707963)* (double) dradius);
  endy=dy+(int)(sin(radang+1.5707963)* (double) dradius);

  idraw_prolog(file,"BSpl",0, color);
  fprintf(file,"%%I 3\n%d %d\n%d %d\n%d %d\n3 BSpl\nEnd\n",startx, starty,
	  midx, midy, endx, endy);
}

/**********************************************************************
 *
 **********************************************************************/
void idraw_draw_rectangle(Xg_context *context, int dx1, int dy1, 
			  int dx2, int dy2, Xg_color *color)
{
  FILE *file = context->xgout;

  idraw_prolog(file,"Rect",0, color);
  fprintf(file,"%%I\n%d %d %d %d Rect\nEnd\n",dx1,dy1,dx2,dy2);
}

/**********************************************************************
 *
 **********************************************************************/
void idraw_fill_rectangle(Xg_context *context, int dx1, int dy1, 
			  int dx2, int dy2, Xg_color *color)
{
  FILE *file = context->xgout;

  idraw_prolog(file,"Rect",1, color);
  fprintf(file,"%%I\n%d %d %d %d Rect\nEnd\n",dx1,dy1,dx2,dy2);
}

/**********************************************************************
 *
 **********************************************************************/
void idraw_draw_circle(Xg_context *context, int dx, int dy, int dradius, 
		       Xg_color *color)
{
  FILE *file = context->xgout;

  idraw_prolog(file,"Elli",0, color);
  fprintf(file,"%%I\n%d %d %d %d Elli\nEnd\n",dx,dy,dradius,dradius);
}

/**********************************************************************
 *
 **********************************************************************/
void idraw_fill_circle(Xg_context *context, int dx, int dy, int dradius, 
		       Xg_color *color)
{
  FILE *file = context->xgout;

  idraw_prolog(file,"Elli",1, color);
  fprintf(file,"%%I\n%d %d %d %d Elli\nEnd\n",dx,dy,dradius,dradius);
}

void idraw_draw_text(Xg_context *context, int dx, int dy, char *txt, Xg_font *fontinfo)
{
  FILE *file = context->xgout;

  fprintf(file,"\nBegin %%I Text\n");
  fprintf(file,"%%I cfg\n%f %f %f SetCFg\n",
	context->fr, context->fg, context->fb);
  /* use color someday */
  fprintf(file,"%%I f %s\n/%s %d Setf\n",
	  fontinfo->name, context->name,context->fonts);
  fprintf(file,"%%I t\n[ 1 0 0 1 %d %d ] concat\n",dx,dy);
  fprintf(file,"%%I\n[\n(%s)\n] Text\nEnd\n",txt);
  context->fonts+=1;
}

/**********************************************************************
 *
 **********************************************************************/
void idraw_prolog(FILE *file, char *s, int fill, Xg_color *color)
{
  fprintf(file,"\nBegin %%I %s\n",s);
  fprintf(file,"%%I b u\n");

  /* Until I figure color in idraw out, I'll just set fg and bg color */
  /* on each object. And leave them undefined (u) in preamble. */

  fprintf(file,"%%I cfg Black\n");
  fprintf(file,"%%I cbg White\n");
  
/*
   if (color!=NULL)
    fprintf(file,"%%I cfg c%.2f%.2f%.2f\n%f %f %f SetCFg\n",
	    color->r, color->g, color->b,color->r, color->g, color->b);
  else
    fprintf(file,"%%I cfg\n0 0 0 SetCFg\n");
*/

/*  fprintf(file,"%%I cbg u\n");*/
/*  fprintf(file,"%%I cbg c%.2f%.2f%.2f\n%f %f %f SetCBg\n",
	  1.,1.,1.,1.,1.,1.);*/

  if (fill)
    fprintf(file,"%%I p \n0 Setp\n");
  else
    fprintf(file,"%%I p\n1 SetP\n");
  fprintf(file,"%%I t u\n");
}


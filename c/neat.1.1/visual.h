#ifndef VISUAL_H
#define VISUAL_H

#include <list>
#include <vector>
#include <algorithm>

#include "networks.h" 
#include "genetics.h"


#include <gtk--/main.h>
#include <gtk--/style.h>
#include <gtk--/window.h>
#include <gtk--/button.h>
#include <gtk--/box.h>
#include <gtk--/drawingarea.h>

extern Gtk::Main *myapp;

char *itoa(int n,char s[]);

class Population;
class Network;

class ScribbleDrawingArea  : public Gtk::DrawingArea
{
  /* Backing pixmap for drawing area */

  Gdk_Pixmap pixmap;
  Gdk_GC gc;
  Gdk_GC brush_gc;
  Gdk_Window win;
  Gdk_Visual visual;

  virtual gint configure_event_impl (GdkEventConfigure *event);
  virtual gint expose_event_impl (GdkEventExpose *event);
  virtual gint button_press_event_impl (GdkEventButton *event);
  virtual gint motion_notify_event_impl (GdkEventMotion *event);
  void draw_brush (gdouble x, gdouble y);

public:
  Network *net;
  list<NNode*> *nodes;
  ScribbleDrawingArea();

  void drawrows_old(NNode *curnode,list<NNode*> &seenlist);
  void drawrows(list<NNode*> *drawlist);
};


class ScribbleWindow : public Gtk::Window
{

  Gtk::VBox vbox;
  ScribbleDrawingArea drawing_area;
  Gtk::Button button;

  void quit ();
public:  
  Network *net;
  ScribbleWindow (Network *n,list<NNode*> *drawlist,int w,int h);  //make a window of width and height pixels

}; 


//These classes are used for Population Visualization
//They specify windows and drawing areas where the visualization
//will be drawn

class PopulationDrawingArea  : public Gtk::DrawingArea
{
  /* Backing pixmap for drawing area */

  Gdk_Pixmap pixmap;
  Gdk_GC gc;
  Gdk_GC brush_gc;
  Gdk_Window win;
  Gdk_Visual visual;

  virtual gint configure_event_impl (GdkEventConfigure *event);
  virtual gint expose_event_impl (GdkEventExpose *event);
  virtual gint button_press_event_impl (GdkEventButton *event);
  virtual gint motion_notify_event_impl (GdkEventMotion *event);
  void draw_brush (gdouble x, gdouble y);

public:
  int start_gen; //First generation to draw
  int stop_gen; //Last generation to draw
  Population *pop;
  PopulationDrawingArea();

};


class PopulationWindow : public Gtk::Window
{

  Gtk::VBox vbox;
  PopulationDrawingArea drawing_area;
  Gtk::Button button;
  void quit ();
public:  
  Network *net;
  PopulationWindow (Population *p,int w,int h,int start_gen,int stop_gen);  //make a window of width and height pixels

}; 

#endif



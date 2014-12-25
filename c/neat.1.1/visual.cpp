#include "visual.h"

Gtk::Main *myapp;

/* ******************************************************* */
/*                 NETWORK VISUALIZATION                   */
/* ******************************************************* */

ScribbleDrawingArea::ScribbleDrawingArea()
    : Gtk::DrawingArea(), pixmap (0)
  {

    set_events (GDK_EXPOSURE_MASK
		| GDK_LEAVE_NOTIFY_MASK
		| GDK_BUTTON_PRESS_MASK
		| GDK_POINTER_MOTION_MASK
		| GDK_POINTER_MOTION_HINT_MASK);
  }


void reverse(char *s) { 
  int c,i,j;

  for(i=0, j=strlen(s)-1;i<j;i++,j--) {
    c=s[i];
    s[i]=s[j];
    s[j]=c;
  }

}

/* itoa: convert n to characters in s */
char *itoa(int n,char s[]) {
  int i,sign;

  if ((sign=n)<0)
    n=-n;

  i=0;

  do {
    s[i++]=n%10+'0';
  } while ((n/=10)>0);
  if (sign<0)
    s[i++]='-';
  s[i]='\0';
  reverse(s);

  return s;
}


//This is an old way of drawing a network by recursively drawing as
//program descends through the networks.  However, this means
//visiting each node potentially mutliple times, which is pointless
//when we already have a drawlist which was constructed when the command
//to draw was issued and row levels were computed.
void ScribbleDrawingArea::drawrows_old(NNode *curnode,list<NNode*> &seenlist) {
  list<Link*> innodes=curnode->incoming;
  list<Link*>::iterator curlink;
  list<NNode*>::iterator location;
  char text[20];

  Gdk_Font font ( "-*-courier-bold-*-*-*-*-240-*-*-*-*-*-*");
  Gdk_Color blue("blue");
  get_colormap().alloc(blue);
  Gdk_Color black("black");
  get_colormap().alloc(black);
  Gdk_Color red("red");
  get_colormap().alloc(red);

  if (!((curnode->type)==SENSOR))
    {
    for(curlink=innodes.begin();curlink!=innodes.end();++curlink) {
      location=find(seenlist.begin(),seenlist.end(),((*curlink)->in_node));
      //if (location==seenlist.end()) {
	seenlist.push_back((*curlink)->in_node);

	cout<<"Connecting "<<curnode<<" TO "<<((*curlink)->in_node)<<endl;
	

	//Draw the node as a rectangle!
	//gc=get_style()->get_black_gc();
	
	//Only draw each node once, if it hasn't been seen
	if (location==seenlist.end()) {
	  gc.set_foreground(red);
	  pixmap.draw_rectangle (gc,
				 TRUE,
				 ((*curlink)->in_node)->xpos-15, 
				 ((*curlink)->in_node)->ypos-15,
				 30, 30);
	}
	
	if ((*curlink)->weight<0)
	  gc.set_foreground(blue);
	else gc.set_foreground(black);
	gc.set_line_width(floor(fabs(2*((*curlink)->weight))));
	pixmap.draw_line(gc,
			 curnode->xpos,
			 curnode->ypos,
			 ((*curlink)->in_node)->xpos, 
			 ((*curlink)->in_node)->ypos);
	
	//cout<<"Drawing text: "<<((*curlink)->in_node)->node_id<<endl;

	//Label the node
	gc.set_foreground(black);
	itoa(((*curlink)->in_node)->node_id,text);
	//cout<<"len: "<<strlen(text)<<endl;
	pixmap.draw_text      (font,
			       gc,
			       ((*curlink)->in_node)->xpos+15,
			       ((*curlink)->in_node)->ypos+5,
			       text,
			       strlen(text));
			 

	drawrows_old((*curlink)->in_node,seenlist);
	//
    }

    }

}

void ScribbleDrawingArea::drawrows(list<NNode*> *drawlist) {
  list<Link*>::iterator curlink;
  list<NNode*>::iterator curnode;
  char text[20];

  int arrowadd;  //For positioning arrowtips

  Gdk_Font font ( "-*-courier-bold-*-*-*-*-240-*-*-*-*-*-*");
  Gdk_Color blue("blue");
  get_colormap().alloc(blue);
  Gdk_Color black("black");
  get_colormap().alloc(black);
  Gdk_Color red("red");
  get_colormap().alloc(red);

  //Draw all links for all nodes first so that nodes will appear on top
  for(curnode=drawlist->begin();curnode!=drawlist->end();++curnode) {
    
    //Draw links for this node
    for(curlink=((*curnode)->incoming).begin();curlink!=((*curnode)->incoming).end();++curlink) {
      if ((*curlink)->weight<0)
	gc.set_foreground(blue);
      else gc.set_foreground(black);
      gc.set_line_width(floor(fabs(2*((*curlink)->weight))));
      
      if (!((*curlink)->is_recurrent)) {
	pixmap.draw_line(gc,
			 (*curnode)->xpos,
			 (*curnode)->ypos,
			 ((*curlink)->in_node)->xpos, 
			 ((*curlink)->in_node)->ypos);

	//If the link is between two nodes on the same row,
	//draw an arrowtip to make it clear which way it goes
	if (((*curnode)->row)==(((*curlink)->in_node)->row)) {

	  if (((*curnode)->xpos)<(((*curlink)->in_node)->xpos))
	    arrowadd=15;
	  else arrowadd=-15;

	  pixmap.draw_line(gc,
			   (*curnode)->xpos+arrowadd,
			   (*curnode)->ypos,
			   (*curnode)->xpos+arrowadd+arrowadd, 
			   (*curnode)->ypos+15);
	   pixmap.draw_line(gc,
			   (*curnode)->xpos+arrowadd,
			   (*curnode)->ypos,
			   (*curnode)->xpos+arrowadd+arrowadd, 
			   (*curnode)->ypos-15);


	}

      }

      //The way to draw a recur
      if ((*curlink)->is_recurrent) {
	gc.set_line_width(2);
	pixmap.draw_line(gc,
			 ((*curlink)->in_node)->xpos,
			 ((*curlink)->in_node)->ypos,
			 ((*curlink)->in_node)->xpos-20,
			 ((*curlink)->in_node)->ypos-20);
	pixmap.draw_line(gc,
			 ((*curlink)->in_node)->xpos-20,
			 ((*curlink)->in_node)->ypos-20,
			 ((*curlink)->in_node)->xpos-25,
			 ((*curlink)->in_node)->ypos-20);
	pixmap.draw_line(gc,
			 ((*curlink)->in_node)->xpos-25,
			 ((*curlink)->in_node)->ypos-20,
			 ((*curlink)->in_node)->xpos-25,
			 (*curnode)->ypos+20);
	pixmap.draw_line(gc,
			 ((*curlink)->in_node)->xpos-25,
			 (*curnode)->ypos+20,
			 (*curnode)->xpos,
			 (*curnode)->ypos+20);
	pixmap.draw_line(gc,
			 (*curnode)->xpos,
			 (*curnode)->ypos+20,
			 (*curnode)->xpos,
			 (*curnode)->ypos);
      }
								      


    }
    
  }

  //Now draw all the nodes and labels
  for(curnode=drawlist->begin();curnode!=drawlist->end();++curnode) {
    
    //Draw the node
    gc.set_foreground(red);
    pixmap.draw_rectangle (gc,
			   TRUE,
			   (*curnode)->xpos-15, 
			   (*curnode)->ypos-15,
			   30, 30);


    //Label the node
    gc.set_foreground(black);
    itoa((*curnode)->node_id,text);
    //cout<<"len: "<<strlen(text)<<endl;
    pixmap.draw_text      (font,
			   gc,
			   (*curnode)->xpos+15,
			   (*curnode)->ypos+5,
			   text,
			   strlen(text));
  }
  

}


/* Create a new backing pixmap of the appropriate size */
int ScribbleDrawingArea::configure_event_impl (GdkEventConfigure * /* event */)
  {

    Gdk_Font font ( "-*-courier-bold-*-*-*-*-240-*-*-*-*-*-*");
    list<NNode*>::iterator curnode;
    list<NNode*> seenlist;
    Gdk_Color red("red");
    Gdk_Color white("white");
    Gdk_Color black("black");
    get_colormap().alloc(black);
    get_colormap().alloc(white);

    char text[20];

    win = get_window();
    visual = win.get_visual();

    if (pixmap)
      pixmap.release();
    
    pixmap.create(get_window(),  width(), height());

    if (!gc)
      {
        gc = Gdk_GC(pixmap);
	gc.set_foreground(white);
      }
    //gc = get_style()->get_white_gc();
    gc.set_foreground(white);
    pixmap.draw_rectangle (gc,
			TRUE,
			0, 0,
			width(),
			height());


    //Draw network off of the drawlist
    drawrows(nodes);

    return TRUE;
  }

/* Redraw the screen from the backing pixmap */
int ScribbleDrawingArea::expose_event_impl (GdkEventExpose *event)
  {
    
    gc = get_style()->get_fg_gc(get_state());
    win.draw_pixmap(gc ,
		    pixmap,
		    event->area.x, event->area.y,
		    event->area.x, event->area.y,
		    event->area.width, event->area.height);

    return FALSE;
  }

/* Draw a rectangle on the screen */
void ScribbleDrawingArea::draw_brush (gdouble x, gdouble y)
  {
    GdkRectangle update_rect;
    update_rect.x = (int)x - 5;
    update_rect.y = (int)y - 5;
    update_rect.width = 5;
    update_rect.height = 5;

    // we can't create the brush gc in the ctor because there was not window
    // so we will initialize it here once, and use it from then on.
    if (!brush_gc)
      {
        brush_gc = Gdk_GC(pixmap);
        Gdk_Color red("red");
        get_colormap().alloc(red);
        brush_gc.set_foreground(red);
      }
    pixmap.draw_rectangle(
			brush_gc,
			TRUE,
			update_rect.x, update_rect.y,
			update_rect.width, update_rect.height);
    draw(&update_rect);
  }

gint ScribbleDrawingArea::button_press_event_impl (GdkEventButton *event)
  {
    if (event->button == 1 && pixmap)
      draw_brush (event->x, event->y);

    return TRUE;
  }

gint ScribbleDrawingArea::motion_notify_event_impl (GdkEventMotion *event)
  {
    int x, y;
    GdkModifierType state;
    Gdk_Window window(event->window);
    if (event->is_hint)
      window.get_pointer (x, y, state);
    else
      {
	x = (int)event->x;
	y = (int)event->y;
	state = (GdkModifierType) event->state;
      }
    
    if (state & GDK_BUTTON1_MASK && pixmap)
      draw_brush (x, y);
  
    return TRUE;
  }

void ScribbleWindow::quit ()
  {
    destroy_();
    //    hide();
    Gtk::Main::quit();
  }

ScribbleWindow::ScribbleWindow (Network *n,list<NNode*> *drawlist,int w,int h)
    :  Gtk::Window(GTK_WINDOW_TOPLEVEL),
       vbox (FALSE, 0),
       button ("quit")
  {

    net=n;
    drawing_area.net=n;
    drawing_area.nodes=drawlist;

    add (vbox);

    /* Create the drawing area */
    drawing_area.size (w, h);
    vbox.pack_start (drawing_area, TRUE, TRUE, 0);


    /* Add the button */
    vbox.pack_start (button, FALSE, FALSE, 0);

    button.clicked.connect(slot(*this, &ScribbleWindow::quit));
    destroy.connect(slot(*this, &ScribbleWindow::quit));
    
    drawing_area.show();
    button.show();
    vbox.show();
  }



/* ******************************************************* */
/*                 SPECIES VISUALIZATION                   */
/* ******************************************************* */


PopulationDrawingArea::PopulationDrawingArea()
    : Gtk::DrawingArea(), pixmap (0)
  {

    set_events (GDK_EXPOSURE_MASK
		| GDK_LEAVE_NOTIFY_MASK
		| GDK_BUTTON_PRESS_MASK
		| GDK_POINTER_MOTION_MASK
		| GDK_POINTER_MOTION_HINT_MASK);
  }

/* Create a new backing pixmap of the appropriate size */
int PopulationDrawingArea::configure_event_impl (GdkEventConfigure * /* event */)
  {

    Gdk_Font font ( "-*-courier-bold-*-*-*-*-120-*-*-*-*-*-*");
    list<Generation_viz*>::iterator curgen;
    Generation_viz *lastgen=0;
    list<Species_viz*>::iterator curspec;
    list<Species_viz*>::iterator curspec2;
    Species_viz *samespec=0;
    Gdk_Color red("red");
    Gdk_Color white("white");
    Gdk_Color black("black");
    Gdk_Color blue("blue");
    Gdk_Color cyan("cyan");
    Gdk_Color yellow("yellow");
    Gdk_Color green("green");
    Gdk_Color orange("orange");
    Gdk_Color purple("purple");
    GdkPoint points[4];  //The polygonal area to fill

    get_colormap().alloc(black);
    get_colormap().alloc(white);
    get_colormap().alloc(red);
    get_colormap().alloc(blue);
    get_colormap().alloc(cyan);
    get_colormap().alloc(yellow);
    get_colormap().alloc(green);
    get_colormap().alloc(orange);
    get_colormap().alloc(purple);
    int ypos=3;
    int xpos=3;
    double ypixels=height()-6;
    double xpixels=width()-6;
    int ystep;
    int xstep;
    int num_gens=stop_gen-start_gen+1;  //Total number of generations
    int gen_count;
    int print_number_every=(int) num_gens/10;
    bool y_for_gens;  //Flag for deciding on which axis is which


    //Markers for points where the color should change with fitness
    //Mark 3 is the MEAN
    //Mark 1 and 2 are 2 and 1 std. away respectively, going down
    //Mark 4 and 5 are 1 and 2 std. away respectively, going up
    double mark1,mark2,mark3,mark4,mark5;

    if (num_gens<10) print_number_every=1;
    
    if (width()>height())
      y_for_gens=false;
    else y_for_gens=true;

    mark3=pop->mean_fitness;
    mark2=mark3-(pop->standard_deviation);
    mark1=mark2-(pop->standard_deviation);
    mark4=mark3+(pop->standard_deviation);
    mark5=mark4+(pop->standard_deviation);

    //cout<<"blue "<<mark1<<" cyan "<<mark2<<" green "<<mark3<<" orange "<<mark4<<" purple "<<mark5<<" red"<<endl;

    cout<<"blue "<<mark3<<" black "<<mark5<<" red"<<endl;

    //Computer the step
    if (y_for_gens)
      ystep=(int) floor(ypixels/((double) num_gens));
    else xstep=(int) floor(xpixels/((double) num_gens));

    char text[20];

    win = get_window();
    visual = win.get_visual();

    if (pixmap)
      pixmap.release();
    
    pixmap.create(get_window(),  width(), height());

    if (!gc)
      {
        gc = Gdk_GC(pixmap);
	gc.set_foreground(white);
      }
    //gc = get_style()->get_white_gc();
    gc.set_foreground(white);
    pixmap.draw_rectangle (gc,
			TRUE,
			0, 0,
			width(),
			height());

    //Set the current generation to the start gen
    curgen=(pop->generation_snapshots).begin();
    for(gen_count=1;gen_count<start_gen;gen_count++)
      ++curgen;

    //DRAW THE POPULATION HERE
    for(gen_count=start_gen;gen_count<=stop_gen;gen_count++) {

      //Print the gen_num in the left or bottom column space every few gens
      if ((gen_count%print_number_every)==0) {
	gc.set_foreground(black);
	itoa(gen_count,text);
	if (y_for_gens) 
	  pixmap.draw_text      (font,
				 gc,
				 1,
				 ypos+5,
				 text,
				 strlen(text));
	else pixmap.draw_text      (font,
				    gc,
				    xpos-5,
				    height()-10,
				    text,
				    strlen(text));
      }

      //Draw each Species in the current generation
      for(curspec=((*curgen)->species_list).begin();curspec!=((*curgen)->species_list).end();++curspec) {
	
	//Determine the line color for this species
	if (((*curspec)->max_fitness)<mark1) {
	    gc.set_foreground(blue);
	    //gc.set_line_width(1);
	}
	else if (((*curspec)->max_fitness)<mark2) {
	    gc.set_foreground(blue);
	    //gc.set_line_width(1);
	}
	else if (((*curspec)->max_fitness)<mark3) {
	    gc.set_foreground(blue);
	    //gc.set_line_width(2);
	}
	else if (((*curspec)->max_fitness)<mark4) {
	    gc.set_foreground(black);
	    //gc.set_line_width(2);
	}
	else if (((*curspec)->max_fitness)<mark5) {
	    gc.set_foreground(black);
	    //gc.set_line_width(3);
	}
	else {
	    gc.set_foreground(red);
	    //gc.set_line_width(3);
	}

	//If this is the first gen, then everything is a straight line
	if (gen_count==start_gen) {
	  //gc.set_foreground(black);
	  //gc.set_line_width(floor(fabs(2*((*curlink)->weight))));
	  if (y_for_gens)
	    pixmap.draw_line(gc,
			     (*curspec)->startx,
			     ypos,
			     (*curspec)->endx, 
			     ypos);
	  else pixmap.draw_line(gc,
				xpos,
				(*curspec)->starty,
				xpos, 
				(*curspec)->endy);
	}
	else {
	  //Check the previous generation for this same Species
	  curspec2=(lastgen->species_list).begin();
	  while ((curspec2!=(lastgen->species_list).end())&&
		 (((*curspec2)->id)!=((*curspec)->id)))
	    ++curspec2;

	  //If it is not found, just draw a horizontal line
	  if (curspec2==(lastgen->species_list).end()) {
	    if (y_for_gens)
	      pixmap.draw_line(gc,
			       (*curspec)->startx,
			       ypos,
			       (*curspec)->endx, 
			       ypos);
	    else pixmap.draw_line(gc,
				  xpos,
				  (*curspec)->starty,
				  xpos, 
				  (*curspec)->endy);

	  }

	  //Otherwise, if it existed before, connect the 2 endpoint pairs
	  else {
 
	    //Instead of just linking lines, draw a filled polygon
	    //contained by them
	    if (y_for_gens) {
	      points[0].x=(*curspec)->startx;
	      points[0].y=ypos;
	      points[1].x=(*curspec)->endx;
	      points[1].y=ypos;
	      points[2].x=(*curspec2)->endx;
	      points[2].y=ypos-ystep;
	      points[3].x=(*curspec2)->startx;
	      points[3].y=ypos-ystep;
	    }
	    else {
	      points[0].x=xpos;
	      points[0].y=(*curspec)->starty;
	      points[1].x=xpos;
	      points[1].y=(*curspec)->endy;;
	      points[2].x=xpos-xstep;
	      points[2].y=(*curspec2)->endy;
	      points[3].x=xpos-xstep;
	      points[3].y=(*curspec2)->starty;
	    }

	    pixmap.draw_polygon    (gc,
				    TRUE,
				    points,
				    4);

    
	    //Make white borders
	    gc.set_foreground(white);
	    gc.set_line_width(1);
	    if (y_for_gens) {
	      pixmap.draw_line(gc,
			       (*curspec)->startx,
			       ypos,
			       (*curspec2)->startx, 
			       ypos-ystep);
	      pixmap.draw_line(gc,
			       (*curspec)->endx,
			       ypos,
			       (*curspec2)->endx, 
			       ypos-ystep);
	    }
	    else {
	      pixmap.draw_line(gc,
			       xpos,
			       (*curspec)->starty,
			       xpos-xstep, 
			       (*curspec2)->starty);
	      pixmap.draw_line(gc,
			       xpos,
			       (*curspec)->endy,
			       xpos-xstep, 
			       (*curspec2)->endy);
	    }


	  }

	  //Print the Species' id for high fitness
	  if ((*curspec)->max_fitness>mark5) {
	    gc.set_foreground(black);
	    itoa((*curspec)->id,text);
	    if (y_for_gens)
	      pixmap.draw_text      (font,
				     gc,
				     (int) ((((*curspec)->endx)+((*curspec)->startx))/2.0),
				     ypos,
				     text,
				     strlen(text));
	    else pixmap.draw_text      (font,
					gc,
					xpos-5,
					(int) ((((*curspec)->endy)+((*curspec)->starty))/2.0)+5.0,
					text,
					strlen(text));
	  }
	    
	  

	}

      }  //End for on curspec

      //Increment pos
      ypos+=ystep;
      xpos+=xstep;
      
      //Remember the last generation
      lastgen=(*curgen);

      ++curgen;

    } //End for on curgen


    //Print labels in a separate loop so they don't get painted over    
    
    //Set the current generation to the start gen
    curgen=(pop->generation_snapshots).begin();
    for(gen_count=1;gen_count<start_gen;gen_count++)
      ++curgen;
    ypos=3;
    xpos=3;
    for(gen_count=start_gen;gen_count<=stop_gen;gen_count++) {
      
      //Draw each Species in the current generation
      for(curspec=((*curgen)->species_list).begin();curspec!=((*curgen)->species_list).end();++curspec) {
	
	if ((*curspec)->max_fitness>mark5) {
	  gc.set_foreground(black);
	  itoa((*curspec)->id,text);
	  if (y_for_gens)
	    pixmap.draw_text      (font,
				   gc,
				   (int) ((((*curspec)->endx)+((*curspec)->startx))/2.0),
				   ypos,
				   text,
				   strlen(text));
	  else pixmap.draw_text      (font,
				      gc,
				      xpos-7,
				      (int) ((((*curspec)->endy)+((*curspec)->starty))/2.0)+5.0,
				      text,
				      strlen(text));
	}
	
      }
      
      //Increment pos
      ypos+=ystep;
      xpos+=xstep;
      
      //Remember the last generation
      lastgen=(*curgen);
      
      ++curgen;
      
    }
    
    return TRUE;
    
  }


/* Redraw the screen from the backing pixmap */
int PopulationDrawingArea::expose_event_impl (GdkEventExpose *event)
  {
    
    gc = get_style()->get_fg_gc(get_state());
    win.draw_pixmap(gc ,
		    pixmap,
		    event->area.x, event->area.y,
		    event->area.x, event->area.y,
		    event->area.width, event->area.height);

    return FALSE;
  }

/* Draw a rectangle on the screen */
void PopulationDrawingArea::draw_brush (gdouble x, gdouble y)
  {
    GdkRectangle update_rect;
    update_rect.x = (int)x - 5;
    update_rect.y = (int)y - 5;
    update_rect.width = 5;
    update_rect.height = 5;

    // we can't create the brush gc in the ctor because there was not window
    // so we will initialize it here once, and use it from then on.
    if (!brush_gc)
      {
        brush_gc = Gdk_GC(pixmap);
        Gdk_Color red("red");
        get_colormap().alloc(red);
        brush_gc.set_foreground(red);
      }
    pixmap.draw_rectangle(
			brush_gc,
			TRUE,
			update_rect.x, update_rect.y,
			update_rect.width, update_rect.height);
    draw(&update_rect);
  }

gint PopulationDrawingArea::button_press_event_impl (GdkEventButton *event)
  {
    if (event->button == 1 && pixmap)
      draw_brush (event->x, event->y);

    return TRUE;
  }

gint PopulationDrawingArea::motion_notify_event_impl (GdkEventMotion *event)
  {
    int x, y;
    GdkModifierType state;
    Gdk_Window window(event->window);
    if (event->is_hint)
      window.get_pointer (x, y, state);
    else
      {
	x = (int)event->x;
	y = (int)event->y;
	state = (GdkModifierType) event->state;
      }
    
    if (state & GDK_BUTTON1_MASK && pixmap)
      draw_brush (x, y);
  
    return TRUE;
  }

void PopulationWindow::quit ()
  {
    destroy_();
    //    hide();
    Gtk::Main::quit();
  }

PopulationWindow::PopulationWindow (Population *p,int w,int h,int start_gen,int stop_gen)
    :  Gtk::Window(GTK_WINDOW_TOPLEVEL),
       vbox (FALSE, 0),
       button ("quit")
  {

    drawing_area.pop=p; 
    drawing_area.start_gen=start_gen;
    drawing_area.stop_gen=stop_gen;

    add (vbox);

    /* Create the drawing area */
    drawing_area.size (w, h);
    vbox.pack_start (drawing_area, TRUE, TRUE, 0);


    /* Add the button */
    vbox.pack_start (button, FALSE, FALSE, 0);

    button.clicked.connect(slot(*this, &PopulationWindow::quit));
    destroy.connect(slot(*this, &PopulationWindow::quit));
    
    drawing_area.show();
    button.show();
    vbox.show();
  }







CC = g++ -o3

LIBS = -L/usr/lib /usr/lib/libqthreads.so.0 -lguile -ldl -lreadline -ltermcap -lm

INCLUDES = -I/usr/include/g++-2 -I/usr/lib/gtkmm/include -I/usr/lib/sigc++/include -I/usr/lib/glib/include -I/usr/include/gtk-1.2 -I/usr/include/glib-1.2

CFLAGS = -g -Wall -Wno-return-type $(INCLUDES) -DSWIG_GLOBAL

neat: networks.o genetics.o visual.o experiments.o neatmain.o neatswig_wrap.o
	$(CC) $(CFLAGS) $(LIBS) networks.o genetics.o visual.o experiments.o neatswig_wrap.o neatmain.o -o neat `gtkmm-config --cflags --libs`

networks.o: networks.cpp networks.h visual.h 
	$(CC) $(CFLAGS) -c networks.cpp -o networks.o  

genetics.o: genetics.cpp networks.h genetics.h visual.h visual.o
	$(CC) $(CFLAGS) -c genetics.cpp -o genetics.o

visual.o: visual.cpp visual.h networks.h
	$(CC) $(CFLAGS) -c visual.cpp -o visual.o

experiments.o: experiments.cpp experiments.h networks.h
	$(CC) $(CFLAGS) -c experiments.cpp -o experiments.o

neatswig_wrap.o: neatswig.i neatswig_wrap.cpp networks.h
	$(CC) $(CFLAGS) -c neatswig_wrap.cpp -o neatswig_wrap.o

#neatswig_wrap.cpp: neatswig.i
#	swig -guile -c++ neatswig.i
#	cp neatswig_wrap.c neatswig_wrap.cpp

neatmain.o: neatmain.cpp neatmain.h neat.h networks.h
	$(CC) $(CFLAGS) -c neatmain.cpp -o neatmain.o





















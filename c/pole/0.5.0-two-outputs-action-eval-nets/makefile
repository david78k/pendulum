LIBS =  -lm -Wno-overflow -Wno-format -Wno-conversion -Wno-format-security
CC = gcc
#XLIBS =  -L/usr/lib -L/usr/X11R6/lib -L/usr/X11R6/LessTif/Motif1.2/lib -lm -lXm -lXt -lX11 -Wno-overflow
XLIBS =  -lm -Wno-overflow
INC = -I/usr/X11R6/include -I/usr/X11R6/LessTif/Motif1.2/include

default:
	$(CC) $(CFLAGS) -c demo.c $(LIBS) 
	$(CC) $(CFLAGS) -o demo demo.o $(LIBS) 

demo.o:	demo.c 
	$(CC) $(CFLAGS) -c demo.c $(LIBS) 

demo:	demo.o 
	$(CC) $(CFLAGS) -o demo demo.o $(LIBS) 

default: demo

demo-net.o:	demo-net.c 
	$(CC) $(CFLAGS) -c demo-net.c $(XLIBS)


demo-net:	demo-net.o xg.a
	$(CC) $(CFLAGS) -o demo-net demo-net.o $(LIBS) $(XLIBS)

testxg:	testxg.o xg.a
	$(CC) $(CFLAGS) $(INC) -o testxg testxg.o xg.a $(XLIBS)

testxg.o:	testxg.c
	$(CC) $(CFLAGS) $(INC) -c testxg.c

idraw.o: idraw.c  xg.h
	$(CC) -c $(CFLAGS) $(INC) idraw.c

X.o: X.c xg.h
	$(CC) -c $(CFLAGS) $(INC) X.c 

xg.o:	xg.c  xg.h
	$(CC) -g -c $(CFLAGS) $(INC) xg.c

xg.a:	xg.o idraw.o X.o
	ar rv xg.a xg.o idraw.o X.o
#	ranlib xg.a


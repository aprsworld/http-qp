



I=-I/usr/local/include/glib-2.0/ -I/usr/local/lib/glib-2.0/include/
L=-L/usr/local/lib/

http-qp: http-qp.o
	cc -g $L http-qp.o -o http-qp -lglib-2.0
http-qp.o: http-qp.c
	cc -c -g $I http-qp.c

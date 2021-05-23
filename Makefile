CC=gcc
CFLAGS=-Wall -O `pkg-config --cflags glib-2.0 gmodule-2.0`
LDLIBS=`pkg-config --libs glib-2.0 gmodule-2.0`

ngubeac: ngubeac.c

clean:
	rm -f ngubeac *.o

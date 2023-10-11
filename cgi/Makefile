CFLAGS=-g -Wall
CC=gcc
AR=ar
RANLIB=ranlib
LIBS=-L./ -lcgic -lsqlite3

all: libcgic.a cgictest.cgi capture view

install: libcgic.a
	cp libcgic.a /usr/local/lib
	cp cgic.h /usr/local/include
	@echo libcgic.a is in /usr/local/lib. cgic.h is in /usr/local/include.

libcgic.a: cgic.o cgic.h
	rm -f libcgic.a
	$(AR) rc libcgic.a cgic.o
	$(RANLIB) libcgic.a

#mingw32 and cygwin users: replace .cgi with .exe

cgictest.cgi: cgictest.o libcgic.a
	gcc cgictest.o -o cgictest.cgi ${LIBS}

capture: capture.o libcgic.a
	gcc capture.o -o capture ${LIBS}

view: view.o libcgic.a
	gcc view.o -o view.cgi ${LIBS}

clean:
	rm -f *.o *.a cgictest.cgi capture cgicunittest view.cgi

test:
	gcc -D UNIT_TEST=1 cgic.c -o cgicunittest
	./cgicunittest

CFLAGS = -O
LDFLAGS = -L/usr/X11R6/lib -lX11
CC = cc

all: yasiti blackhole mori1 mori2

yasiti: yasiti.o
	$(CC) $@.o -o $@ $(LDFLAGS)
yasiti.o: yasiti.c u1.xbm u2.xbm u3.xbm u4.xbm
blackhole: blackhole.o
	$(CC) $@.o -o $@ $(LDFLAGS)
blackhole.o: blackhole.c
mori1: mori1.o
	$(CC) $@.o -o $@ $(LDFLAGS)
mori1.o: mori1.c

mori2: mori2.o
	$(CC) $@.o -o $@ $(LDFLAGS)
mori2.o: mori2.c

clean:
	-rm -f *.o *~ yasiti blackhole mori1 mori2

CFLAGS += -O
CPPFLAGS += -Wall
LOADLIBES = -lX11

progs = yasiti blackhole mori1 mori2

prefix = /usr/local
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin

all: $(progs)

yasiti.o: yasiti.c u1.xbm u2.xbm u3.xbm u4.xbm

INSTALL = install
INSTALL_PROGRAM = $(INSTALL)

install: all
	mkdir -p $(DESTDIR)$(bindir)
	$(INSTALL_PROGRAM) $(progs) $(DESTDIR)$(bindir)/

uninstall:
	-cd $(DESTDIR)$(bindir) && rm -f $(progs)

clean:
	-rm -f $(progs) *.o

.PHONY: all install uninstall clean

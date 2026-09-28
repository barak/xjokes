#include	<X11/Xlib.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>

#include	"mori.xbm"
#include	"mori2.xbm"

Display *dpy;
Window win;
int screen;

void
usage (char *filename)
{
  fprintf (stderr, "Usage: %s [-display <displayname>]\n", filename);
  exit (1);
}

extern void do_all ();

int
main (int argc, char **argv)
{
  char *display = NULL;
  unsigned long vmask;
  XSetWindowAttributes xswat;

  switch (argc)
    {
    case 1:
      break;
    case 3:
      display = argv[2];
      break;
    default:
      usage (argv[0]);
      exit (1);
    }

  if ((dpy = XOpenDisplay (display)) == NULL)
    {
      fprintf (stderr, "can't open display\n");
      exit (0);
    }

  screen = DefaultScreen (dpy);

  xswat.override_redirect = True;
  xswat.do_not_propagate_mask =
    KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask;
  vmask = CWOverrideRedirect | CWDontPropagate;
  win = XCreateWindow (dpy, RootWindow (dpy, screen), 0, 0,
		       DisplayWidth (dpy, screen), DisplayHeight (dpy,
								  screen), 0,
		       CopyFromParent, CopyFromParent, CopyFromParent, vmask,
		       &xswat);
  XMapWindow (dpy, win);

  XSync (dpy, 0);

  do_all ();

  exit (0);
}

typedef struct
{
  int x, y;
  int d;
} mask_type;
int
comp (i, j)
     mask_type *i, *j;
{
  return i->d - j->d;
}

inline int
isqr (int a)
{
  return a * a;
}

void
do_all ()
{
  int height, width;
  int xn, yn, i;
  GC morigc[2];
  Pixmap mori[2], p[2];
  mask_type *mask, *m;
  int x, y;
  XGCValues gcvals;
  static char *(bit[2]) =
  {
  mori_bits, mori2_bits};
  GC copygc, fillgc;

  gcvals.graphics_exposures = False;

  gcvals.foreground = 1;
  gcvals.background = 0;
  copygc = XCreateGC (dpy, win,
		      GCForeground | GCBackground | GCGraphicsExposures,
		      &gcvals);

  gcvals.foreground = WhitePixel (dpy, screen);
  fillgc = XCreateGC (dpy, win, GCForeground, &gcvals);

  width = DisplayWidth (dpy, screen);
  height = DisplayHeight (dpy, screen);

  xn = width / mori_width;
  yn = height / mori_height;

  m = mask = (mask_type *) malloc (sizeof (mask_type) * xn * yn);
  for (y = 0; y < yn; y++)
    {
      for (x = 0; x < xn; x++)
	{
	  m->d = isqr (x - (xn / 2)) + isqr (y - (yn / 2));
	  m->x = x;
	  m->y = y;
	  m++;
	}
    }
  qsort (mask, xn * yn, sizeof (mask_type), comp);

  gcvals.foreground = BlackPixel (dpy, screen);
  gcvals.background = WhitePixel (dpy, screen);
  gcvals.function = GXcopy;
  gcvals.fill_style = FillStippled;
  for (i = 0; i < 2; i++)
    {
      mori[i] = XCreateBitmapFromData (dpy, win, bit[i],
				       mori_width, mori_height);
      morigc[i] = XCreateGC (dpy, win,
			     GCForeground | GCBackground | GCFillStyle |
			     GCFunction, &gcvals);
      XSetStipple (dpy, morigc[i], mori[i]);
      p[i] = XCreatePixmap (dpy, win,
			    mori_width, mori_height, DefaultDepth (dpy, 0));
      XFillRectangle (dpy, p[i], fillgc, 0, 0, mori_width, mori_height);
      XFillRectangle (dpy, p[i], morigc[i], 0, 0, mori_width, mori_height);
    }
  for (m = mask, i = 0; i < xn * yn; i++, m++)
    {
      XCopyArea (dpy, p[0], win, copygc, 0, 0, mori_width, mori_height,
		 m->x * mori_width, m->y * mori_height);

      XSync (dpy, 0);
      usleep (1000);

    }

  XSync (dpy, 0);
  sleep (1);
  for (m = mask, i = 0; i < xn * yn; i++, m++)
    {
      XCopyArea (dpy, p[1], win, copygc, 0, 0, mori_width, mori_height,
		 m->x * mori_width, m->y * mori_height);
    }
  XSync (dpy, 0);
  sleep (1);
  for (m = mask, i = 0; i < xn * yn; i++, m++)
    {
      XCopyArea (dpy, p[0], win, copygc, 0, 0, mori_width, mori_height,
		 m->x * mori_width, m->y * mori_height);

    }
  XSync (dpy, 0);
  sleep (1);
}

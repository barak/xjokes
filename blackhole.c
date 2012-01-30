#include	<X11/Xlib.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>

#define	N 10

Display *dpy;
Window win;
GC copygc, fillgc;
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
  XGCValues gcvals;


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

  gcvals.graphics_exposures = False;

  gcvals.foreground = 1;
  gcvals.background = 0;
  copygc = XCreateGC (dpy, win,
		      GCForeground | GCBackground | GCGraphicsExposures,
		      &gcvals);

  gcvals.foreground = BlackPixel (dpy, screen);
  fillgc = XCreateGC (dpy, win, GCForeground, &gcvals);

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
comp (const void *ii, const void *jj)
{
  const mask_type *i = ii;
  const mask_type *j = jj;
  return i->d - j->d;
}

mask_type *mask;

inline int
isqr (int a)
{
  return a * a;
}

void
mov (int fw, int tw, int width, int height)
{
  int w, h;
  int wx, hy, fox, foy, tox, toy, fh, th, i;
  mask_type *ma;

  th = height * tw / (width);
  w = tw / N;
  h = th / N;

  fh = height * fw / (width);
  wx = fw / N;
  hy = fh / N;

  fox = (width - fw) / 2;
  foy = (height - fh) / 2;

  tox = (width - tw) / 2;
  toy = (height - th) / 2;

  ma = mask;
  for (i = 0; i < N * N; i++)
    {
      XCopyArea (dpy, win, win, copygc,
		 fox + wx * ma->x, foy + hy * ma->y,
		 w, h, tox + w * ma->x, toy + h * ma->y);
      ma++;
    }
  XFillRectangle (dpy, win, fillgc, fox, foy, fw, toy - foy);
  XFillRectangle (dpy, win, fillgc, fox, foy, tox - fox, fh);
  XFillRectangle (dpy, win, fillgc, fox, foy + hy * N, fw, toy - foy + 5);
  XFillRectangle (dpy, win, fillgc, fox + wx * N, foy, tox - fox + 5, fh + 5);

}

void
do_all ()
{
  int height, width;
  int w;
  int x, y;
  mask_type *ma;
  width = DisplayWidth (dpy, screen);
  height = DisplayHeight (dpy, screen);

  ma = mask = (mask_type *) malloc (sizeof (mask_type) * N * N);
  for (y = 0; y < N; y++)
    {
      for (x = 0; x < N; x++)
	{
	  ma->x = x;
	  ma->y = y;
	  ma->d = isqr ((x + 1) * 2 - N) + isqr ((y + 1) * 2 - N);
	  ma++;
	}
    }
  qsort (mask, N * N, sizeof (mask_type), comp);

  for (w = width; w > 30; w -= 10)
    {
      mov (w, w - 10, width, height);
    }

  XFillRectangle (dpy, win, fillgc, 0, 0, width, height);

  XSync (dpy, 0);
  sleep (1);
  exit (1);
}

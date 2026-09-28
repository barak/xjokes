#include	<X11/Xlib.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>

#include	"mori.xbm"
#include	"mori2.xbm"

inline int
isqr (int a)
{
  return a * a;
}

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

  /* NOTREACHED */
  exit (0);
}

typedef struct
{
  int x, y, m;
  int d;
} mask_type;
int
comp (const void *ii, const void *jj)
{
  const mask_type *i = ii;
  const mask_type *j = jj;
  return i->d - j->d;
}

int
bit (char *s, mask_type ** mask)
{
  int num, x, y;
  mask_type *m;

  m = *mask =
    (mask_type *) malloc (sizeof (mask_type) * mori_width * mori_height);
  num = 0;
  for (y = 0; y < mori_height; y++)
    {
      for (x = 0; x < mori_width / 8; x++)
	{
	  unsigned char c;
	  int i, xx;
	  c = *s++;
	  for (i = 0; i < 8; i++)
	    {
	      xx = x * 8 + i;
	      m->x = xx;
	      m->y = y;
	      m->m = c & 1;
	      c >>= 1;
	      m->d = isqr (xx - mori_width / 2) + isqr (y - mori_height / 2);
	      if (m->d <= isqr (mori_width / 3) * 2)
		{
		  num++;
		  m++;
		}
	    }
	}
    }
  return num;
}

void
do_all ()
{
  int height, width;
  mask_type *mask, *m, *maskt, *mask2;
  int i, sc;
  int num, numt, num2;
  int ox, oy;
  GC bgc[2];
  XGCValues gcvals;

  gcvals.foreground = WhitePixel (dpy, screen);
  bgc[0] = XCreateGC (dpy, win, GCForeground, &gcvals);
  gcvals.foreground = BlackPixel (dpy, screen);
  bgc[1] = XCreateGC (dpy, win, GCForeground, &gcvals);


  width = DisplayWidth (dpy, screen);
  height = DisplayHeight (dpy, screen);

  num = bit (mori_bits, &mask);
  numt = bit (mori2_bits, &maskt);
  m = mask2 = (mask_type *) malloc (sizeof (mask_type) * numt);
  num2 = 0;
  for (i = 0; i < numt; i++)
    {
      if (mask[i].m != maskt[i].m)
	{
	  m->x = maskt[i].x;
	  m->y = maskt[i].y;
	  m->m = maskt[i].m;
	  m++;
	  num2++;
	}
    }

  qsort (mask, num, sizeof (mask_type), comp);

  for (sc = 1;; sc++)
    {
      m = mask;
      ox = (width - sc * mori_width) / 2;
      oy = (height - sc * mori_height) / 2;

      for (i = 0; i < num; i++)
	{
	  XFillRectangle (dpy, win, bgc[m->m],
			  ox + m->x * sc, oy + m->y * sc, sc, sc);
	  m++;
	}
      if (ox < 300)
	break;
    }
  m = mask2;

  XSync (dpy, 0);
  sleep (1);

  for (i = 0; i < num2; i++)
    {
      XFillRectangle (dpy, win, bgc[m->m],
		      ox + m->x * sc, oy + m->y * sc, sc, sc);
      m++;
    }

  XSync (dpy, 0);
  sleep (2);
}

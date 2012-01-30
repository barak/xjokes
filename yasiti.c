#include	<X11/Xlib.h>
#include	<stdio.h>

#include	"u1.xbm"
#include	"u2.xbm"
#include	"u3.xbm"
#include	"u4.xbm"
#define	DIN	11

Display		*dpy;
Window		win;
int		screen;

usage(filename)
char	*filename;
{
    fprintf(stderr, "Usage: %s [-display <displayname>]\n", filename);
    exit(1);
}

main(argc, argv)
     int	argc;
     char	**argv;
{
    char			*display = NULL;
    unsigned long		vmask;
    XSetWindowAttributes	xswat;

    switch( argc ){
      case 1: break;
      case 3: display = argv[2]; break;
      default: usage( argv[0]); exit(1);
    }

    if ((dpy = XOpenDisplay(display)) == NULL){
	fprintf(stderr, "can't open display\n");
	exit(0);
    }

    screen = DefaultScreen(dpy);

    xswat.override_redirect = True;
    xswat.do_not_propagate_mask =
      KeyPressMask | KeyReleaseMask |ButtonPressMask | ButtonReleaseMask;
    vmask = CWOverrideRedirect | CWDontPropagate;
    win = XCreateWindow(dpy, RootWindow(dpy, screen), 0, 0,
			DisplayWidth(dpy, screen), DisplayHeight(dpy, screen),
			0, CopyFromParent, CopyFromParent, CopyFromParent,
			vmask, &xswat);
    XMapWindow(dpy, win);


    XSync(dpy, 0);
    do_all();

    exit(0);
}

do_all()
{
    int		height, width;
    int		dir, w, h, ox, oy;
    int		i, j, ll, xp, yp, xd, yd;
    int		x, y, lx, ly, wi, hi;
    int		lc1, lc2, lcc, ln;
    GC		uzugc[4], copygc, fillgc;
    Pixmap 	uzu[4], circ;
    XGCValues	gcvals;

    static char	*(bit[4]) = {u4_bits, u3_bits, u2_bits, u1_bits};


    width = DisplayWidth(dpy, screen);
    height = DisplayHeight(dpy, screen);

    w = width / DIN;
    h = height / DIN;

    ox = (DIN / 2) * w + (w - u1_width) / 2;
    oy = (DIN / 2) * h + (h - u1_height) /2;

    gcvals.graphics_exposures = False;
    gcvals.foreground = 1;
    gcvals.background = 0;
    copygc = XCreateGC(dpy, win,
		       GCForeground | GCBackground | GCGraphicsExposures,
		       &gcvals);

    gcvals.foreground = BlackPixel(dpy, screen);
    fillgc = XCreateGC(dpy, win, GCForeground, &gcvals);

    gcvals.foreground = BlackPixel(dpy, screen);
    gcvals.background = WhitePixel(dpy, screen);
    gcvals.fill_style = FillStippled;

    for( i = 0; i < 4; i++){
	uzu[i] = XCreateBitmapFromData(dpy, win, bit[i], u1_width, u1_height);
	uzugc[i] = XCreateGC(dpy, win,
			     GCForeground| GCBackground |
			     GCFillStyle| GCGraphicsExposures,
			     &gcvals);
	XSetStipple( dpy, uzugc[i], uzu[i]);
	XSetTSOrigin( dpy, uzugc[i], ox % u1_width , oy % u1_height);
    }

    circ = XCreatePixmap(dpy, win, u1_width, u1_height, DefaultDepth( dpy, 0));
    XCopyArea(dpy, win, circ, copygc, ox, oy, u1_width, u1_height, 0, 0);
    for( i = 1; i < 20; i++){
	XDrawArc( dpy, win, fillgc,
		 ox + ( u1_width - i) / 2, oy + ( u1_height - i )/ 2 ,
		 i, i,
		 0, 360 * 64);
	XSync(dpy, 0);
	usleep(100000);
    }
    for( i = 50; i ; i--){
	XCopyArea(dpy, circ, win, copygc, 0, 0, u1_width, u1_height, ox, oy);
	XFillRectangle(dpy, win, uzugc[ i & 3], ox, oy, u1_width, u1_height);
	XSync( dpy, 0);
	usleep( 100000 );
    }
    XFillRectangle(dpy, win, fillgc, 0, 0, w, h);
    XFillRectangle(dpy, win, fillgc, 0, h * DIN, width, height - h * DIN);
    XFillRectangle(dpy, win, fillgc, w * DIN, 0, width - w * DIN, height);
    lc2 = DIN - 1;
    lc1 = lc2;
    lcc = 3;
    for( ln = DIN * 2 - 1; ln > 1; ){
	lx = ly = DIN / 2;
	ll = 0;
	for( i = dir = 0; i < ln; i++){
	    if ( !(i & 1)){
		ll++;
	    }
	    switch( dir ){
	      case 0:
		x = lx;
		y = ly - ll;
		wi = 1;
		hi = ll;
		xd = xp = x * w;
		yp = y * h;
		yd = ( y + 1) * h;
		break;
	      case 1:
		x = lx + ll;
		y = ly;
		wi = ll;
		hi = 1;
		xd = lx * w;
		xp = (lx + 1) * w;
		yd = yp = y * h;
		break;
	      case 2:
		x = x;
		y = ly + ll;
		wi = 1;
		hi = ll;
		xd = xp = x * w;
		yp = (ly + 1)* h;
		yd = ly  * h;
		break;
	      case 3:
		x = lx - ll;
		y = ly;
		wi = ll;
		hi = 1;
		xd = (x + 1) * w;
		xp = x * w;
		yd = yp = y * h;
		break;
	    }
	    wi *= w;
	    hi *= h;
	    XCopyArea(dpy, win, win, copygc, xp, yp, wi, hi, xd, yd);
	    if( i == 0){
		XFillRectangle(dpy, win, uzugc[ lc1 & 3],
			       ox, oy,
			       u1_width, u1_height);
	    }
	
	    dir = ( dir + 1 ) & 3;	
	    lx = x;
	    ly = y;
	}
	
	if( --lc1 ==  0){
	    lcc--;
	    ln--;
	    if( lcc == 0){
		lc2--;
		lcc = 3;
	    }
	    lc1 = lc2;
	}
	
	
    }
    XSync( dpy, 0);
}


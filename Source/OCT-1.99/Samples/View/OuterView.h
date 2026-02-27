/*   OUTERVIEW.H   Views that are extravagant windows       April 90, bjw */


#include "view.h"


@interface OuterView : View
{
	struct Window	*view_wndw; 	/* Lower Layer Abstraction Handle */
	struct NewWindow	*wndw_spec;
}

/* ---------------------------------------------------------------------- */


/*  STORAGE:  */


/*  ADJUSTMENT:  */
- intoScreen: (struct Screen *) scrn ;
- setViewSpec: (register struct NewWindow *) new_window ;
- setTitleSTR: (STR) stringTitle ;
- setSize: (register RECT *) prect ;

- doHide ;      	/* Hide window from all screens */
- doReveal ;    	/* Restore window to its last screen */

- drawBorder ;		/* Draw controls around self's content */

@end


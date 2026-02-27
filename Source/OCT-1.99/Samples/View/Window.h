/*   WINDOW.H   Views that are extravagant windows       April 90, bjw */


#include "OuterView.h"


@interface Window : OuterView
{
	RECT 	normalSize;
	id  	miniIcon;
}

/* ---------------------------------------------------------------------- */


/*  STORAGE:  */
+ new ;


/*  ADJUSTMENT:  */
- setViewSpec: (register struct NewWindow *) new_window ;
- setSize: (register RECT *) prect ;

- doClose ;     	/* Shutdown and free */
- doHide ;      	/* Hide window from all screens */
- doReveal ;    	/* Restore window to its last screen */
- doSizeMini ;  	/* Shrink to minumum size */
- doSizeNormal ;	/* Size to a nominal size */
- doSizeMax ;   	/* Expand size to screen max */

@end


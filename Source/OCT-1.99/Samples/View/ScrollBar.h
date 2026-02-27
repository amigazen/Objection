/*   ScrollBar.H    Just that Scroll Bar Fixture            April 90, bjw */

/*
//		This class provides just the scrollbar fixture.  You interpret
//		the reactions of this object.
//
//		03-april-90	bjw 	Initial typing (and still my taxes are undone).
*/


#include "view.h"

@interface ScrollBar : View
{
	id  	less, more;
	short 	sbar_flags;
}

#define  SB_LEFT_RIGHT	((int)1)
#define  SB_UP_DOWN 	((int)2)


+ newAligned: (int) direction ;
- setSize: (RECT *) prect ;
- drawBorder ;
- drawContent ;


@end


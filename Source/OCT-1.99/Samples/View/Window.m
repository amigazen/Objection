/*   WINDOW.M   Talk with underlyng Windowing System     April 90, bjw */

/*
//		The class Window is a full featured outer window.  It includes
//		the ability to minimize itself, which is a lot of work.
//
//		12-april-90	bjw 	Maximize full view.
*/

#include "Window.h"
#include "Pick.h"


@implementation Window : OuterView ;
/*
	RECT 	normalSize;
	id  	miniIcon;
*/


/* ------------------------------------------------------------------- */


/**************/
/*  STORAGE:  */

+ new
{
	id  	guy = [super new];

	guy->normalSize.or.x = 60;
	guy->normalSize.or.y = 22;
	guy->normalSize.ex.x = 520;
	guy->normalSize.ex.y = 160;
	guy->miniIcon = [Pick newID: ID_miniApp];

	return( guy );
}	/* + new */



/*****************/
/*  ADJUSTMENT:  */

- setViewSpec: (register struct NewWindow *) new_window
{

	[super setViewSpec: new_window];
	normalSize = self->hitArea;   			/* Structure Assign */
	return( self );
}	/* - setViewSpec */



- doSizeMini
{

	printf( "mini" );
	/*  If set to normal size, store the size: */
	if( !(self->view_flags & (VF_Maximized | VF_Minimized)) )
		[self getSize:&normalSize ];

	[super doSizeMini];
	/*  [self setSize: [miniIcon getSize]];
		[miniIcon drawAt:0 :0];
	*/
	return self ;
}	/* - doMini */


- doSizeNormal
{
	return [self setSize: normalSize];
}	/* - doSizeNormal */


- doSizeMax
{
	static RECT 	maxsize = { 0, 0, 640, 200 } ;

	/*  If set to normal size, store the size: */
	if( !(self->view_flags & (VF_Maximized | VF_Minimized)) )
		[self getSize:&normalSize ];

	[super doSizeMax];
	[self setSize: &maxsize];
	return self;
}	/* - doSizeMax */


@end


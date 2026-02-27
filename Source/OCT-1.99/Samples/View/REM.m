/*   REM.M    Object: REM  Resource-n-Element Manager       March 90, bjw */

/*
//
//		20-mar-90	bjw 	Initial coding, again.
//		25-apr-90	bjw 	Uses IdList instead of OrderedCollection.
*/

#include "rem.h"
#include <objc/ByteArray.h>
#include <objc/List.h>


@implementation REM : Object


/* --------------------------------------------------------------------- */

short 		from_sdb = FALSE;


/*  Private string-ing area:  */
static char  	buffer[ 256 ];

/*  Examine last char in string (assume strlen > 0): */
#define  LASTCHR(str)	((str)[strlen(str)-1])


/* ------------------------------------------------------------------- */


/*  STORAGE:  */
+ new
{
	id  	nude = [super new];

	nude->dirList = [List new];
	[nude addDir: "" ];
	return( nude );
}	/* + new */


- free
{
	[dirList freeContents];
	[dirList free];
	return [super free];
}	/* - free */


/*  DATASET:  */
-  addDir:(STR)directory 
{
	char	last;

	strncpy( buffer, directory, sizeof(buffer)-4 );
	if( strlen(buffer) > 0 &&
	   (last = LASTCHR(buffer)) != ':' && last != '/' )
	{
		strcat( buffer, "/" );
	}

	[dirList addLast:[ByteArray str:buffer]];
	return( self );
}	/* - addDir: */


/****************/
/*  SEARCHING:  */

- findName:(STR)categoryName rid:(int)rid 
	//	Searches REM dataset for matching identification.
	//	Returns the new object, ready for use. Otherwise, nil.
{
	id   	dirs;
	id  	dname;
	id  	newobj = nil;

	printf( "(findName:rid:) for <%s.%d>\n", categoryName, rid );

	[dirList toFirst];		/* Begin a sequence */
	while( (dirs = [dirList next]) != nil )
	{
		sprintf( buffer, "%sREM-file", [dirs str] );
		printf( "trying <%s>\n", buffer );
	}

}	/* - findName:id: */


@end


/* ---------------------  Public Support Routines  --------------------- */

/*   sdb_pause  --  If running from a debugger, don't pause.  Programmer
//				is probably single stepping, anyway.
*/
	void
sdb_pause()
{
	if( ! from_sdb )
	{
		fputs( "Hit return to cont [ ]\010\010", stdout );
		fflush( stdout );  getchar();
	}
}	/* sdb_pause */


/* ----------------------  Rectangluar Routines  ----------------------- */


/*   prect  --  Print a rectangle, includes title.
*/
	void
prect( title, pr )
	CONST char	*title;
	CONST register RECT   *pr;
{

	printf( "Rect: %s = (%d,%d)-(%d,%d)\n", title,
				pr->or.x,            pr->or.y,
				pr->or.x + pr->ex.x, pr->or.y + pr->ex.y );
}	/* prect */


/*   pointInRect  --  Test if point is within the rectangle.
*/
	BOOL
pointInRect( point, boundRect )
	CONST VECT	point;
	register CONST RECT	*boundRect;
{
	register short 	xpos = point.x;  	/* Check this X */
	register short 	ypos = point.y;  	/* Check this Y */
	BOOL 	inside = NO;

	if( boundRect->or.x >= xpos && boundRect->or.y >= ypos &&
		boundRect->or.x + boundRect->ex.x < xpos &&
		boundRect->or.y + boundRect->ex.y < ypos )
		inside = YES;
	return( inside );
}	/* pointInRect */



/*   REM.H    Object: REM  Resource-n-Element Manager       March 90, bjw */

/*
//
//		20-mar-90	bjw 	Initial coding, again.
*/

#ifndef REM_H
# define REM_H	1

/*  # include "REM:rem.h"  */
# include <objc/Object.h>
# include "gui.h"


@interface REM : Object
{
	id  	dirList;		/* OrderedCollection of dir's to search */
}


/* -------------------------------------------------------------------- */

/*  STORAGE:  */
+ new ;
- free ;

/*  DATASET:  */
-  addDir:(STR)directory ;

/*  SEARCHING:  */
- findName:(STR)categoryName rid:(int)rid ;
	//	Searches REM dataset for matching identification.
	//	Returns the nw object, ready for use. Otherwise, nil.

@end

#endif


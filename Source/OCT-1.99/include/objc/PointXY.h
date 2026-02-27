/*  class @interface :  PointXY                                    */

/*		Class name changed due to conflict with Amiga graphics library.
 *
 *    19-sept-93   bjw    Changed and added protected from #import.
 */

#ifndef _OBJC_POINTXY_H
# define _OBJC_POINTXY_H   1

#include <objc/Object.h>

@interface PointXY : Object
{
	id  	xvalue, yvalue;
}

/* -------------------  Public Methods  ------------------- */

+ x: newx ;
+ y: newy ;
+ x: newx y: newy ;
- freeContents ;
- freeObjects ;
- transpose ;
- x ;
- x: newx ;
- y ;
- y: newy ;

@end

#endif


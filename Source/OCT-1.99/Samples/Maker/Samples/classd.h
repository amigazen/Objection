/*  class @interface :  DClass                                   */

#ifndef _MAKER_DCLASS_H
#define _MAKER_DCLASS_H  1

#include "classc.h"

@interface DClass : CClass
{
	int 	dvar;
}


/* -------------------  Public  Methods  ------------------- */

- awake ;
- (int) mycount ;
- (int) capacity ;
- next ;

@end
#endif


/*  class @interface :  CClass                                   */

#ifndef _MAKER_CCLASS_H
#define _MAKER_CCLASS_H  1

#include  "classb.h"

@interface CClass : BClass
{
	int 	cvar;
}


/* -------------------  Public  Methods  ------------------- */

- awake ;
- (int) mycount ;
- (int) capacity ;
- next ;

@end
#endif


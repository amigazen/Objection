/*  class @interface :  BClass                                   */

#ifndef _MAKER_CLASSB_H
#define _MAKER_CLASSB_H  1

#include  "classa.h"

@interface BClass : AClass
{
	int 	bvar;
}


/* -------------------  Public  Methods  ------------------- */

- awake ;
- (int) mycount ;
- (int) capacity ;
- next ;

@end

#endif


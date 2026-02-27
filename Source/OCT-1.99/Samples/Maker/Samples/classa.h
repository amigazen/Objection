/*  class @interface :  AClass                                   */

#ifndef _MAKER_ACLASS_H
#define _MAKER_ACLASS_H  1

#include  <objc/Object.h>

@interface AClass : Object
{
	int 	avar;
}

/* -------------------  Public  Methods  ------------------- */

- awake ;
- (int) mycount ;
- (int) capacity ;
- wankel:(int) cylinder position:(int) pos ;
- foo ;
- bar ;
- first ;
- next ;

@end
#endif


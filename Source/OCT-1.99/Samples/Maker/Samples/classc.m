//   classc.m   One of four classes that depend on the others  nov 91, bjw
//	:ts=8

//	This file is a group of four classes. They form a single
//	inheritance chain.  This is useful in testing method searching
//	and caching of methods.
//
//	23-nov-91   bjw   initial coding.
//

#include "classc.h"


/////////////////////////////////////////////////////////////////////////


@implementation CClass : BClass

- awake
{
    return self;
}    // - awake

- (int) mycount
{
    return 0;
}   // - count

- (int) capacity
{
    return 0;
}   // - capacity

- next
{
    return nil;
}   // - next

@end


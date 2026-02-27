//   classa.m   One of four classes that depend on the others  nov 91, bjw
//	:ts=8

//	This file is a group of four classes. They form a single
//	inheritance chain.  This is useful in testing method searching
//	and caching of methods.
//
//	23-nov-91   bjw   initial coding.
//

#include "classa.h"


/////////////////////////////////////////////////////////////////////////


@implementation AClass : Object

- awake
{
    return self;
}    // - awake

- (int) mycount
{
    return 0;
}   // - mycount

- (int) capacity
{
    return 0;
}   // - capacity

- wankel:(int) cylinder position:(int) pos 
{
    return self;
}

- foo
{
    return nil;
}

- bar
{
    return self;
}

- first
{
    return self;
}

- next
{
    return nil;
}

@end


// --------------------- test.m

#include "test.h"

@implementation Thing : Object


+ new
{
	printf ("my new routine\n");
	return [super new];
}

- free
{
	printf ("my free routine\n");
	return [super free];
}

@end


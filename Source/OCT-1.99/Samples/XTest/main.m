// --------------------- main.m

#import "t7.h"
#import <objc/System.h>
#include "test.h"
@includes

main ()
{
	id	thing;

	[System dumpDictionary:Thing];

        printf( "\n----- TEST 1 ------\n" );
	thing = [Thing new];
	thing = [thing free];

        printf( "\n----- TEST 2 ------\n" );
        thing = [BongoDude new];    [thing show];
        [thing fill];              [thing show];
        [thing cleanse];           [thing show];
        thing = [thing free];

        printf( "\n----- FINISH ------\n" );
}


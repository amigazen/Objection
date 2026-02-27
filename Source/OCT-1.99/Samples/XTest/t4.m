//   Test File #4   for O.O.C.                            July 89, bjw
/*
 *    19-sept-93    bjw   Added double indirection of simple array.
 */

#import <objc/ByteArray.h>

extern char 	*yytext;

extern id	sam;

/*  This test was posted to USENET by David McIntyre.
From: mcintyre@turing.cs.rpi.edu (David McIntyre)
Newsgroups: comp.lang.objective-c
Subject: You know what I hate.....
Message-ID: <-T4%8D_@rpi.edu>
Date: 23 Oct 90 23:43:37 GMT
Reply-To: mcintyre@turing.cs.rpi.edu (David McIntyre)
Organization: Rensselaer Instipoly Technitute
Lines: 47

...that Objective-C barfs on the following code:

I reported the first bug to Stepstone a very long time ago (maybe over a
year) but never received either a confirmation, denial or bug fix.  Sigh.

				-Dave

Dave "mr question" McIntyre     |      "....say you're thinking about a plate
mcintyre@turing.cs.rpi.edu      |       of shrimp.....and someone says to
office : 518-276-8633		|	you 'plate,' or 'shrimp'......"

*/

typedef struct {
        long flags;  /* marks which fields in this structure are defined */
        int x, y;           /* obsolete for new window mgrs, but clients */
        int width, height;       /* should set so old wm's don't mess up */
        int min_width, min_height;
        int max_width, max_height;
        int width_inc, height_inc;
        struct {
                int x;  /* numerator */
                int y;  /* denominator */
        } min_aspect, max_aspect;
        int base_width, base_height;          /* added by ICCCM version 1 */
        int win_gravity;                      /* added by ICCCM version 1 */
} XSizeHints;


/* --------------  Begin Custom objC stuff  ---------- */

@protocol

- eachElement ;
- add:obj ;
- at: (int) i put: obj ;
- toFirst ;

@end

@interface Collection : Object
{
	id 	contents;
}
- x ;
- y ;
@end

/* ----------  from file:  Node.h  ----------- */

@interface Node : ByteArray
{
	id	   subViews;
	int    isDefined;
}   // Another fast comment

- faster ;

@end

/* ----------  from file:  doobie.h  ----------- */

@interface Node ( doobie )

- (char) takeOneChar: (char) letter ;
- enumerate ;
- work ;

@end


/* ----------------------------------------------------------- */

@implementation Node ( doobie )

- (char) takeOneChar: (char) letter
{
     return letter ^ 1;      /* Toggle lowest bit */
}

- enumerate
{
	id	spot, table;
	extern char   *arguments[6], firstCh;

    /*  See if the assignment to 'spot' disappears! :-(  */
	if( ! [(spot=[table x]) work] ) 
	{
		if( (firstCh = [self takeOneChar:arguments[1][0]]) ) 
			printf( "got it" );
	}

	/*  Don't forget to balance your square-brackets: */
	printf( "1X %s %s.\n", [[spot x] str], [[spot y] str] );

	spot = [self faster];
	for( [self toFirst] ; [self count] ; [self next] )
		[self str];

        if( spot )
               return [self faster] ;
    return (id) ( (XSizeHints *) [spot name] );
}


typedef struct {
	id 	b[20];
	id 	z[20];
}  ZEE;

typedef struct {
	ZEE *y, *b;
}  WHY;

- work
{
	extern unsigned char	jones;
	id	 *x;
	WHY  *w, *a;

	[self->subViews eachElement];
	[x[10]  new];
	[w->y->z[10] new ];
	[a->b[jones] add:[[Collection new] x]];
	[isDefined new];

	[[Collection new] add:subViews];
	[sam at:2 put:[Object new] ];

	return subViews;
}

@end


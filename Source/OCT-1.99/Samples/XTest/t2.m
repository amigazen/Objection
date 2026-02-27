//   This is a little test  :bk=0    ((( t2.m )))
//	Tests parsing types in instance variable.

#include <objc/ByteArray.h>
#include <objc/IdArray.h>
#include <collect/Collection.h>

@protocol AsyncObject

- display: thing except: excepts ;
- suspend ;
- resume ;
- answer ;
- restart ;
@end

@interface Junk : Object

/*  These three statements should produce warnings from the translator.
 * 	All redefine a return type.
 */
+ (int) new ;
- (long) hash ;
+ poseAs: (int) notAClass ;
- x ;

- R1 ;
- Xreg ;
- regAt: (int)index ;
- setCC: (int)condCode ;

@end

/* -------------------------------------------------------- */

struct    Food    {    int    fing   ;    int    fong   ;    }    ;
typedef    struct    Food    FOOD    ;

typedef  struct  Zaphod  {  char  uu  [3]  ;  }  Zippy  ;

Zippy zapme [] = { {0, 2, 4} , {3, 5, 7} };

@interface Set : Collection 
{
	id   regSet, decoder;	// First line..
	unsigned arena_size,capacity;FOOD f;id subViews;
}

+ at:(int) garbage put:obj ;
- (short) shortAt:(int) anOffset ;

@end

/* ----------------------------------------------------------- */


@implementation Set : Collection


/********  ALL FUNCTIONS MUST APPEAR AFTER @IMP or @INCLUDES()  ********/

    static char
comparison( x, y ) id x, y;
{
    return '=' ;
}

- copy
{
    [regSet setCC:
	        comparison( [regSet Xreg], [regSet regAt:[decoder R1]] ) ];
}   // - copy

#if 1
- mycopy 
{
    comparison( [regSet Xreg], [subViews str] ) ;

    [regSet setCC:
	        comparison( [regSet Xreg], self ) ];

    [regSet setCC:
	        comparison( self,
			            [regSet regAt:[decoder R1]] ) ];
}   // - mycopy



- (int) capacity
{
	/* reach into another class and read its mind! */
	struct _xx { @defs(IdArray) }	*id_ptr;

	id_ptr = (struct _xx *) self->contents;  /* Grab pointer to array obj */
	printf( "its capacity=%u\n", id_ptr->capacity );

	return( (int) [id_ptr capacity] );
}

+ at:(int)garbage put:anObject
{
	id	*there;
	id 	list, target;

	if( [(list=[self x]) isEqual: anObject] )
		[list add: (target=[ByteArray str:" zap  "]) ];

	[self error:"%s can't %d", [self name], garbage];

	[*there str];
	[*there++ hash];
	[(*there)->subViews eachElement];
	*there = [self display: ((int)1) except: ((id)0)];

	[self perform: @selector(at:put:) with: ((8)) with: self ];

	if([[super new] count]<garbage) { 
		there[garbage] = anObject ;
	} else
		[self boundsViolation:garbage];
	[[IdArray new] at:garbage put:*there];
	return( anObject );
}

//  Give the user some access:
- at:(int) anOffset
{
	id	bigCities;
	int  x = (int) ((long) (self->contents));

	f.fong = -1;
	if( x ) { [self hash]; }

	x += (long) [super count] + @selector(show);
	bigCities = [IdArray with:3, [ByteArray str:"New York"],
			[ByteArray str:"Los Angles"],
			[ByteArray str:"Chicago"] ];

	x %= (short) IV(self)[anOffset];
	return [bigCities suspend];
}

- (int) count
{ return (int) arena_size; }
#endif

@end


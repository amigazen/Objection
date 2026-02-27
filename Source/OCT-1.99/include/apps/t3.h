//   interface for:  T3.M

#include <objc/objc.h>


/*  "collection" is defined as our super-root class */
@interface  Collection
{
	Class     isa;
	UWORD     sysFlags;
}

+ new ;
- at:(int) off put:anObj ;
- error:(STR) msg vaDcl:what_ever ;
- (unsigned) hash ;
- class ;
- (BOOL) isKindOf: class ;
- charAt:(int) off put:(char)letter ;
- (STR) describe ;
- (STR) str ;
- next ;
- (BOOL) isMarked ;
- show ;

@end


struct Window	{  short	x, y; APTR there; } ;

/*  "t3.m" uses this class: */
@interface  Stupid : Collection 
{
	int  arena_size ;
	id   collect;
	struct Window	*wndw;
}

+ at:(int)anOffset  put:anObject ;
- class ;
- (short) at:(int) anOffset ;
- findSTR: (STR) aStr ;

@end 

/* -------------------------- */
/*  A known, external class:  */
@interface  Sequence : Collection ;

+ perform:(SEL)aSel with:thang ;

@end


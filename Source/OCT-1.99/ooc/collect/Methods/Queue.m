/*		Class:	Queue   Double Ended Queue  	*/
/*                      */
/*		This class stores objects added in the order they were added. */
/*		Duplicates may be added repeatedly.  Added may occur at       */
/*		either end.  When an item is removed, the contents are packed */
/*		down.       	  */
/*                      */

#include <collect/Queue.h>
#include <objc/objc-runtime.h>


@implementation  Queue : Collection ;



/*  Control the expansion of the set:  */
#define  Q_EXPAND(id)	((id)->contents == nil || \
 (id)->firstEmptySlot+2 >= CAPACITY(self->contents) )

#define  FIRST_EXPAND 	(9)
#define  NEXT_EXPAND(self)  	((CAPACITY((self)->contents)*4)/3)



/* --------------------------------------------------------------------- */


- expand
{
	id	prev;
	int zz;

	if( self->contents == nil )
		self->contents = [IdArray new:FIRST_EXPAND ];
	else
	{
		prev = self->contents;
		zz = firstEmptySlot;
		self->contents = [IdArray new:NEXT_EXPAND(self) ];
		[prev addContentsTo: self ];
		[prev free];
#if SW_DBG_WATCH
		OC_SBG_PF(( " (OrderedCollection expand from %d to %d) ",
					zz, [self->contents capacity] ));
#endif
	}
	return( self );
}	/* - expand */


/* -----------------------------  Adding  ----------------------------- */


//	Store the non-nil item into an empty hash bucket:
- add: newObject
{
	id	*p;

	if( newObject != nil )
	{
		if( Q_EXPAND( self ) )
			[self expand];
		ARRAY( self->contents ) [firstEmptySlot++] = newObject;
	}
	return( self );
}


- at: (int) offset
{
	return [self->contents at:offset];
}


- at: (int) offset put: obj
{
	return [self->contents at:offset put:obj];
}


//	Add ourselves into the collection
-addContentsOf: aCollection
{
	[super addContentsOf: aCollection ];
	firstEmptySlot = (short) [self->contents count];
	return( self );
}



/* ------------------------  Testing  --------------------------------- */


//	Reply how many we're currently holding:
- (int) count
{
	return firstEmptySlot;
}



/* -------------------------  Removing  ------------------------------- */


//	Pack down the contents array removing all nils:
- packContents
{

	[self->contents packContents];
	firstEmptySlot = (short) [self count];
	return( self );
}



//	Remove an object, returning object if found
- remove: anObject
{
	int 	off = [self offsetMatching:anObject];
	id  	cut = nil;

	if( off >= 0 )
	{
		cut = [self at:off];
		[self at:off put:nil];
		[self packContents];
	}
	return( cut );
}


- boundsViolation: (int)anOffset
{
	return [self error:"cannot access element %d (firstEmpty=%d, cap=%d)",
						anOffset, firstEmptySlot, [self capacity] ];
}

@end


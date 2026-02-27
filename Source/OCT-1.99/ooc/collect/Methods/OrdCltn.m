/*		Class:	OrderedCollection  	*/
/*                      */
/*		This class stores objects added in the order they were added. */
/*		Duplicates may be added repeatedly.  Adding is on rear.       */
/*		When an item is removed, the Collection is packed down. 	  */
/*                      */
/*		Similar to Smalltalk Bag class.  Removal consists of locating */
/*		the item you're interested in and calling remove:.            */
/*                      */

#include <collect/OrdCltn.h>


@implementation  OrderedCollection : Collection ;



/*  Control the expansion of the set:  */
#define  Q_EXPAND(id)	((id)->contents == nil || \
 (id)->firstEmptySlot+2 >= CAPACITY(self->contents) )

#define  FIRST_EXPAND 	(9)
#define  NEXT_EXPAND(self)  	((CAPACITY((self)->contents)*4)/3)


/* --------------------------------------------------------------------- */


//	Expands our 'contents holder' one step larger:
- expand
{
	id	prev = self->contents;
	int zz = firstEmptySlot;   	/* dbg - hold previous size */

	if( prev == nil )
		self->contents = [IdArray new:FIRST_EXPAND ];
	else
	{
		self->contents = [IdArray new:NEXT_EXPAND(self) ];
		[prev addContentsTo: self ];
		[prev free];
#if SW_DBG_WATCH
		OC_SBG_PF(( " (OrderedCollection expanded %d items into %d) ",
					zz, [self->contents capacity] ));
#endif
	}
	return( self );
}	/* - expand */


/* -----------------------------  Adding  ----------------------------- */


//	Store the non-nil item into an empty slot (towards the end):
- add: newObject
{

	if( newObject != nil )
	{
		if( Q_EXPAND( self ) )
			[self expand];
		ARRAY( self->contents ) [firstEmptySlot++] = newObject;
	}
	return( self );
}	/* - add: */


- at: (int) offset
{
	return [self->contents at:offset];
}


- at: (int) offset put: obj
{
	return [self->contents at:offset put:obj];
}


- firstElement
{
	return[ self at:0];
}


- lastElement
{
	return[ self at:firstEmptySlot];
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
#if 1
    return [self->contents remove:anObject];
#else
	int 	off = [self offsetMatching:anObject];
	id  	cut = nil;

	if( off >= 0 )
	{
		cut = [self at:off];
		[self->contents at:off put:nil];   	/* Replace hole with 'nil' */
		[self packContents];
	}
	return( cut );
#endif
}	/* - remove: */


- boundsViolation: (int)anOffset
{
	return [self error:"cannot access element %d (firstEmpty=%d, cap=%d)",
						anOffset, firstEmptySlot, [self capacity] ];
}

@end


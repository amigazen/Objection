/*		Class:	Set  	*/
/*	03-aug-89	bjw 	OOC service
/*                      */

/*  add:		method discards any duplicates added; the original is
//				preserved.
*/

#include <collect/Set.h>
#include <objc/objc-runtime.h>


@implementation  Set : Collection 



/*  Control the expansion of the set:  */
#define  Q_EXPAND(id)	((id)->contents == nil || \
 (id)->tally >= ((4*CAPACITY(self->contents))/5) )

#define  FIRST_EXPAND    	(9)
#define  NEXT_EXPAND(self)  	((CAPACITY((self)->contents)*4)/3)


/* -----------------------  Support Code  ------------------------------ */


/*   findElementOrNil  --  Searches contents looking for a match.  Since
//					   hashing is done with overflow into next bucket
//					   instead of linking, any deletions will screw
//					   up the search, since it stops at a 'nil'
//			element.  Such are preformance tradeoffs...
*/
	id *
findElementOrNil( self, element )
	id	self;
	id	element;
{
	register id  	*now;
	register int 	n;
	id  	 *end, *begin;
	int 	tmp;

	if( self->contents == nil )
		[self expand];

	n = CAPACITY( self->contents );
	begin = ARRAY( self->contents );
	if( (tmp=[element hash]) == 0 )
	{
		now = begin + ( tmp % n );
		end = begin + n;
		for ( ; --n >= 0 ; now++ )
		{
			if( now >= end )
				now = begin;
			if( *now == nil || [*now isEqual:element] )
				return now;
		}
	}
	else
	{
		/*  If the element doesn't hash, then exhaustively search the
		//	entire set looking for an equal.  We remember the last
		//	empty slot in the set in case no match is found.
		//	We could call "self->contents" to search, but that would
		//	require two passes through the array: [offsetOf:element ..],
		//	then [offsetOf:nil ..] if not found.
		*/
		now = begin;
		end = (id *) nil;
		for( ; --n >= 0 ; now++ )
		{
			if( *now == nil )
				end = now;
			else
			if( [*now isEqual:element] )
				return now;
		}

		if( end != (id *)nil )
			return( end );

	}	/* else non-hashing element */

	[self error: "(findElementOrNil) failure", 0 ];
}	/* findElementOrNil() */



- expand
{
	id	prev;
	int zz;

	if( self->contents == nil )
		self->contents = [IdArray new:FIRST_EXPAND ];
	else
	{
		prev = self->contents;
		zz = [prev count];
		self->contents = [IdArray new:NEXT_EXPAND(self) ];
		tally = 0;
		[prev addContentsTo: self ];
		[prev free];
#if SW_DBG_WATCH
		OC_DBG_PF(( " (Set expand from %d to %d) ",
					zz, [self->contents capacity] ));
#endif
	}
	return( self );
}


/* -----------------------------  Adding  ----------------------------- */


//	Store the non-nil item into an empty hash bucket:
- add: newObject
{
	id	*p;

	if( newObject == nil )
		return nil;
	if( Q_EXPAND( self ) )
		[self expand];
	p = findElementOrNil( self, newObject );
	if( *p == nil )
	{
		*p = newObject;
		++tally;
		return( nil );
	}
	return( newObject );			/* Duplicate */
}



//	Insert a new object, return old object if replacing or new if original
- filter: newObject
{
	id	*p;

	if( newObject == nil )
		return nil;
	if( Q_EXPAND( self ) )
		[self expand];
	p = findElementOrNil( self, newObject );
	if( *p == nil )
	{
		++tally;
		return( *p = newObject );		/* Store new value and return */
	}
	[newObject free];
	return( *p );
}


//	Add ourselves into the collection
-addContentsTo: aCollection
{
	register id	*member;
	int  n = [self capacity];

	member = ARRAY( self->contents );
	while( --n >= 0 )
		[aCollection add: (*member++) ];
	return aCollection;
}




/* ---------------------------  Combining  ----------------------------- */


//	Return the union (add) of ourselves with another collection thing
- union: aCollection
{
	id	all = [[self class] new];

	return [ [all addContentsOf:self]  addContentsOf:aCollection] ;
}


//	Returns the intersection (common to both) collection things
- intersection: aCollection
{
	id	all, tmp;

	all = [self union: aCollection];

	tmp = [self difference: aCollection];
	[all removeContentsOf: tmp];
	[tmp free];

	tmp = [aCollection difference: self];
	[all removeContentsOf: tmp];
	[tmp free];

	return all;
}



//	Return the difference of two collection things
- difference: aCollection
{
	id	all = [[self class] new];

	return [ [all addContentsOf:self]  removeContentsOf:aCollection] ;
}




/* ------------------------  Testing  --------------------------------- */


- (int) count
{
	return tally;
}


//	Search for the object, returning it, or nil if not found
- find: anObject
{
	return( * findElementOrNil(self, anObject) );
}



//	Return a Boolean if an object exists within
- (BOOL) contains: anObject
{
	return(BOOL) ( [self find: anObject] != nil );
}



/* -------------------------  Removing  ------------------------------- */


//	Remove an object, returning object if found
- remove: anObject
{
	id	old, *p = findElementOrNil( self, anObject );

	old = *p;
	*p = nil;
	return( old );
}


@end


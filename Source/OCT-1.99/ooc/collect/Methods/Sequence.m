/*		Class:	Sequence.m		*/

/*
//    Provides an enumerating class.  You add elements by pointing at
//    an existing IdArray object.  Complements the services provided
//    by an OrderedCollection.  Instead of 'rewind', we use 'toFirst'.
//
//    03-aug-89    bjw   OOC service
//    20-aug-89    bjw   Motion methods return self
//    26-april-91  bjw   +array: accepts a collection, also!
*/



#include <collect/Sequence.h>


@implementation  Sequence : Collection ;
/*
	short 	offset;  		-- Current offset into 'contents'
*/


/* -------------------  Allocation / Deallocation  --------------------- */


//	Create a new Sequence from the given "IdArray" type:
//  Will also extract the "array" from another collection!
+ array: anIdArray
{
	id  	obj = nil;

    if( anIdArray != nil )
	{
	    if( [anIdArray isKindOf: Collection] == YES )
		{
            //  Opps, it's actually a collection.  Convert it to sequence.
			id   collect = (id) anIdArray;
            anIdArray = collect->contents;
		} else
		{
    	    if( [anIdArray isMemberOf: IdArray ] == NO )
    	    	return [self error: "IdArray or Collection needed", 0 ];
        }

        obj           = [self new];
        obj->contents = [anIdArray copy];
   	    obj->offset   = 0;
    }
	return( obj );
}	/* + array: */



/* ------------------------  Sequencing  ----------------------------- */


//	Look through contents for the "next" non-nil item:
//	If 'offset' is 0, then return first item in 'contents'.
- next
{
	register id  *next_id;
	int     size;

	/*  Are there any elements left? */
	size = [self capacity];
	if( size <= offset || size == 0 )
		return( nil );

	next_id = ((id *)IV(self->contents)) + offset ;
	for( ; ++offset <= size ; ++next_id )
	{
		if( *next_id != nil )
			return( *next_id );
	}
	return( nil );			/* Ran out of elements to search */
}	/* - next */


//	Move back to the previous non-nil member:
- previous
{
	id	*prev_id;

	if( [self count] == 0 )
		return nil;
	prev_id = ((id *) (self->contents)) + offset;
	while( --offset > 0 )
		if( *--prev_id != nil )
			return( *prev_id );

	return( nil );
}	/* - previous */


//	Position the cursor to the beginning of the list (like rewind):
- toFirst
{
	offset = 0;
	return( self );
}


//	Position the cursor just past the end of the list:
- toLast
{
	offset = (short) [self->contents capacity];
	return( self );
}



//	Rewind the "cursor" to the first non-nil member, returns first:
- first
{
	offset = 0;
	return( [self next] );
}


//	Advance forward to the last significant element, returns last:
- last
{
	offset = (short) [self->contents capacity];
	return( [self previous] );
}



//	Returns count of number of elements remaining to sequence over.
- (int) count
{
	return [self->contents count] - offset;
}

@end


/*   Class:   Collection      */

/*
 *    A parent collection class that defines how more specific collections
 *    ought to behave.  Assumes the instance variable 'contents' points
 *    to an IdArray object.
 *
 *   03-aug-89   bjw    OOC service
 *   10-may-91   bjw    Handles empty 'contents'; doesn't message to nil.
 */


#include <string.h>

#include <collect/Collection.h>
#include <collect/Sequence.h>


@implementation  Collection
/* Collection : Object { id contents; }  */

/*  "Set" and "OrderedCollection" are not included above since
//	no new methods are added from these classes.  For the time
//	being, it's easier just to declare them external reference.
*/


/* ---------------------  Instance Variables  -------------------------- */



//   Create an sized instance:
+ new: (int)size
{
   id   obj;

   obj = [self new];
   obj->contents = [IdArray new:size];
   return( obj );
}



//   Create an initialize instance:
+ with: (int)nArgs vaDcl:args
{
   id   new, *p = & args;

   new = [self new:nArgs];
   while( nArgs-- )
   {
      [new add: (*p++)];
   }
   return( new );
}


//  Free items we hold in array.  Array is maintained.
- freeContents
{

   /*  Free whatever we're holding in Array thing, but not array: */
   if( contents != nil && contents->isa != Nil )
	   [contents freeContents];
   return self;
}


//   Free the array, but not the items it held.  Free's self, too!
//   Off to the bit bucket!
- free
{
   if( contents != nil && contents->isa != Nil )
       [contents free];  	/* Free Array thing, not its contents */
   return [super free];
}


+ initialize
{

	/*  ONLY Collection may pass, others continue the search: */
	if( self != &instCollection )
		return [instCollection.super_class initialize] ;

	if( !CLS_GETINFO(self, CLS_INITIALIZED) )
	{
		CLS_SETINFO( self, CLS_INITIALIZED );
		[self->super_class initialize]; 	/* Continue init'ing */
		[IdArray        initialize];    	/* And don't forget */
	}
	return( (id) self );
}	/* + initialize */


//   Expand our holding capacity.  Also called if there is no contents
//   and we need to access it directly for some object addition.
//   Should be over-ridden in children classes.
- expand
{
    return [self error:"Can't add to nil contents"];
}    // - expand


/* ---------------------------  Adding Members  ------------------------ */



//   Add a new thing to the collection of known things:
- add: newObject
{
   return [self subClassResponsibility:_cmd];
}


//   Add our contents to aCollection, replying the aCollection.
- addContentsTo: aCollection
{
    if( contents == nil )
        [self expand];
    [contents addContentsTo: aCollection];
    return( aCollection );
}



//   Add the contents of aCollection to ourselves, replies self.
- addContentsOf: aCollection
{
   [aCollection addContentsTo: self];
   return( self );
}



/* ----------------------------  Removing  ---------------------------- */



//  Remove anObject from our contents, replies anObject or nil if not found.
- remove: anObject
{
   return [self subClassResponsibility:_cmd];
}



//   Removes any number from ourselves that are also members of aCollection:
- removeContentsOf: aCollection
{
   [contents removeContentsOf: aCollection];
   return( self );
}


/* --------------------------  Enumeration  ----------------------------- */



//   Replies a Sequence of ourselves. Caller owns it.
- eachElement
{

    if( contents != nil )
        return [Sequence array:contents];
    return [Sequence new:1];
}


//   Broadcast a selector to each member of ourselves:
- eachElementPerform: (SEL) aSelector
{
	id 	result = nil;

	if( contents != nil )
	   result = [contents eachElementPerform:aSelector];
	return( result );
}   // eachElementPerform:


- eachElementPerform: (SEL) aSelector with:arg1
{
	id 	result = nil;

	if( contents != nil )
	   result = [contents eachElementPerform:aSelector with:arg1 with:nil];
	return( result );
}   // eachElementPerform:with:


//   Broadcast a selector to each member of ourselves:
- eachElementPerform: (SEL) aSelector with:arg1 with:arg2
{
	id 	result = nil;

	if( contents != nil )
	   result = [contents eachElementPerform:aSelector with:arg1 with:arg2];
	return( result );
}   // eachElementPerform:with:with:



/* ----------------------  Class Conversions  ------------------------- */


//   Reply self as a set.  Caller now owns it.
- asSet
{
	extern id  Set;
   int    size = ([self count] * 6) / 5 + 1;

   return [self addContentsTo: [Set new:size] ];
}



//   Reply self as an OrderedCollection
- asOrderedCollection
{
	extern id  OrderedCollection;

   return [self addContentsTo: [OrderedCollection new:[self count] ] ];
}



/* ---------------------------  Testing  ------------------------------ */



- (int) count
{
   return [contents count];
}


- (int) capacity
{
   return [contents capacity];
}


- (BOOL) isEmpty
{
   return(BOOL) ( [contents count] == 0 );
}


- (BOOL) contains: anObject
{
   return(BOOL) ( [self find: anObject] >= 0 );
}



//   Compare instance to subject, YES if same (contents==)
- (BOOL) isCopyOf: anObject
{
   BOOL    same = NO;

   if( [anObject isMemberOf: [self class]] &&
       [anObject->contents isCopyOf:self->contents] )
   {
      same = YES;
   }
   return( same );
}


//   Copy the internal support structures:
- copy 
{
	id    	guy = [super copy];

	guy->contents = [contents copy];
	return( guy );
}	/* - copy */


/* --------------------------  Searching  ----------------------------- */


- (unsigned) hash
{
	int 	cap, size, spice;

	cap = [self capacity];
	size = [self count];
	spice = (int) ((ULONG)self >> 3);		/* Painless typecasting */

	return (unsigned) ((~size) ^ (cap*size) ^ spice) >> 1;
}


- find: anObject
{
    int    there;

    if( contents != nil && (there=[self offsetOf:anObject]) >= 0)
             return [contents at:there ] ;
    return nil;
}



- findMatching: anObject
{
    int    there;

    if( contents != nil && (there=[self offsetMatching:anObject]) >= 0)
             return [contents at:there ] ;
    return nil;
}



- findSTR: (STR) aString
{
    int    there;

    if( contents != nil && (there=[self offsetSTR:aString]) >= 0)
             return [contents at:there ] ;
    return nil;
}



- (int) offsetOf: anObject
{

    if( contents != nil )
        return [contents offsetOf: anObject];
    return -1;
}



- (int) offsetMatching: anObject
{
    if( contents != nil )
        return [contents offsetMatching: anObject];
    return -1;
}



- (int) offsetSTR: (STR)aString
{

    if( contents != nil )
        return [contents offsetSTR: aString ];
    return -1;
}

@end


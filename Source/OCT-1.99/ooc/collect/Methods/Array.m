/*	Class: Array	*/
/*	03-aug-89	bjw 	OOC service
/*	march-90 	bjw 	Revised header files.
/*	*/


#include <collect/Array.h>
#include <string.h>
#include <objc/objc-runtime.h>


@implementation  Array : Object ;


/* -----------------------  Creation Methods  ------------------------- */


//	Create a new self:
+ new: (int) nElements
{
	id	newObject;

	/*  Allocate room for array and header maintaince: */
	newObject = (*oc_alloc)( self, [self ndxVarSize] * nElements );
	newObject->capacity = nElements;
	return( newObject );
}


//	Create an initialized self.
//	usage:  id = [AnyArraySubClass with:3, entry1, entry2, entry3];
+ with: (int) nArgs vaDcl: arglist
{
	id	newObject;

	newObject = [self new:nArgs];
	memcpy( IV(newObject), (char *)&arglist, nArgs * [self ndxVarSize]);
	return newObject;
}


/* -----------------------  Indexed Instancing  ------------------------ */


//	Declare the type of what's in the array:
+ (STR) ndxVarType
{
	[self subClassResponsibility: _cmd];
	return "";
}



+ (int) ndxVarSize
{
	[self subClassResponsibility: _cmd];
	return 0;
}


#if 0
- (STR) describe
{
	[self subClassResponsibility: _cmd];
	return "";
}
#endif


//	Some details about how much we have and how much we can hold:
- (int) size
{
	return capacity;	/* Others may re-declare as non-Nil element count */
}


- (int) capacity
{
	return capacity;
}


/* -----------------------  Compare and Order  ------------------------- */


//	Please over-ride me in descendant classes!
- (unsigned) hash
{
	return( (~capacity | (int)(self->isa)) & 0x07FFF );
}


//	Grow (or shrink) ourselves to better accomodate reality:
- capacity: (int) nSlots
{
	return (*oc_realloc)( self, 
			[self ndxVarSize] * nSlots + self->isa->clsSizInstance );
}


//	Is the receiver equal to the argument?
- (BOOL) isEqual: anObject
{
	return (BOOL) (self == anObject);
}


//	Is the receiver and argument containing the same objects
//	ie, are they copies?
- (BOOL) isCopyOf: anObject
{
	register char	*s, *t;
	int 	size;

	/*  Compare whole object to each other: */
	size = [self->isa ndxVarSize] * self->capacity +
					self->isa->clsSizInstance;	/* encode of 'clsTypes' */
	s = (char *) self;
	t = (char *) anObject;
	while( --size >= 0 && *s++ == *t++ )
		;;
	return (BOOL) (size < 0);
}



//	Not logical, Sir.  Your index is out of range:
- boundsViolation: (int) anOffset
{
	return( [self error: capacity > 0
				? "bounds violation: %d outside range [0..%d]"
				: "zero capacity array", anOffset, capacity-1] );
}

@end


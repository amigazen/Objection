//    NODE.M   Node in a graph of named nodes  (Objective C book, pg 107)

#include "Node.h"

@implementation Node : ByteArray


//	This node references another node, replies Node
- addReference: anotherNode
{

	if( references == nil )
		references = [Set new];
	[references add:anotherNode];
	return self;
}


//	Mark myself and my dependents as reachable
- mark
{

	if( isMarked == YES )
		return nil;
	isMarked = YES;
	if( references != nil )
		[references eachElementPerform: @selector(mark) ];
	return self;
}


//	Mark myself as defined, replies Node.
- define
{
	isDefined = YES;
	return self;
}

//	Reply whether I am reachable or defined.
- (BOOL) isMarked
{
	return isMarked;
}

- (BOOL) isDefined
{
	return isDefined;
}

@end


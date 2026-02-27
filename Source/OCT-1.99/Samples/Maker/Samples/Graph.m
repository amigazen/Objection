//    GRAPH.M   Dependency Graph        (Objective-C book, pg 107)

#include "Graph.h"

@implementation Graph : Set


/* -------------------------  Class Code  ----------------------------- */


//	Add a new node, replies a Node.
- addNode:(STR) aNodeName
{
	return [self filter:[Node str:aNodeName] ];
}


//	Add a new node and define it, replies a Node.
- defineNode:(STR) aNodeName
{
	return [[self addNode:aNodeName] define];
}


//	Mark the given node (and all nodes referenced by it).
- mark: (STR) aNodeName
{
	return [[self addNode:aNodeName] mark];
}

@end


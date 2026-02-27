//   TREE.h   Test out tree-graph-node trio (Test Module)     Jan 90, bjw


#include <objc/Object.h>

@interface Tree : Object
{
	id 	graph;
}

/* -------------------  Public Methods  ------------------- */

+ new ;
- addNode: (STR) aNodeName;

@end


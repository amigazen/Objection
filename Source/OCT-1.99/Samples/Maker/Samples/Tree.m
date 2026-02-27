//   TREE.M   Grabs graph and node classes (Test module)   Jan 90, bjw
//

#include  <apps/tree.h>

/*  Dummy declaration  */
@interface Graph : Set
@end


@implementation  Tree : Object ;

+ new 
{
	id 	newobj = [super new];

	if( (newobj->graph = [Graph new]) == NULL )
		printf( "Tree can't alloc a graph\n" );

	return newobj;
}


- addNode:(STR) aNodeName
{
	return [self->graph addNode:aNodeName ];
}

@end


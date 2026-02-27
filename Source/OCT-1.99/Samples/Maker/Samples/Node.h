//     class interface:  node.m                               Sept 89, bjw

#ifndef  APPS_NODE_H
#define  APPS_NODE_H 1

#include <objc/ByteArray.h>
#include <collect/Set.h>


@interface  Node : ByteArray
{
	BOOL isDefined, isMarked;
	id references;
}

- addReference: anotherNode ;
- mark ;
- define ;
- (BOOL) isMarked ;
- (BOOL) isDefined ;

@end

#endif


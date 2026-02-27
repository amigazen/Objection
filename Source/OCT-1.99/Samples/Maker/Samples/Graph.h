//    class interface:  graph.m                         Sept 89, bjw


#include <collect/Set.h>
#include "Node.h"


@interface Graph : Set ;


- addNode:(STR) aNodeName ;
- defineNode:(STR) aNodeName ;
- mark: (STR) aNodeName ;

@end


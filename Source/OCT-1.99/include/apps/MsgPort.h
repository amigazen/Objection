/*  class @interface :  MsgPort                            April 90, bjw */

#include <objc/Object.h>
#include <exec/ports.h>


@interface MsgPort : Object
{
	struct MsgPort	*port;   		/* Port we're maintaining */
	struct Message	*message;		/* Current Message */
	short 		port_flags;
}


/* -------------------  Public Methods  ------------------- */

+ newSTR: (STR) port_name ;
- free ;
- findSTR:(STR) port_name ;
- str: (STR) port_name;
- (STR) str ;
- dropPort ;
- takePort: (struct MsgPort *) leach_port ;
- (STR) str ;
- (struct Message *) first ;
- (struct Message *) next ;
- (struct Message *) replyMsg ;

@end


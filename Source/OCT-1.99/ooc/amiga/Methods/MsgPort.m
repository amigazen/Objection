/*   MsgPort.M   Amiga Exec Message Port                   April 90, bjw */

/*
//		The message port has a notion of the current message, and
//		how to obtain the next message.  Repy messages can also
//		be handled.
//
//		4-april-90	bjw 	Initial coding.
*/

#include <octhead.h>
#include <apps/MsgPort.h>


@implementation  MsgPort : Object
/*
	struct MsgPort	*port;
	struct Message	*message;
	short   		port_flags;
*/

#define  PF_OWN_PORT	0x001
#define  PF_FREE_MEM	0x002


//	Create a named message port that we own.
+ newSTR: (STR) portname
{
	return [[super new] str:portname];
}	/* + newSTR: */


- free
{

	[self replyMsg];
	[self dropPort];
	return [super free];
}	/* - free */


/*************/
/**  USAGE  **/

//	Assume the guise of a specified port. You may be creating an duplicate!
//	 Better to use findSTR: instead.
- str: (STR) port_name 
{
	extern struct MsgPort	*CreatePort( );

	port = CreatePort( port_name, 0L );
	port_flags = PF_OWN_PORT;

	return( self );
}	/* - str: */


//	Answer the name of this message port.
- (STR) str
{
	STR 	name = "" ;

	if( port != NULL )
		name = port->mp_Node.ln_Name;
	return( name );
}	/* - str */


//	Search around for the named port.  If none found, then it creates a
//	 unique port.  Otherwise, answers nil.  This avoids duplicates.
- findSTR:(STR) port_name
{
	extern struct MsgPort	*FindPort( );
	id  	unique = nil;

	Forbid();
	if( FindPort( port_name ) == NULL )
	{
		[self replyMsg];
		[self dropPort];
		[self str:port_name];
		unique = self;
	}
	Permit();

	return( unique );
}	/* - findSTR: */


//  Discontinue use of a message port.  If we own, the delete it.
- dropPort
{
	extern void 	DeletePort( );

	if( port_flags & PF_OWN_PORT )
		DeletePort( port );
	port = NULL;
	message = NULL;
	port_flags &= ~PF_OWN_PORT;

	return( self );
}	/* - dropPort */


//	Start using an existing message port.
- takePort: (struct MsgPort *) leach_port
{

	[self dropPort];
	port = leach_port;
	return( self );
}	/* - takePort: */


/*****************/
/**  MESSAGING  **/

//	Return current message.  If none, then GetMsg().
- (struct Message *) first
{
	extern struct Message	*GetMsg( );
	struct Message	*mes = message;

	if( port != NULL )
	{
		if( mes == NULL )
			message = mes = GetMsg( port );
	}

	return( mes );
}	/* - first */


//	Advance to next waiting message, if any.
- (struct Message *) next
{
	extern struct Message	*GetMsg( );

	[self replyMsg];		/* Create a vacancy */
	if( port != NULL )
		message = GetMsg( port );
	return( message );
}	/* - next */


//	Take action with the current message, free'ing the 'message' ptr.
//	 Try and reply to message, otherwise, if it is a reply, then try
//	 and get rid of it.
- (struct Message *) replyMsg
{
	extern void 	ReplyMsg( );
	extern void 	FreeMem( );

	if( message != NULL )
	{
		/*  If the message isn't a reply, then get rid of it by replying.
		//	Otherwise try and de-allocate the replied message.  If we're
		//	allowed, we can FreeMem(), otherwise just drop it...
		*/
		if( message->mn_Node.ln_Type != NT_REPLYMSG )
			ReplyMsg( message );
		else
		if( port_flags & PF_FREE_MEM )
			FreeMem( message, (long) message->mn_Length );
		else
			return( message );  		/* Not sure how to get rid of msg */
		message = NULL;
	}

	return( NULL );
}	/* - replyMsg */


@end


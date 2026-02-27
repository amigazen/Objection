/*   RESPONDER.H   Basic Event Responder                  April 90, bjw */

/*
//		The responder class is the root for a hierarchy of things that
//		responding to windowing events.  This is an abstract class.
//		Child classes refine this into an outer window (with a frame),
//		into a small gadget or control element, or into the application
//		itself.
//
//		Responders have a title and an event handling chain.  Rendering
//		is not part of this class.
//
//		24-april-90	bjw 	This class coded.
*/


#include "responder.h"
#include <objc/objc-class.h>
#include "gui.h"
#include <apps/IdList.h>
#include <objc/ByteArray.h>
#include <functions.h>


#define  SW_DEBUG	0


@implementation Responder : Object
/*
	id  	strname;    	-- class: ByteArray.
	id  	delegates;  	-- sub-parts of this object.
	short 	categoryMask;	-- message categories we'd like to see
*/


struct Library	*IntuitionBase;
struct Library	*GfxBase;
struct Library	*LayersBase;


/* -------------------------------------------------------------------- */


	LOCAL void
view_close_all()
{

#if SW_DEBUG
	write( 2, "Close Libs\n", 11 );
#endif
	if( GfxBase )
		CloseLibrary( GfxBase );
	if( LayersBase )
		CloseLibrary( LayersBase );
	if( IntuitionBase )
		CloseLibrary( IntuitionBase );

	GfxBase = LayersBase = IntuitionBase = NULL;

}	/* view_close_all */


+ initialize
{

	/*  ONLY Responder may pass, others continue the search: */
	if( self != & instResponder )
		return [instResponder.super_class initialize] ;

	if( !CLS_GETINFO(self, CLS_INITIALIZED) )
	{
#if SW_DEBUG
		printf( "[Responder init]\n" );
#endif
		CLS_SETINFO( self, CLS_INITIALIZED );

		_oc_atExit( view_close_all );

		GfxBase       = OpenLibrary( "graphics.library", 33L );
		LayersBase    = OpenLibrary( "layers.library",   33L );
		IntuitionBase = OpenLibrary( "intuition.library", 33L );

		if( !IntuitionBase || !LayersBase || !GfxBase )
			return nil; 	/* ERROR: Resources not available */
	}
	return( (id) self );
}	/* + initialize */



/**************/
/*  STORAGE:  */

+ new
{
	id  guy = [super new];

	guy->strname = [ByteArray str: [self class]];
	guy->delegates = [IdList new];
	guy->categoryMask = CF_VIEW;
	return( guy );
}	/* + new */


- free
{

	[strname free];
	[delegates freeContents];
	[delegates free];
	return [super free];
}	/* - free */


/***********************/
/*  MESSAGE HANDLING:  */

- addHeadDelegate: handler
{
	return [delegates addFirst: handler] ;
}


- addTailDelegate: handler
{
	return [delegates addLast: handler] ;
}


- removeDelegate: handler
{
	return [delegates remove: handler] ;
}


- setCategory: (int) mask
{
	categoryMask = mask;
	return ( self );
}


- (int) getCategory 
{
	return( categoryMask );
}


- eachDelegatePerform: (SEL) sel with:arg 
{
	return [delegates eachElementPerform: sel with: arg ];
}


/**********************/
/*  NAME MANAGEMENT:  */

- (STR) getTitle
{
	return [strname str];   	// Return "C" string.
}

- setTitleSTR: (STR) name
{

	[strname free];
	strname = [ByteArray str: name];
	return( self );
}	/* - setNameSTR: */


@end


/*  class @interface :  Responder                                */

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

#include <objc/Object.h>

@interface Responder : Object
{
	id  	strname;    	// class: ByteArray.
	id  	delegates;  	// sub-parts of this object.
	short 	categoryMask;	// message categories we would like to see
}


/* -------------------  Public Methods  ------------------- */

/*  STORAGE:  */
+ new ;
- free ;

/*  EVENT HANDLING:  */
- addHeadDelegate: handler ;
- addTailDelegate: handler ;
- removeDelegate: handler ;
- setCategory: (int) mask ;
- (int) getCategory  ;
- eachDelegatePerform: (SEL) sel with:arg ;

/*  NAME MANAGEMENT:  */
- (STR) getTitle ;
- setTitleSTR: (STR) name ;

@end


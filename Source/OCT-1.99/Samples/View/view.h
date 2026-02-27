/*   View.H   View (user interface) for Amiga           March 90, bjw */

/*
//		Views are things that can respond to events.  A view is an
//		active element.  It has a boundry and a string attached to it.
//		These basic elements respond to events, letting the subViews or
//		dependent views handle the event.
//
//		Views can employe other views to present itself (dependents).
//		Views can also contain other views, to fullfil the needs of the
//		application programmer (subViews).  Except for the screen itself,
//		all views are contained within other views (superView).
//
//		4-april-90	bjw 	Initial re-coding.
//		24-april-90	bjw 	Parent class now Responder.
*/

#ifndef VIEW_H
#define VIEW_H	1

#include "responder.h"
#include <intuition/intuition.h>
#include "gui.h"


extern struct Library	*IntuitionBase;
extern struct Library 	*GfxBase;
extern struct Library 	*LayersBase;


@interface View : Responder
{
	id   	activeCtrl;    		/* Object currently taking events */

	id  	subViews;   		/* Views within us (children) */
	id  	superView;   		/* Who we're inside of */

	RECT  	hitArea;			/* Entirity of view */
	struct RastPort 	*wholeRastPort;		/* The whole view */
	short 	view_flags;

}


/*   Appearence codes for setAppearence:  method */
#define  VA_Close 		 0x001
#define  VA_Hide    	 0x002
#define  VA_Reveal  	 0x003
#define  VA_Mini    	 0x004
#define  VA_Normal  	 0x005
#define  VA_Max   		 0x006

/*  Private values for View->view_flags:  */
#define  VF_Hidden 		0x0001  	/* Are we Hidden? */
#define  VF_Maximized	0x0002  	/*     .. Max'ed? */
#define  VF_Minimized   0x0004  	/*     .. Min'ed? */

#define  VF_HasRP  		0x0010  	/* We have use of RP */
#define  VF_OwnsRP  	0x0020  	/* We own that RP! */
#define  VF_ParentsRP   0x0040  	/* Borrowing from superView */

#define  VF_Parenting   0x8000  	/* Private: setSuper:/setSubView: */



/* -------------------------------------------------------------------- */

/*  STORAGE:  */
+ initialize ;
+ new ;
- free ;

/*  RastPort Ownership Protocol:  */
- (struct RastPort *) useRP ; 			// Ask permission
- setRP: (struct RastPort *) rp;		// We are the owners
- dropRP: (struct RastPort *) rp;		// Done with it
- releaseRP: (struct RastPort *) rp;	// Owner wants us to release it


/*  ADJUSTMENT:  */
- setCategory: (int) mask ;
- (int) getCategory ;
- getSize: (RECT *) prect ;
- setSize: (register RECT *) prect ;
- setApperance: (int) appear ;

- doClose ;     	/* Shutdown and free */
- doHide ;      	/* Hide window from all screens */
- doReveal ;    	/* Restore window to its last screen */
- doSizeMini ;  	/* Shrink to minumum size */
- doSizeNormal ;	/* Size to a nominal size */
- doSizeMax ;   	/* Expand size to screen max */


/*  CONTROLS:  */
- findControlType: (int) mask ;

/*  RENDERING:  */
- drawSelf ;		/* Draw whole view */
- drawContent ;		/* Draw interior contents */
- drawBorder ;		/* Draw controls around content */

/*  SUB-PORTIONS: */
- addSubView: sub_view ;	/* Add dependent view */
- setSuperView: parent ;	/* Set containing view */

/*  CONTROL:  */
- (MSGCTX *) command: (MSGCTX *) mc ;   	/* Dispatch message */

@end

#endif


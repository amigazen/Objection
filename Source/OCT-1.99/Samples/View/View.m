/*   View.M   Basic View (user interface) for Amiga     March 90, bjw */

/*
//		This class implements a "window" abstraction named views.  This
//		is borrowed from Dr. Brad Cox, who probably is borrowing from
//		some one else (life goes on)...  The differences from Intuition
//		are:
//			1)	Generic renderings can be attached.
//			2)	User can subclass and extend a window class.
//			3)	Windows/Controls/Renderings can be accessed from
//				a resource file, ands thus shared among many users.
//
//		20-mar-90	bjw  	Initial coding.
//		24-apr-90	bjw 	child class of Responder.
*/

/*  We don't want preferences.. */
#define INTUITION_PREFERENCES_H  1

#include <exec/types.h>
#include "view.h"
#include <apps/IdList.h>
#include <functions.h>


@implementation View : Responder
/*
	id   	activeCtrl;    		-- Object currently taking events 

	id  	subViews;   		-- Views within us (children)
	id  	superView;   		-- Who we're inside of

	RECT  	hitArea;			-- Entirity of view
	RastPort	*wholeRastPort;		-- The whole view
	short 	view_flags;
*/


/* -------------------------------------------------------------------- */


/**************/
/*  STORAGE:  */
+ new
{
	id 		nude = [super new];

	nude->view_flags = VF_Hidden | VA_Hide ;
	nude->activeCtrl = nil;
	nude->wholeRastPort  = NULL;
	nude->hitArea.or.x   = 0;
	nude->hitArea.or.y   = 0;
	nude->hitArea.ex.x   = 127;
	nude->hitArea.ex.y   = 55;

	nude->subViews = [IdList new];

	return( nude );
}	/* + new */


- free 
{

	[self doHide];
	return [super free];
}	/* - free */


/*************************/
/*  RastPort OWNERSHIP:  */

//  'self' wants to render something, so it asks for a rastport. Search
//   for a valid rastport to render to, and return it to caller.
- (struct RastPort *) useRP
{

	if( wholeRastPort == NULL )
	{
		wholeRastPort = [superView useRP];
		view_flags |= VF_ParentsRP;
	}

	if( wholeRastPort == NULL )
	{
		printf( "%s: render RP == NULL!\n", [self name] );
		exit( 10 );
	}

	view_flags |= VF_HasRP;
	return wholeRastPort;
}	/* - useRP */


//  Accept usage of another rastport.  Release ours if owner already.
- setRP: (struct RastPort *) rp
{

	if( wholeRastPort != NULL )
		[self releaseRP: wholeRastPort];

	wholeRastPort = rp;
	view_flags |= VF_OwnsRP;
	return self ;
}	// - setRP:


//  'self' is done rendering.  Signal this fact. If rp == NULL, drop all!
//    We still owe the RP (ie, the cache points to us).
- dropRP: (struct RastPort *) rp
{

	if( !(view_flags & VF_OwnsRP) )
	{
		view_flags &= ~VF_HasRP;
	}
	return self;
}	/* dropRP: */


//   Somebody wants us out. We can no longer use this RastPort.
//   If rp == NULL, drop all!
- releaseRP: (struct RastPort *) rp
{

	if( view_flags & VF_HasRP )
	{
		[subViews eachElementPerform: @selector(releaseRP:) with: rp ];
		[self dropRP: rp];
		/*  .. should find RP capability and remove it .. */
	}
	if( view_flags & VF_ParentsRP )
	{
		[superView dropRP: rp];
	}

	view_flags &= ~(VF_ParentsRP | VF_OwnsRP | VF_HasRP);
	wholeRastPort = NULL;
	return self;
}	/* - releaseRP: */


/*****************/
/*  ADJUSTMENT:  */
- setApperance: (int) appear 
{

	switch( appear )
	{
		case VA_Close :
					return [self doClose];

		case VA_Hide :
					return [self doHide];
		case VA_Reveal :
					return [self doReveal];

		case VA_Mini :
					return [self doSizeMini];
		case VA_Normal :
					return [self doSizeNormal];
		case VA_Max :
					return [self doSizeMax];

		default :
					printf( "Bad appearance code%d\n", appear );
	}

	return self ;
}	/* - setHide */


/* Shutdown and free */
- doClose 
{
	return [self free];
}	/* - doClose */


/* Hide window from all screens */
- doHide
{ 
	view_flags |= VF_Hidden;
	[self    eachDelegatePerform: @selector(doHide) with: nil ];
	[subViews eachElementPerform: @selector(doHide) with: nil ];

	/*  Once the class has set stored any important values, it must
	 *	call:  [self releaseRP: wholeRastPort]
	 *	when done rendering into exposed view.
	 */

	return self;
}	/* - doHide */


/* Restore window to its last screen and position */
- doReveal
{
	view_flags &= ~VF_Hidden;
	[self    eachDelegatePerform: @selector(doReveal) with: nil ];
	[subViews eachElementPerform: @selector(doReveal) with: nil ];
	return self;
}	/* doReveal */


/* Shrink to minumum size */
- doSizeMini 
{
	view_flags &= ~(VF_Maximized );
	view_flags |=   VF_Minimized;
	return self;
}	/* - doSizeMini */


/* Size to a nominal size */
- doSizeNormal 
{
	view_flags &= ~(VF_Maximized | VF_Minimized );
	return self;
}	/* - doSizeNormal */


/* Expand size to screen max */
- doSizeMax 
{
	view_flags &= ~(VF_Minimized );
	view_flags |=   VF_Maximized;
	return self ;
}	/* - doSizeMax */


- setSize: (register RECT *) prect 
{

	/*  Only replace those coordinates that change: */
	if( prect->or.x != NOCHANGE )
		hitArea.or.x = prect->or.x;
	if( prect->or.y != NOCHANGE )
		hitArea.or.y  = prect->or.y;

	if( prect->ex.x != NOCHANGE )
		hitArea.ex.x    = prect->ex.x;
	if( prect->ex.y != NOCHANGE )
		hitArea.ex.y   = prect->ex.y;

	return( self );
}	/* - setSize */


- getSize: (RECT *) prect
{
	*prect = hitArea;   		/* Give caller a copy of rectangle */
	return self;
}	/* - getSize: */


/***************/
/*  CONTROLS:  */
- findControlType: (int) mask 
{
	return nil;
}	/* - findControlType */



/****************/
/*  RENDERING:  */
- drawSelf 		/* Draw whole view */
{

printf( "[%s drawSelf]\n", [self name] );
	[self drawBorder];
	return [self drawContent];
}	/* - drawSelf */


- drawContent 		/* Draw interior contents */
{

	[subViews eachElementPerform: @selector(drawSelf) with:nil ];
	return self;
}	/* - drawContent */


- drawBorder 		/* Draw controls around content */
{

	return self;
}	/* - drawBorder */


/******************/
/*  SUB-PORTIONS  */

- addSubView: sub_view
{

	[subViews addFirst:sub_view];
	if( !(view_flags & VF_Parenting) )
	{
		view_flags |= VF_Parenting;
		[sub_view setSuperView:self];   	/* Cross-link */
	}
	view_flags &= ~VF_Parenting;

	return( self );
}	/* - addSubView */


- setSuperView: parent
{

#if SW_ASSERT
	if( superView != nil )
	{
		printf( "%s: can't belong to two parent views!\n", [self name] );
		exit( 10 );
	}
#endif

	superView = parent;       	/* We're gunna use */
	if( !(view_flags & VF_Parenting) )
	{
		view_flags |= VF_Parenting;
		[parent addSubView: self];	/* Keep in touch   */
	}
	view_flags &= ~VF_Parenting;

	return( self );
}	/* - setSuperView: */



/**************/
/*  MESSAGES  */

- (MSGCTX *) command: (MSGCTX *) mc
{
	int 	size, i, class = mc->TClass;
	id  	node;

	if( activeCtrl != nil )   		/* Object currently taking events */
		return( [activeCtrl command: mc ] );

	if( ((class & [self getCategory]) == 0) ||
		((class & CF_QHASMXY) && ! pointInRect( mc->TSpot, & hitArea )) )
	{
		return( mc );		/* we're not interested, or it's outside of us */
	}

	/*  First, the things inside of us: */
	for( [subViews toFirst] ; node = [subViews next] ; )
	{
		if( [node command: mc] == NULL )
			return( NULL );
	}

	/*  Next, the things that are us: */
	for( [self->delegates toFirst] ; node = [self->delegates next] ; )
	{
		if( [node command: mc] == NULL )
			return( NULL );
	}

	if( mc->TClass == CF_VIEW )
	{
		if( mc->TOper == 0 )
			;
	}

	return( mc );   	/* Message not used up */
}	/* - command */

@end


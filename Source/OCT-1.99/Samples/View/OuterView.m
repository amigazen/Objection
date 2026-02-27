/*   OUTERVIEW.M   Talk with underlyng Windowing System     April 90, bjw */


#include "OuterView.h"
#include <objc/objc-class.h>
#include "pick.h"


@implementation OuterView : View ;
/*
	APTR 	view_wndw; 			-- Lower Layer Abstraction Handle 
	struct NewWindow	*wndw_spec;
*/


/* ------------------------------------------------------------------- */

struct Window	*last_wndw;


	void
free_windows()
{

	if( last_wndw != NULL )
		CloseWindow( last_wndw );

}	/* free_windows */


/**************/
/*  STORAGE:  */

+ initialize
{

	/*  ONLY OuterView's may pass, others continue the search: */
	if( self != &instOuterView )
		return [instOuterView.super_class initialize] ;

	if( !CLS_GETINFO(self, CLS_INITIALIZED) )
	{
		CLS_SETINFO( self, CLS_INITIALIZED );

		[Responder initialize];         /* Need libraries open first */
		_oc_atExit( free_windows ); 	/* Catch errors */
	}
	return( (id) self );
}	/* + initialize */


/*****************/
/*  ADJUSTMENT:  */

- intoScreen: (struct Screen *) scrn 
{

	wndw_spec->Screen = scrn;
	if( scrn == NULL )
		wndw_spec->Type = WBENCHSCREEN;
	else
		wndw_spec->Type = CUSTOMSCREEN;

	return self;
}	/* - intoScreen */


- setViewSpec: (register struct NewWindow *) new_window
{
	RECT	place;

	wndw_spec = new_window;
	place.or.x = new_window->LeftEdge;
	place.or.y = new_window->TopEdge;
	place.ex.x = new_window->Width;
	place.ex.y = new_window->Height;
	[self setSize: &place];
	[self setTitleSTR: new_window->Title];

	return [self intoScreen: new_window->Screen];
}	/* - setViewSpec */


- setTitleSTR: (STR) stringTitle
{

	[super setTitleSTR: stringTitle];
	if( view_wndw != NULL )
		SetWindowTitles( view_wndw, stringTitle, -1L );

	return( self );
}	/* - setTitleSTR: */


- setSize: (register RECT *) prect 
{

	[super setSize: prect];
	wndw_spec->TopEdge  = self->hitArea.or.x;
	wndw_spec->LeftEdge = self->hitArea.or.y;
	wndw_spec->Width    = self->hitArea.ex.x;
	wndw_spec->Height   = self->hitArea.ex.y;

	if( !(self->view_flags & VF_Hidden) )
	{
		struct Window	*w = (struct Window *) view_wndw;
		short 	dx, dy;

		dx = wndw_spec->TopEdge - w->TopEdge;
		dy = wndw_spec->LeftEdge - w->LeftEdge;
		if( dx != 0 || dy != 0 )
			MoveWindow( w, (long) dx, (long) dy );

		dx = wndw_spec->Width - w->Width;
		dy = wndw_spec->Height - w->Height;
		if( dx != 0 || dy != 0 )
			SizeWindow( w, (long) dx, (long) dy );

	}	/* if view visible */

	return self;
}	/* - setSize */



- doHide 
{

	if( view_wndw != NULL )
	{
		ClearMenuStrip( view_wndw );
		CloseWindow( view_wndw );

		[self releaseRP:NULL];   /* whatever the rastport, free it! */
		view_wndw = NULL;
		last_wndw = NULL;
	}

	return [super doHide];
}	/* doHide */


- doReveal
{
	extern void 	*OpenWindow();

	if( view_wndw == NULL )
	{
		/*  perfrom open.. */
		wndw_spec->Title = (UBYTE *) [self getTitle];
		view_wndw = OpenWindow( wndw_spec );
		last_wndw = view_wndw;
		[self setRP: view_wndw->RPort ];       /* This object owns it! */
	}

	[super doReveal];
	return [self drawSelf];
}	/* - doReveal */



- drawBorder 		/* Draw controls around content */
{

	RefreshWindowFrame( view_wndw );
	return self;
}	/* - drawBorder */


@end


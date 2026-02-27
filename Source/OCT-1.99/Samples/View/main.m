/*   MAIN.M   Test Driver for R.E.M. system                March 90, bjw */

/*
//
*/

#include "rem.h"
#include "OuterView.h"
#include "Pick.h"
#include "ScrollBar.h"
@includes


struct NewWindow	nw =
{
	44, 25,  450, 110,
	2, 1,
	0L,	/* IDCMP */
	WINDOWDRAG | WINDOWDEPTH |	/* Flags */
		SMART_REFRESH | NOCAREREFRESH,
	(struct Gadget *) NULL,
	(struct Image *)  NULL,
	(UBYTE *) "Test View",
	(struct Screen *) NULL,
	(struct BitMap *) NULL,
	90, 40, -1, -1,
	WBENCHSCREEN
} ;


id  	spec_maker;
id  	view;

extern short  	from_sdb;


/* -------------------------  Code Code Code  --------------------------- */

	void
short_delay()
{
	if( ! from_sdb )
		Delay( 2*50L );
}


	int
main( argc, argv )
	int 	argc;
	char 	*argv[];
{
	static RECT 	rect = { NOCHANGE, NOCHANGE,  200, 80 } ;
	RECT    ourSize;
	id  	img, sb;

	/*  1.  Setup  */
	spec_maker = [REM new];
	[spec_maker addDir: "LIB:" ];

	/*  2.  Test REM class:  */
	[spec_maker findName: "Controller" rid: 12];

	img = [Pick newID: ID_miniApp];

	if( img != nil && argc == 1 )
	{
		/*  3.  Test View class:  */
		view = [[OuterView new ]  setViewSpec: &nw ];
		[view setTitleSTR: "Short" ];
		[view doReveal ];  short_delay();

		ourSize.or.x = 5;
		ourSize.or.y = 22;
		ourSize.ex.x = NOCHANGE;
		ourSize.ex.y = NOCHANGE;
		[img setSize: &ourSize];
		[img setSuperView: view];
		[img drawSelf]; 	short_delay();

		sb = [ScrollBar newAligned: (int)SB_UP_DOWN ];
		[sb setSuperView: view];
		[sb show];
		[img getSize:&ourSize];
		[sb setSize: &ourSize ];
		[sb drawSelf];   	short_delay();

		[view setSize: &rect ];  short_delay();
		[view doSizeMax];	 short_delay();

		/*  x.  Cleanup */
		[view free];
	}

	[spec_maker free];
	return 0;
}	/* main */


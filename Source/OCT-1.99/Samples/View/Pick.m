/*   Pick.M    Pick-ture Elements                         April 90, bjw */

/*
//		These pick-tures are rather stupid.  They don't do much.  However,
//		their magic is `where do they come from?'  Secret is:  they're
//		stored in a seperate, "chip only" file that is loaded in at
//		'initialize' time.
//
//		03-April-90	bjw 	Initial recovery.
//		9-spet-90	bjw 	interface: userRP, setSize: .
*/


#include "pick.h"
#include <objc/objc-class.h>
#include <exec/resident.h>
#include <stdlib.h>
#include <functions.h>


#define  LOCAL

/*  Convert a clunky BCPL BPTR into a typical C pointer: */
#undef  BADDR
#define  BADDR(bptr)	((void *) ((ULONG)(bptr)<<2))


struct memdescrip
{
	void 	*data;
	WORD	length;
} ;


/* -----------------------  Class Description  -------------------------- */


@implementation Pick ;

LOCAL	void 		*image_seg;
LOCAL 	struct memdescrip	*mem_table;

static char 	img_fname[ ] = {  "BMFile"  };


/*   load_img_seg  --  When the application starts up, we wanna get some.
*/
	LOCAL void
load_img_seg()
{
	register struct Resident 	*match;
	register UWORD	*mem;		/* We search at every 'UWORD' boundry */
	ULONG  	*seg;
	int 	cnt;

	if( (image_seg = LoadSeg(img_fname)) == NULL )
	{
		printf( "Can't load images' file <%s>!\n", img_fname );
		exit( 10 );
		/*NOTREACHED*/
	}

	/*  Examine each segment that was loaded for our resident list.
	//	Since we're linking a "C" file, it loads in with code, data,
	//	and bss segments:
	*/
	for( seg = BADDR(image_seg) ; seg != NULL ; seg = BADDR(seg[0]) )
	{
		mem = (UWORD *) & seg[1];	/* Skip 'next' BPTR */

		/*  The count is in bytes.  We search a few */
		for( cnt=((int)seg[-1])/3 ; --cnt > 0 ; )
		{
			match = (struct Resident *) mem;
			if( match->rt_MatchWord == RTC_MATCHWORD &&
				match->rt_MatchTag == match )
			{
				/* Yes:  Found Resident strucyture in our data file: */
				printf( "[Pick: version %d]\n", match->rt_Version );
				mem_table = (struct memdescrip *) match->rt_Init;

				return ;
			}
		}	/* for enough attempts */
	}	/* for each segment */

	printf( "Can't find images in <%s>!\n", img_fname );
	exit( 10 );
	/*NOTREACHED*/
}	/* load_img_seg */


/*   free_img_seg  --  When the application stops, we wanna free stuff.
*/
	void
free_img_seg()
{

	if( image_seg != NULL )
	{
		UnLoadSeg( image_seg );
		image_seg = NULL;
	}

}	/* free_img_seg */


/* ------------------------  Method Dictionary  ----------------------- */


/***************/
/*  CREATION:  */

+ initialize
{

	/*  ONLY Pick may pass, others continue the search: */
	if( self != &instPick )
		return [instPick.super_class initialize] ;

	if( !CLS_GETINFO(self, CLS_INITIALIZED) )
	{
		CLS_SETINFO( self, CLS_INITIALIZED );

		_oc_atExit( free_img_seg ); 	/* Catch errors */
		load_img_seg();
	}
	return( (id) self );
}	/* + initialize */


//  Search table for image.  Return new object if found.
+ newID:(enum IconId) id_code
{
	id 	nude = nil;
	RECT    itSize;

	if( (unsigned) id_code <= mem_table[0].length )
	{
	 	nude = [self new];
		nude->pick.pickType = PICK_IMG;
		nude->pick.partCode = 0;
		nude->pick.pu.img = * ((struct Image *)mem_table[id_code].data) ;

		[nude getImageSize: &itSize];	/* private method */
		[nude setSize: &itSize];
	}

	return( nude );
}	/* + newID: */


+ newType:(int) type_code
{

	return( nil );
}	/* + newType */


//   Assign a user code to the pick element.
- setCode: (int) user_key
{

	pick.partCode = user_key;
	return( self );
}	/* - setCode: */


/**************************/
/*  DISPLAY INTERFACING:  */


- getImageSize: (RECT *) pRect
{

	pRect->or.x = 0;
	pRect->or.y = 0;
	pRect->ex.x = 0;
	pRect->ex.y = 0;

	switch( pick.pickType )
	{
		case PICK_IMG :
					pRect->ex.x = pick.pu.img.Width;
					pRect->ex.y = pick.pu.img.Height;
					break ;

	}

}	/* - getImageSize: */


- setSize: (RECT *) pRect
{

	switch( pick.pickType )
	{
		case PICK_IMG :
					pick.pu.img.LeftEdge = pRect->or.x;
					pick.pu.img.TopEdge  = pRect->or.y;
					break ;

		default :
					break ;
	}

	return [super setSize: pRect] ;
}	/* - setSize: */


- drawSelf
{
	struct RastPort   *rp = [self useRP];

printf( "[pick drawself] " );
prect( [self name], & self->hitArea );

	switch( pick.pickType )
	{
		case PICK_TXT :
					PrintIText( rp, & pick.pu.txt, 0L, 0L );
					break ;

		case PICK_IMG :
					DrawImage( rp, & pick.pu.img, 0L, 0L );
					break ;

		case PICK_BOR :
					DrawBorder( rp, & pick.pu.bor, 0L, 0L );
					break ;

		default :
					[self error:"can't render type %d yet", pick.pickType ];
	}

	[self dropRP:rp];
	return( self );
}	/* - drawSelf */


/* -----------------------  Box Creation  --------------------------- */


@end


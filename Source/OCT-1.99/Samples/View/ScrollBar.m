/*   ScrollBar.M   Just the ScrollBar fixture            April 90, bjw */

/*
//		This class provides just the scrollbar fixture.  You interpret
//		the reactions of this object.
//
//		03-april-90	bjw 	Initial typing (and still my taxes are undone).
//		9-spet-90	bjw 	Now actually draws interior.
*/

#include "ScrollBar.h"
#include "Pick.h"

@implementation  ScrollBar : View
/*
	id  	less, more;  		-- The touch-spots
	short 	sbar_flags;
*/


/* --------------------  Method Dictionary  --------------------------- */


+ newAligned: (int) direction
{
	id  	nude = [self new];

	nude->sbar_flags = direction;
	if( direction == SB_UP_DOWN )
	{
		nude->less = [Pick newID: ID_upArrow];
		nude->more = [Pick newID: ID_downArrow];
	} else
	{
		nude->less = [Pick newID: ID_leftArrow];
		nude->more = [Pick newID: ID_rightArrow];
	}

	[nude addSubView: nude->less ];
	[nude addSubView: nude->more ];
	return( nude );
}	/* + newAligned: */


- setSuperView: parent
{
	RECT 	parentSize;

	[parent getSize: &parentSize];
	[self setSize: &parentSize];	/* Co-ordinate sizes */
	return( [super setSuperView: parent] );
}	/* - setSuperView */


/*   setSize:  --  Position box and LESS/MORE Pick's  */
- setSize: (RECT *) pRect 
{
	RECT    mysize;
	RECT 	edge, corner;

	[less getSize: &corner];    	/* Get Pick's extent size */
	mysize = corner;
	mysize.or.x = pRect->or.x;
	mysize.or.y = pRect->or.y;
	if( sbar_flags == SB_UP_DOWN )
		mysize.ex.y == pRect->ex.y;
	else
		mysize.ex.x == pRect->ex.y;

	[super setSize: &mysize];  	/* Broadcast coordinates */
	[super getSize: &mysize];	/* Normalize */
prect( "scrollBar - setSize", &mysize );

	/*  Now position the LESS/MORE Picks' at correct offsets: */
	edge = mysize;
	if( sbar_flags == SB_UP_DOWN )
	{
		/*  LESS:  */
		edge.or.x += 2;
		edge.or.y += edge.ex.y - corner.ex.y - 2;
		[less setSize: &edge ];

		/*  MORE:  */
		edge.or.y -= (corner.ex.y + 2);
	} else
	{
		/*  LESS:  */
		edge.or.x += edge.ex.x - corner.ex.x - 2;
		edge.or.y += 2;
		[less setSize: &edge ];

		/*  MORE:  */
		edge.or.x -= (corner.ex.x + 2);
	}
	[more setSize: &edge ];

	return( self );
}	/* - setSize */


- drawBorder
{
	struct RastPort 	*rp = [self useRP];
	RECT	sizes;

	[self getSize: &sizes];

	/*  draw an enclosing box.. */
	A_Move( rp, sizes.or.x, sizes.or.y );
	A_Draw( rp, sizes.or.x, sizes.or.y+sizes.ex.y );
	A_Draw( rp, sizes.or.x+sizes.ex.x, sizes.or.y+sizes.ex.y );
	A_Draw( rp, sizes.or.x+sizes.ex.x, sizes.or.y );
	A_Draw( rp, sizes.or.x, sizes.or.y );

	[self dropRP:rp];
	return self;
}


- drawContent
{
	struct RastPort 	*rp = [self useRP];

	[less drawSelf];
	[more drawSelf];

	[self dropRP:rp];
	return( self );
}	/* - drawSelf */


@end


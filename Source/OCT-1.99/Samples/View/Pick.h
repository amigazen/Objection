/*   PICK.H    Pick Header File  (Public)                  Nov 89, bjw */

/*
//		This file contains declarations for micro-pictures, 'pick'.
//		This pieces include sprites, borders, images and intuitext.
//
//		9-feb-90 	bjw 	Moved from icon.h to pick.h
//		27-mar-90	bjw 	Added recursive pick list.
//   	14-nov-90 	bjw 	Recovered from backup.
*/


#include "view.h"
#include <graphics/gfxmacros.h>
#include <graphics/sprite.h>


/* ----------------------  RE:  CONTROLS  ------------------------------ */



/*  Id's available from RNAME_PICK: */
enum IconId  {
		ID_leftArrow = 1,
		ID_rightArrow = 2,
		ID_upArrow = 3,
		ID_downArrow = 4,
		ID_radio = 5,			/* Radio button */
		ID_radioH = 6,
		ID_check = 7,			/* Check box */
		ID_checkH = 8,

		ID_miniApp = 9,  		/* Miniaturized application */
} ;



/*  When requesting rendering, keep use this list header:  */
struct PickList  {
		struct PickList  *next;   		/* A List of:  RE_PICK  */
		struct PickList  *prev;

		short 		leftEdge;
		short 		topEdge;
} ;


/*  Format of RE stored 'Pick' structure: */
typedef struct  {
		UBYTE 			pickType;		/* PICK_xxx */
		UBYTE   		partCode;		/* Part code (for lower driver) */

		union {
			struct IntuiText	txt;
			struct Image 		img;
			struct Border 		bor;
			struct SimpleSprite  spr;
			struct PickList 	pck;
		}  pu;
}  RE_PICK ;

/*  Type of pick that is this node: */
#define  PICK_TXT 	0x01
#define  PICK_IMG	0x02
#define  PICK_BOR 	0x03
#define  PICK_SPR	0x04
#define  PICK_PICK	0x05


/*  Pick codes for items on PickList.  0 <= n <= 7. (Octal) */
#define  PART_MASK   	(0xF8)
#define  PART_BOX(n)	(0010|(n))
#define  PART_LESS(n) 	(0020|(n))
#define  PART_LLESS(n)	(0030|(n))  	/* Mega Less, ie page up */
#define  PART_MORE(n)	(0040|(n))
#define  PART_MMORE(n)	(0050|(n))  	/* Mega More, ie page down */
#define  PART_BOOL(n)  	(0060|(n))
#define  PART_KNOB(n)	(0070|(n))
#define  PART_TEXT(n)	(0100|(n))
/*   Last --->          (0370|(n))    */


/* ------------------------  Public Interface  -------------------------- */

@interface Pick : View
{
	RE_PICK 	pick;
}

/* -------------------  Public  Methods  ------------------- */

+ initialize ;
+ newID:(enum IconId) id_code ;
+ newType:(int) type_code ;
- setCode: (int) user_key ;
- getImageSize: (RECT *) pRect ;
- setSize: (RECT *) pRect ;
- drawSelf ;

@end


/*  end of PICK.H */


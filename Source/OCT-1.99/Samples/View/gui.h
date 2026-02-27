/*   GUI.H   Graphical User Interface Header               March 90, bjw */

#ifndef GUI_H
#define GUI_H	1

#include <exec/ports.h>

/*  Mark a subroutine as private to one source file: */
#define  LOCAL


/*****************************/
/*  Useful Graphics Starts.. */

typedef struct { SHORT	x, y; } 	VECT;
typedef struct { VECT or, ex; } 	RECT;

#define  NOCHANGE	0x8001


/*******************************/
/*  Method Message Transport.. */

typedef struct MessageContext  {
		struct Message	ExecMsg;
		USHORT		TClass;
		USHORT		TOper;
		USHORT		TCode;
		USHORT  	TQualifier; 	/* From InputEvent */
		APTR    	AttenObject;
		VECT    	TSpot;   		/* Hot Spot */
		struct View  *TView;
		APTR		TOverhead;
}  MSGCTX ;


/*  TClass field is one of these: */
#define  CF_VERIFY  	0x0001
#define  CF_SYSTEM  	0x0002
#define  CF_INPUTS  	0x0004
#define  CF_VIEW    	0x0008

#define  CF_PRIVATE 	0x1000
#define  CF_REACTION	0x2000

 /*  Those categories that provide Mouse XY: */
#define  CF_QHASMXY 	(CF_VERIFY | CF_INPUTS)


/* --------------------------  Short Circuits  ------------------------- */

struct collTable	{	short	len, data[1]; } ;
struct VSprite  	{	short	len, data[1]; } ;
struct Region   	{ 	short	len, data[1]; } ;
struct LayerInfo_extra   	{ 	short	len, data[1]; } ;


/* ------------------------  Service Functions  ------------------------ */

/*  Public, General Routines  */
extern void 	*memdup PARMS(( CONST char *, CONST length ));
extern void     sdb_pause PARMS(( void ));
extern void     prect PARMS(( struct Rectange * ));

extern BOOL 	pointInRect PARMS(( CONST VECT, CONST RECT * ));


#endif

/*  end of GUI.H */


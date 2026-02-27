//   Test file submitted by Stu Ferguson                     Aug 89, shf
//   (Even though he does not know about this file, he created it)


/* ------------------------------------------------------------------ */
/*		T5.H  --  Class Interface definition file             */

#import <objc/Array.h>

struct Foot {
	int	i1, i2;
};
typedef struct Foot	FOOT;

typedef struct ScrollBar
{
	USHORT	here;
	USHORT	extreme;
}  SBAR;


@interface  MyClass : Array
{
	struct Foot	*myFoot;
	FOOT		boot;
	int		myThing;
	short   lastSec, lastMicro;
	struct ScrollBar	sbar;
}

- (void *) at:(int) offset put:obj ;
- zimbabwe: (struct Foot *) feet ;
- ziare: (FOOT *) boot ;
- (int) blort: (int) num ;
- (void *) asIntuiMenus ;
- menuPick ;

@end

/* ------------------------------------------------------------------- */

#import <objc/PointXY.h>
#import <objc/ByteArray.h>

@implementation MyClass


#define  MENUNULL  0x0000
#define  MENUPICK  0x0400

struct PointXY 	{ char	*spot; } ;

struct IMsg {
    ULONG   Class;
	USHORT  Code;
	short   Seconds, Micros;
} ;


struct Menu  {
    char	*bongodude;
}  ;


struct SuperMenuItem {
    id  myOwner;
    struct { USHORT NextSelect; } mitem;
} ;

extern void  *AllocMem();
extern void  *ItemAddress();


/* ------------------------------------------------------------------- */

- dispatchMenuAction: (struct IMsg *) imess
{
	USHORT			mnum;
	struct Menu		*m;
	struct SuperMenuItem	*smi;

	if( DoubleClick (lastSec, lastMicro, imess->Seconds, imess->Micros) ) {
		m = NULL;
	}

	if (imess->Class != MENUPICK) return self;
	m = (struct Menu *) [self asIntuiMenus];

	mnum = imess->Code;
	while (mnum != MENUNULL) {
		smi = (struct SuperMenuItem *) ItemAddress (m, (LONG)mnum);
		[smi->myOwner menuPick];
		mnum = smi->mitem.NextSelect;
	}
}   // - dispatchMenuAction:


- zimbabwe: (struct Foot *) feet
{
	struct SuperMenuItem	*uu;
	void 	*smi;
	int 	n;

	/* Next line has super paren truobles: */
if (!(uu = (struct SuperMenuItem*)AllocMem((long)((n)*sizeof(struct SuperMenuItem)),0L)))
        ;

	/*  Next line will tickle lex_text()  */
	[[PointXY new] x:[ByteArray new:" x "] y:[ByteArray new:" y "] ];
	[self x:[ByteArray new:" x "] y:[ByteArray new:" y "] ];

	if (feet)  [self blort:1];
	else		[self show] ;
	return self;
}

- ziare: (FOOT *) boot
{
	struct {  @defs(MyClass) } xxx;

	if( * [self name] == 'M' ) {		/* " * (char *) " */
		(8 * [self hash] );
		[self name][0];			/* Reference first letter of name */
	}
	return self;
}

- (int) blort: (int) num
{

	[self blort:[self hash] + num];  	// Does "+" remain ?
	return [self hash] ;
}


@end
// ----- end test.m -----


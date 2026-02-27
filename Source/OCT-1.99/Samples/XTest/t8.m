/*   t8.m    Test bitfields and array typedefs */


typedef   long    MAT3_2[8];

 typedef struct { char *_bp; } XFILE, *ZFILEPTR;
 typedef struct _file { char *_bp; } YFILE;
 typedef int  (*APFI)( /* char * */ );


#import  <objc/Object.h>

extern Class   List, ByteArray;

@interface AbstractWindow : Object
{
	/* insert ivars here */
	ULONG	cookIEinterest;	/* Cooked events of interest */
	id      children, wname;
	short	translation[4];	/* reference inside parent */

	struct {
	 unsigned  visible : 1;		/* are we mapped visibile? */
	 unsigned  mapped : 1;		/* are we mapped in ? */
	} flags;
}


/* -------------------  Public  Methods  ------------------- */

+ new ;
+ str:(STR) windowname ;
- onlyChild:onlychild ;

@end


@implementation AbstractWindow
/*  rawIEinterest, cookIEinterest, translation, flags;
*/


/* ---------------------------------------------------------------------- */

+ new
{
	id guy = [super new];

	guy->children = [List new];
	guy->wname    = [ByteArray str:"none"];
	return( guy );
}	// + new


+ str:(STR) windowname
{
	id	guy = [self new];
	[guy->wname str:windowname];
	return( guy );
}	// + str:


@end


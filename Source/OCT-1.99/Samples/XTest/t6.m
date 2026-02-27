//   T6.M    Test out longer type names                         Jan 91, bjw
extern int that;
/* #ident	"@(#)head:stdio.h	2.14" */
/*  The above line was found a 3B15 UNIX SysVR2 box.  */

#include <objc/objc.h>    /* Include OCT types! */


extern struct Node *roundup( );

typedef struct _file {
	long int	_cnt;
	unsigned char	*_ptr,
	                *_base;
	char	_flag;
	char	_file;
	unsigned long int	*_roof;
        struct _vect    *that;
} YOODLE;    /* Line 18  */

typedef struct _vect { short x, y; } VECT;
typedef unsigned long int      ULI;

extern unsigned char *_bufendtab[];

///////// define that base thing!   line #25
@interface   Object
{
	int    isDefined;  char	name[10];
}

+ new ;
- getX: xpoint Y:ypoint ;
- at:(int) offset put:anObject ;

@end

@interface AbstractCPU : Object

- loadProgramFP: (void *) datefile ;
@end


@implementation Object


+ new
{

    return nil;
}

- at:(int) offset put:anObject
{

   extern char   _ctab[];

   isDefined = (_ctab-1)[name[0]];

   return self;
}

@end


extern unsigned char *_bufendtab[];


extern int    atoi();
int   j, progCnt = 0;
id    proc = nil;


main( argc, argv )
   int argc;
   char *argv[];
{
   YOODLE   * mine = (YOODLE *) argv;

   for (j=atoi(&argv[1][1]) ; --j > 0 ; ++progCnt )
       [proc loadProgramFP: mine] ;

   if( argc == 2 && argv[1][0] == '=' )
      printf( "hi" );
   exit(0);
}


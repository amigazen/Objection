//   Test File #1   for O.O.C.                            July 89, bjw



typedef struct {
#if vax || u3b || M32 || u3b15 || u3b5 || u3b2
	int	_cnt;
	unsigned char	*_ptr;
#else
	unsigned char	*_ptr;
	int	_cnt;
#endif
	unsigned char	*_base;
	char	_flag;
	char	_file;
} XFILE;

extern unsigned char *_bufendtab[];

#import <objc/Object.h>
#import <objc/ByteArray.h>


@protocol XXX
- add: obj ;
@end


@interface   Node : ByteArray
{
	int    isDefined;  char	name[10];
}

+ new ;
- getX: xpoint Y:ypoint ;
- at:(int) offset put:anObject ;

@end

@interface   Node (doobie)

- boundsViolation: (int) anOffset ;

@end

extern char 	*yytext;

@implementation Node (doobie)

+ new
{
	id	sam;

	sam=[Object new:5];
	[sam at:2 put:[Object new] ];
	[[[sam class] str:"Hello world"] hash] + 7;
	return sam;
}


extern struct { id	b;	}	*a;

- at:(int) offset put:anObject
{
	int 	cap = self->capacity;	/* Ref ivar of superclass */

	name[0] = (char) 0;
	[a->b add:anObject];
	return[self boundsViolation:offset];
}


//	Not logical, Sir.  Your index is out of range:
- boundsViolation: (int) anOffset
{
	return( [self error: isDefined > 0
				? "bounds violation: %d outside range [0..%d]"
				: "zero capacity array", anOffset, isDefined-1] );
}



/*		T5.H  --  Class Interface definition file             */

#include <objc/Array.h>

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
	struct ScrollBar	sbar;
}

- (void *) at:(int) offset put:obj ;
- zimbabwe: (struct Foot *) feet ;
- ziare: (FOOT *) boot ;
- (int) blort: (int) num ;

@end


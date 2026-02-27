//   NewByte.m class    Demonstration class                  Aug 89, bjw


#include <apps/NewByte.h>

@implementation  NewByte : ByteArray ;


/* --------------------------  The Details  -------------------------- */

	static void
str_reverse( str )
	char	*str;
{
	register char	*f, *b;
	char	t;

	f = str;
	b = & str[strlen(str) - 1];
	while( f < b )
	{
		t  = *b;
		*b = *f;
		*f = t;

		++f;
		--b;
	}
}	/* str_reverse */


- reverse
{
	str_reverse( IV(self) );
	return self;
}	/* reverse */

@end


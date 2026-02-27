/*  ELIST.M                                         Stu F. Nov 25, 1989 */
/*	27-mar-90	bjw 	Added remTail method
*/

#include <octhead.h>
#include <objc/EList.h>
#include <objc/ENode.h>

@implementation EList : Object ;


struct SuperClass {
	@defs (Object)
};


/*  Our node is 'nxt' and 'prv': */
#define  nxt  mln_Succ
#define  prv  mln_Pred


#define PLST(l)		(&(l)->head)
#define BACK(p)		((id)(((char*)p)-sizeof(struct SuperClass)))


/* ----------------------  Implementation Code  ------------------------ */

+ new
{
	id	guy;

	guy = [super new];
	NewList (PLST (guy));
	return guy;
}

- free
{
	id	elt;

	while (elt = [self remHead]) {
		[elt free];
	}
	return [super free];
}

- (int) count
{
	id	elt;
	int	n = 0;

	for (elt = [self first]; elt; elt = [elt next])
		n++;
	return n;
}

- addHead: elt
{
	AddHead (PLST (self), PLST (elt));
	return self;
}

- addTail: elt
{
	AddTail (PLST (self), PLST (elt));
	return self;
}

- first
{
	if (!((struct MinNode *) head)->nxt)
		return nil;
	return BACK (head);
}

- last
{
	if (!((struct MinNode *) tail)->prv)
		return nil;
	return BACK (tail);
}

- remHead
{
	id	elt = [self first];

	if (elt) {
		[elt remove];
	}
	return elt;
}

- remTail
{
	id	elt = [self last];

	if (elt) {
		[elt remove];
	}
	return elt;
}


@end

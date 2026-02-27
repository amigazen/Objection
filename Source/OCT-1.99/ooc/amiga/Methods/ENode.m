/*    ENODE.M                                       Stu F.  Nov 25, 1989 */

#include <octhead.h>
#include <objc/ENode.h>


@implementation ENode 
/**  : Object { char  *next; char  *prev; }  **/


struct SuperClass {
	@defs (Object)
};


/*  Our node is 'nxt' and 'prv': */
#define  nxt  mln_Succ
#define  prv  mln_Pred


#define PNOD(le)	(&(le)->next)
#define BACK(p)		((id)(((char*)p)-sizeof(struct SuperClass)))


- next
{
	struct MinNode	*n;

	n = (struct MinNode *) next;
	if (!n->nxt)
		return nil;
	return BACK(n);
}

- previous
{
	struct MinNode	*n;

	n = (struct MinNode *) prev;
	if (!n->prv)
		return nil;
	return BACK(n);
}

- insertAfter: elt
{
	AddHead (PNOD(self), PNOD(elt));
	return self;
}

- remove
{
	Remove (PNOD (self));
	return self;
}

@end


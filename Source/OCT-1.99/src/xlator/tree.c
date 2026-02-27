/*	TREE.C    Tree handler routines                           Mar 89, bjw */
/*****************************************************************
 *   This program is copywrite (c) 1989, 1990 by Brian J. Witt.
 *                  rael%aleks@seer.UUCP
 *   The author retains all rights to this work, and may alter
 *   the restrictions/permissions at any time.  The author assume
 *   no responibility for usefulness or damage incurred arising
 *   from any use of this intellectual property.
 *   Permission is hereby granted for non-commerical distribution
 *   and usage.  A small fee ( < $15 US) may be charged to cover
 *   distribution costs.
 *
 ******************************************************************/

/*
//		The enumeration function handles deletion of the node it just
//		returned to its caller.
//
//		29-mar-89	bjw		Initial Coding.
//		08-apr-89	bjw		Added enumerating.
//		14-oct-89	bjw 	Added List handler for non-Amiga sites.
//   	14-dec-90 	bjw 	Uses <octhead.h> to list/node defines.
*/

#include <octhead.h>
#include <stdio.h>
#include <stdlib.h>


#include "tree.h"

#define  SW_DEBUG  0


/* ----------------------  Externally, Yours  ------------------------ */


/* --------------------  List and Node Support  ---------------------- */


/*  These functions are included for non-Amiga machines: */
#ifndef MCH_AMIGA

	void
NewList( list )
	register struct MinList 	*list;
{
	list->mlh_Head     = (struct MinNode *) & (list->mlh_Tail);
	list->mlh_Tail     = NULL;
	list->mlh_TailPred = (struct MinNode *) & (list->mlh_Head);
}   /* NewList */

	void
Remove( node )
	register struct Node 	*node;
{
	register struct Node	*n2;

	n2   = node->ln_Succ;		/* Get successor */
	node = node->ln_Pred;		/* Get predecessor */
	node->ln_Succ = n2;  		/* Fixup pred's succ ptr */
	n2->ln_Pred   = node;		/* Fixup succ's pred ptr */
}   /* Remove */


	void
AddHead( list, node )
	struct MinList 	 	*list;
	register struct Node 	*node;
{
	register struct Node 	*n2;

	n2 = (struct Node *) list->mlh_Head;
	list->mlh_Head = (struct MinNode *) node;	/* Connect others to us */
	n2->ln_Pred = node;

	node->ln_Succ = n2; 		/* Connect out to others */
	node->ln_Pred = (struct Node *) & (list->mlh_Head);
}   /* AddHead */


	void
AddTail( list, node )
	struct MinList 	*list;
	register struct Node 	*node;
{
	register struct Node 	*n2;

	n2 = (struct Node *) list->mlh_TailPred;
	list->mlh_TailPred = (struct MinNode *) node;	/* Connect others to us */
	n2->ln_Succ = node;

	node->ln_Pred = n2; 		/* Connect out to others */
	node->ln_Succ = (struct Node *) & (list->mlh_Tail);

}   /* AddTail */



	struct Node *
FindName( list, name )
	struct MinList 	*list;
	register char	*name;
{
	register struct Node 	*np;

	for( np=(struct Node *)list->mlh_Head ; np->ln_Succ != NULL ; np=np->ln_Succ )
	{
		if( strcmp( np->ln_Name, name ) == 0 )
			return( np ) ;
	}

	return( NULL );
}   /* FindName */
#endif


/* --------------------  Private Code Workings  ---------------------- */


	unsigned
hashTree( name )
	char	*name;
{
	unsigned	h;

#if SW_DEBUG
	printf( "for <%s>", name );
#endif
	for( h=strlen(name) ; *name ; ++name )
		h ^= (*name) * 13;

	h %= MX_BUCKETS;

#if SW_DEBUG
	printf( " hash id=$%x\n", h );
#endif
	return( h );
}


/* ----------------------  Public Code Works  ------------------------ */


	void
addTree( tp, node )
	TREE	*tp;
	struct Node  *node;
{
	AddTail( & tp->b[ hashTree(node->ln_Name) ], node );
}


	struct Node *
searchTree( tp, name )
	TREE	*tp;
	char	*name;
{
	return( FindName( & tp->b[ hashTree(name) ], name ) );
}


	TREE *
newTree( )
{
	register TREE	*tp;
	short	j;

	if( (tp=(TREE *)malloc(sizeof(TREE))) != NULL )
	{
		for( j=0 ; j < MX_BUCKETS ; ++j )
			NewList( & tp->b[j] );
	}
	return( tp );
}   /* newTree */


/*   enumTree  --  Sequentially access all nodes in the hash tree in
//				   uncertain order.  Works correctly even if the
//				   returned node is free'd by the caller.
*/
	struct Node *
enumTree( root, index )
	TREE	*root;
	int		*index;			/* Use -1 to init enumeration */
{
	static int	header = MX_BUCKETS;
	static struct Node	*next = NULL;
	struct Node 	*result;

	/*  If starting, then init our private state variables: */
	if( *index == -1 )
	{
		header = 0;
		next = (struct Node *) root->b[0].mlh_Head;
	}

	/*  If this bucket empty, move on to the next.. */
	if( next == NULL || next->ln_Succ == NULL )
	{
		do
		{
			/*  If already exhausted list, return NULL: */
			if( ++header >= MX_BUCKETS )
				return NULL;
			next = (struct Node *) root->b[ header ].mlh_Head;
		}
		while( next->ln_Succ == NULL ) ;;
	}

	/*  Count one more success and return node: */
	++*index;
	result = next;
	next   = next->ln_Succ;   	/* Next call starts searching here */
	return( result );
}   /* enumTree */



/*   TREE.H         Tree handling header                     Mar 89, bjw */

/*
//		This header file describes a hash tree.  The number of buckets
//		is a compile time option.  Code is designed to be fast and
//		useful, rather than true to hashing.  Ie, the number of buckets
//		is a power of 2 rather than a prime number.  Also, a MinList
//		is used instead of List to save two bytes.
//
//		15-mar-89 	bjw 	Initial coding.
//		15-jan-90	bjw 	Using MinList instead.
*/


#ifndef TREE_H
#define  TREE_H  1

/*  How many hash buckets we support: */
#define  MX_BUCKETS     8


/*  The hash tree uses chaining when a bucket overflows.  Chaining
//		is done on a doubly-linked list.  In the user code, they
//		can reference the typedef TREE, or use 'void *', since the 
//		structure should really be opauge to the user.
*/

typedef struct
{
	struct MinList     b[ MX_BUCKETS ] ;
} TREE;


/*  The public interface:  */
extern TREE     	*newTree( /* */ );
extern void     	addTree( /* void *root, struct Node *node */ );
extern struct Node	*searchTree( /* void *root, char *name */ );
extern struct Node	*enumTree( /* void *root, int *index */ );

#endif


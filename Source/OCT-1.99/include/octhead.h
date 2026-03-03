/*   OCTHEAD.H   Collect typedefs for entire OCT system      Dec 90, bjw */

/*
 *    This file contains typedefs and rudimentary structures used
 *    throught the Object C Translator suite of programs and
 *    sometimes classes.  The defines are mostly system independent,
 *    but always check the typedef to ensure the 'int' sizes
 *    agree with your compiler model.
 *
 *    For the Amiga, we turn "AMIGA" define into MCH_AMIGA also.
 *
 *    If your processor requires variable length argument lists
 *    (or you're just feeling ANSI-C compliant), enable USE_STDARG below.
 *    It actives if __STDC__ is defined.  This environment then assumes
 *    vprintf() types are available from the standard library.
 * 
 *    This file depends on no other defines.
 *    This file includes no other header files.
 *
 *    14-dec-90     bjw      Initial coagulating.
 *    20-april-91   Ramsey   Better use of defines.
 *    7-aug-91     db/bjw    Added USE_STDARG for those _other_ machines :-)
 */


/*  Determine if Var-Args are useful:  */
#ifdef __STDC__
# define  USE_STDARG   1
#endif

/*  For Amiga include compatibility  */
#ifdef EXEC_LISTS_H
# define  _OCTHEAD_H   1
#endif

#ifndef _OCTHEAD_H
# define _OCTHEAD_H  1

# ifndef MCH_AMIGA
#  ifdef AMIGA
#   define  MCH_AMIGA  1
#  endif
# endif

/* Some vendor headers (or cross-toolchain headers) may use GCC-style
 * __attribute__((...)) even when compiling with a non-GCC compiler such
 * as SAS/C.  If the compiler doesn't recognize __attribute__, treat it
 * as an empty macro so those headers still parse.
 */
# ifndef __GNUC__
#  ifndef __attribute__
#   define __attribute__(x)
#  endif
# endif

# define EXEC_NODES_H  1
# define EXEC_LISTS_H  1
# define EXEC_TYPES_H  1


/* ---------------------------------------------------------------------- */

/* vvvvvvvvvvvvvvvvvvv  User Servicable Parts Follow  vvvvvvvvvvvvvvvvvv */



# define VOID void

typedef  unsigned long   ULONG;
typedef  unsigned short  UWORD;
typedef  unsigned short  USHORT;
typedef  unsigned char   UCHAR;
typedef  unsigned char   UBYTE;
typedef           long   LONG;
typedef           short  WORD;
typedef           short  SHORT;
typedef           char   BYTE;
typedef           short  BOOL;
typedef  unsigned char  *STRPTR;
typedef         STRPTR  *APTR;   /* Generic pointer pointer thing */

typedef  int 	(*PFI)( );   	/* Ptr to function return 'int' */

# define BYTEMASK  0x0FFL

/* ^^^^^^^^^^^^^^^^  No User Servicable Parts Beyound  ^^^^^^^^^^^^^^^^^ */


#ifndef TRUE
# define TRUE   1
# define FALSE  0
#endif


#ifndef USE_STDARG
# define  USE_STDARG  0
#endif


/* ---------------------------------------------------------------------- */


struct Node {
	struct Node *ln_Succ;
	struct Node *ln_Pred;
	UBYTE ln_Type;
	BYTE ln_Pri;
	char *ln_Name;
};

struct MinNode {
	struct MinNode *mln_Succ;
	struct MinNode *mln_Pred;
};

struct List {
	struct Node *lh_Head;
	struct Node *lh_Tail;
	struct Node *lh_TailPred;
	UBYTE lh_Type;
	UBYTE l_pad;
};

struct MinList {
	struct MinNode *mlh_Head;
	struct MinNode *mlh_Tail;
	struct MinNode *mlh_TailPred;
};


#define NT_UNKNOWN 0L
#define NT_TASK 1L
#define NT_INTERRUPT 2L
#define NT_DEVICE 3L
#define NT_MSGPORT 4L
#define NT_MESSAGE 5L
#define NT_FREEMSG 6L
#define NT_REPLYMSG 7L
#define NT_RESOURCE 8L
#define NT_LIBRARY 9L
#define NT_MEMORY 10L
#define NT_SOFTINT 11L
#define NT_FONT 12L
#define NT_PROCESS 13L
#define NT_SEMAPHORE 14L
#define NT_SIGNALSEM 15L

/*  Node functions (Amiga: from exec; non-Amiga: from tree.c).  */
#ifdef MCH_AMIGA
extern void NewList(struct List *list);
extern void Remove(struct Node *node);
extern void AddHead(struct List *list, struct Node *node);
extern void AddTail(struct List *list, struct Node *node);
extern struct Node *RemTail(struct List *list);
extern struct Node *FindName(struct List *list, char *name);
#else
extern void NewList(struct MinList *list);
extern void Remove(struct Node *node);
extern void AddHead(struct MinList *list, struct Node *node);
extern void AddTail(struct MinList *list, struct Node *node);
extern struct Node *RemTail(struct MinList *list);
extern struct Node *FindName(struct MinList *list, char *name);
#endif
#define _OCT_HEAD_LIST_FUNCS 1
#endif

/* end octhead.h */


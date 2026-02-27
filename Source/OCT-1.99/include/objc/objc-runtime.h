/*  objc-runtime.h   Definitions of substrate routines      March 90, bjw */
/*****************************************************************
 *   This program is copyright (c) 1989, 1990 by Brian J. Witt.
 *                  netcom.com!gentoo.com!rael!aleks
 *   The author retains all rights to this work, and may alter
 *   the restrictions/permissions at any time.  The author assumes
 *   no responibility for usefulness or damage incurred arising
 *   from any use of this intellectual property.  All distributions
 *   must include this copyright notice.
 *   Permission is hereby granted for non-commerical distribution
 *   and usage.  A small fee ( < $25 US) may be charged to cover
 *   distribution costs.
 *
 ******************************************************************/

/*
//		The definitions here define the substrate of functions the
//		OCT environment provides to the running classes.
//
//		6-march-90	bjw 	Broke out of objc.h.
//		23-mar-90 	bjw 	Macro for substrate debugging messages.
//		3-april-90	bjw 	Added _oc_atExit( callback ).
//    	15-sept-90	bjw 	Name was 'substrate.h', now 'objc-runtime.h'
//     	15-sept-90	bjw 	Added some missing NeXT structures.
*/

#include <objc/objc.h>


/* ----------------------  Helper Routines  ---------------------------- */


#ifndef _OOC_PASS2
extern  void    _oc_errfmt PARMS(( CONST char *fmt, ... ));
extern  void    _oc_atExit PARMS(( IMP callback )); /* Call at app_exit() */

extern  IMP     _msgCheck PARMS(( Class  class, SEL sel ));


/*  Substrate operations:  */

extern  Class     objc_getClass PARMS(( STR className ));   /* instance */
extern  Class     objc_getMetaClass PARMS(( STR className ));   /* factory */


/*  Specific extensions...  */
extern char       *_progName;   	/* From argv[0] */


/*  Amiga specific runtime.  For non-amigas, this is part of substrate. */
extern struct Node 	*FindName PARMS(( struct List *, CONST char * ));
extern void  	NewList PARMS(( struct List * ));
extern void 	AddHead PARMS(( struct List *, struct Node * ));

# ifdef MCH_AMIGA
extern struct Process   *_oc_thread0;	/* Holds central Process ID */
# endif


#endif


/*  end of objc-runtime.h */


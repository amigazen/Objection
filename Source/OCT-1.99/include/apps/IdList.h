/*  interface:  IdList      Free flowing list of objects    April 90, bjw */

#ifndef _APPS_IDLIST_H
# define _APPS_IDLIST_H 1

#include <objc/List.h>

/*
 * IdList is a real subclass of List (not a macro alias) so that the amiga
 * library can ship IdList.o without redefining List and causing duplicate
 * symbols when linking with AEBase.lib.  toFirst returns first element (same
 * as first) for API compatibility.
 */
@interface IdList : List
- toFirst ;
@end

#endif


/*  class @interface :  System                                   */

/*   Misc system functions that won't clutter the Object class.
 *
 *    19-sep-93     bjw     Use #include instead of #import.
 */

#ifndef  _OBJC_SYSTEM_H
# define  _OBJC_SYSTEM_H  1

#include  <objc/Object.h>

@interface System : Object ;	 /* no ivars! */


/* -------------------  Public Methods  ------------------- */

+ abort: (int) rc ;
+ initCache ;
+ dumpDictionary: (Class) aClass ;  // Display the methods for aClass:
+ beep ;
+ getProfileString: (STR) key ;
+ putProfileString: (STR) key data: (STR) value ;

@end

#endif


/*
 * String.h -- String of characters (OCT).
 * Null-terminated character array with length/size; object-oriented
 * string for use in Collections.
 */

#ifndef _OBJC_STRING_H
# define _OBJC_STRING_H  1

#include <objc/Object.h>

@interface String : Object
{
  unsigned int  length;   /* Length of string (excluding NUL) */
  unsigned int  size;     /* Allocated buffer size */
  char         *string;   /* Null-terminated C string */
}

+ str: (STR) aStr ;
+ sprintf: (STR) aFormatStr vaDcl: firstArg ;
- (STR) str ;
- (STR) describe ;
- str: (STR) aStr ;
- (char) charAt: (int) anOffset ;
- (char) charAt: (int) anOffset put: (char) aChar ;
- (STR) strcat: (STR) aBuf ;
- (int) asInt ;
- (long) asLong ;
- (double) asFloat ;
- (unsigned) hash ;
- (int) compare: anObject ;
- (int) compareSTR: (STR) aCString ;
- (BOOL) isEqual: anObject ;
- (BOOL) isEqualSTR: (STR) aStr ;
- (BOOL) isCopyOf: anObject ;
- concat: anObject ;
- concatSTR: (STR) aCString ;
- boundsViolation: (int) anOffset ;
- sort ;

@end

#endif

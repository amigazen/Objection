//		Class Interface: ByteArray.m

#ifndef _OBJC_BYTEARRAY_H
#define  _OBJC_BYTEARRAY_H  1

#include <objc/Object.h>

@interface  ByteArray : Object
{
	short	capacity;		/* How many storing now */
}


+ new: (int) nElements ;
+ str: (STR) aStr ;
+ (int) ndxVarSize ;
+ (STR) ndxVarType ;
- (int) count ;
- (int) capacity ;
+ sprintf: (STR)aFormatStr vaDcl:firstArg ;
- (STR) describe ;
- (STR) str ;
- (int) asInt ;
- (long) asLong ;
- (double) asFloat ;
- str: (STR) aStr ;
- (char) charAt:(int) anOffset ;
- (char) charAt:(int) anOffset put:(char) aChar ;
- (unsigned) hash ;
- (int) compareSTR: (STR) aCString ;
- (int) compare: anObject ;
- (BOOL) isEqual: anObject ;
- (BOOL) isEqualSTR: (STR) str ;
- (BOOL) isCopyOf: anObject ;
- boundsViolation: (int) anOffset ;

@end

#endif


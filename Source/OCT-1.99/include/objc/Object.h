//    Object.h  --  Class Interface Header file

/*
 *    28-sept-89	bjw 	Initial coding.
 *    6-feb-90      bjw 	Removed sysFlags, now part of _SHARED.
 *    15-sept-90	bjw 	Added some missing NeXT structures.
 *    19-sep-93     bjw     Use #include instead of #import.
 */


#ifndef  _OBJC_OBJECT_H
# define  _OBJC_OBJECT_H   1


#include <objc/objc.h>

@interface Object
{
@public
	Class 	isa;		// Famous "is-a" link...
}


//   Methods as defined by NeXT, Inc:
+ new ;
- free ;
- initialize ;
- copy ;
+ free ;
- (STR) name ;
- (unsigned) hash ;
- (BOOL) isEqual: anObject ;
- self ;	// new as NeXT.
+ (Class) class ;	// new as NeXT.
+ (Class) superClass ;	// new as NeXT.
- (Class) class ;
- (Class) superClass ;
- (BOOL) isKindOf: (Class)aClass ;
- (BOOL) isMemberOf: (Class)aClass ;
- (BOOL) isKindOfGivenName: (STR)className ;	// new as NeXT.
- (BOOL) isMemberOfGivenName: (STR)className ;
+ (int) version ;	// new as NeXT.
+ (Class) setVersion: (int) versionNumber ; 	// new as NeXT.
+ (BOOL) instanceRespondsTo: (SEL)aSelector ;	// new as NeXT.
- (BOOL) respondsTo: (SEL)aSelector ;
- (IMP)  methodFor: (SEL)aSelector ;
+ (IMP)  instanceMethodFor: (SEL)aSelector ;	// new as NeXT.
- perform: (SEL)aSelector ;
- perform: (SEL)aSelector with:anObject ;
- perform: (SEL)aSelector with:anObject with:secondObject ;
+ poseAs: (Class) aClass ;
- (Class) findClass: (STR)className ;	// different on NeXT.
- subClassResponsibility: (SEL) aSelector ; 	// different on NeXT.
- shouldNotImplement: (SEL) aSelector ; 	// different on NeXT.
- notImplemented: (SEL) aSelector ; 	// different on NeXT.
- doesNotRecognize: (SEL) aSelector ;	// different on NeXT.
- error:(STR)aFormatString vaDcl:arg1 ;
- awake ;
- write: (IOD) stream ; 	// new as NeXT.
- read: (IOD) stream ; 	// new as NeXT.
- finishUnarchiving ; 	// new as NeXT.

//  These are extras not defined in Object on the NeXT computer:
- freeContents ;
+ initialize ;
+ (int) ndxVarSize ;
- (int) capacity ;
- (int) count ;
- show ;
- (STR) str ;
- next ;
- (BOOL) isEqualSTR: (STR)aCStr ;
- (int) compare: anObject ;
- (BOOL) isSame: anObject ;
- (BOOL) notEqual: anObject ;
- (BOOL) notSame: anObject ;
- (STR) describe ;

@end

#endif


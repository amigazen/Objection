/*
 * NSObject.h -- OpenStep root class interface.
 * NSObject subclasses Object (OCT root) and adds OpenStep-style
 * description and reference-counting protocol.
 */

#ifndef _OPENSTEP_NSOBJECT_H
# define _OPENSTEP_NSOBJECT_H 1

# include <objc/Object.h>

@interface NSObject : Object

+ (id) alloc;
- (id) init;
- (void) dealloc;

- (id) self;
- (Class) class;
- (Class) superclass;
- (const char *) name;

- (id) description;
- (BOOL) isEqual: (id)anObject;
- (unsigned int) hash;

- (BOOL) isKindOf: (Class)aClass;
- (BOOL) isMemberOf: (Class)aClass;
- (BOOL) respondsTo: (SEL)aSelector;
- (IMP) methodFor: (SEL)aSelector;

- (id) perform: (SEL)aSelector;
- (id) perform: (SEL)aSelector with: (id)anObject;
- (id) perform: (SEL)aSelector with: (id)obj1 with: (id)obj2;

- (id) autorelease;

@end

#endif

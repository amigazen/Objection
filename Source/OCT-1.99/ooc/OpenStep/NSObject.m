/*
 * NSObject.m -- OpenStep root class implementation.
 * Subclasses Object (OCT root); adds description and OpenStep-style protocol.
 */

#include <OpenStep/NSObject.h>
#include <objc/objc-runtime.h>
#include <objc/vectors.h>
#include <stdio.h>
#include <string.h>

@implementation NSObject : Object

+ (id) alloc
{
  return (*oc_alloc)(self, 0);
}

- (id) init
{
  return self;
}

- (void) dealloc
{
  [super free];
}

- (id) self
{
  return self;
}

- (Class) class
{
  return (Class)self->isa;
}

- (Class) superclass
{
  return self->isa->super_class;
}

- (const char *) name
{
  return self->isa->name;
}

- (id) description
{
  return (id)self->isa->name;
}

- (BOOL) isEqual: (id)anObject
{
  return (BOOL)(self == anObject);
}

- (unsigned int) hash
{
  return (unsigned int)(unsigned long)self;
}

- (BOOL) isKindOf: (Class)aClass
{
  Class cls;

  for (cls = self->isa; cls != (Class)0; cls = cls->super_class)
    if (cls == aClass)
      return YES;
  return NO;
}

- (BOOL) isMemberOf: (Class)aClass
{
  return (BOOL)(self->isa == aClass);
}

- (BOOL) respondsTo: (SEL)aSelector
{
  return (BOOL)(_msgCheck(self->isa, aSelector) != (IMP)0);
}

- (IMP) methodFor: (SEL)aSelector
{
  return _msgCheck(self->isa, aSelector);
}

- (id) perform: (SEL)aSelector
{
  return (id)_msg(self, aSelector);
}

- (id) perform: (SEL)aSelector with: (id)anObject
{
  return (id)_msg(self, aSelector, anObject);
}

- (id) perform: (SEL)aSelector with: (id)obj1 with: (id)obj2
{
  return (id)_msg(self, aSelector, obj1, obj2);
}

- (id) autorelease
{
  return self;
}

@end

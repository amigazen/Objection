/*
 * Object.m -- Root class implementation for OCT (Objective-C Translator).
 *
 * This class is the root of the inheritance hierarchy. It establishes how
 * objects are managed in memory and defines behaviors inherited by all
 * other classes. Factory methods provide the basic protocol for
 * instantiating objects of any class.
 *
 * OCT runtime: oc_alloc, oc_dealloc, oc_copy, oc_cvtToId, oc_cvtToSel,
 * oc_show, oc_error from vectors.h; _msg, _msgCheck from objc runtime.
 */

#include <objc/Object.h>
#include <objc/objc-runtime.h>
#include <objc/objc-class.h>
#include <objc/vectors.h>
#include <objc/release.h>
#include <stdio.h>
#include <string.h>

/* Substrate hooks for activation/passivation (provided by ocsubs). */
extern id (*_storeOn)();
extern id (*_readFrom)();

/* poseAs is a runtime hook (provided by ocsubs/subbind). */
extern void poseAs();

@implementation Object

/* -----------------------------------------------------------------------
 * Factory (class) methods
 * ----------------------------------------------------------------------- */

+ initialize
{
  return (id)self;
}

+ new
{
  return (*oc_alloc)(self, 0);
}

+ free
{
  return nil;
}

+ (int) ndxVarSize
{
  return 0;
}

+ (STR) ndxVarType
{
  return "";
}

+ (STR) describe
{
  return "";
}

+ (Class) class
{
  return self;
}

+ (Class) superClass
{
  return self->super_class;
}

+ (int) version
{
  return (int)self->version;
}

+ (Class) setVersion: (int) versionNumber
{
  self->version = (UBYTE)versionNumber;
  return self;
}

+ (BOOL) instanceRespondsTo: (SEL) aSelector
{
  return (BOOL)(_msgCheck(self, aSelector) != (IMP)0);
}

+ (IMP) instanceMethodFor: (SEL) aSelector
{
  return _msgCheck(self, aSelector);
}

+ poseAs: (Class) aClass
{
  poseAs(self, aClass);
  return (id)self;
}

+ readFrom: (STR) aFileName
{
  return (*_readFrom)(aFileName);
}

/* -----------------------------------------------------------------------
 * Instance methods: creation, copy, free
 * ----------------------------------------------------------------------- */

- initialize
{
  return self;
}

- free
{
  return (*oc_dealloc)(self);
}

- copy
{
  return (*oc_copy)(self, 0);
}

- shallowCopy
{
  return (*oc_copy)(self, 0);
}

- deepCopy
{
  id copy = (*oc_copy)(self, 0);
  return copy;
}

/* -----------------------------------------------------------------------
 * Identity and self
 * ----------------------------------------------------------------------- */

- self
{
  return self;
}

- (Class) class
{
  return (Class)self->isa;
}

- (Class) superClass
{
  return self->isa->super_class;
}

- (STR) name
{
  return self->isa->name;
}

- (STR) str
{
  return self->isa->name;
}

- (STR) describe
{
  return self->isa->name;
}

/* -----------------------------------------------------------------------
 * Indexed instance variable protocol (default: no indexed vars)
 * ----------------------------------------------------------------------- */

- (int) capacity
{
  return 0;
}

- (int) count
{
  return 0;
}

- (unsigned) size
{
  return 0;
}

/* -----------------------------------------------------------------------
 * Type and selector introspection
 * ----------------------------------------------------------------------- */

- (Class) findClass: (STR) className
{
  return (Class)(*oc_cvtToId)(className);
}

- (BOOL) isKindOf: (Class) aClass
{
  Class cls;

  for (cls = self->isa; cls != Nil; cls = cls->super_class)
    if (cls == aClass)
      return YES;
  return NO;
}

- (BOOL) isMemberOf: (Class) aClass
{
  return (BOOL)(self->isa == aClass);
}

- (BOOL) isKindOfGivenName: (STR) className
{
  Class cls;

  if (className == (STR)0)
    return NO;
  for (cls = self->isa; cls != Nil; cls = cls->super_class)
    if (cls->name && strcmp(cls->name, className) == 0)
      return YES;
  return NO;
}

- (BOOL) isMemberOfGivenName: (STR) className
{
  return (className != (STR)0 && self->isa->name &&
	  strcmp(self->isa->name, className) == 0);
}

- (BOOL) respondsTo: (SEL) aSelector
{
  return (BOOL)(_msgCheck(self->isa, aSelector) != (IMP)0);
}

- (IMP) methodFor: (SEL) aSelector
{
  return _msgCheck(self->isa, aSelector);
}

/* -----------------------------------------------------------------------
 * Dynamic perform (dispatch through runtime _msg)
 * ----------------------------------------------------------------------- */

- perform: (SEL) aSelector
{
  return (id)_msg(self, aSelector);
}

- perform: (SEL) aSelector with: anObject
{
  return (id)_msg(self, aSelector, anObject);
}

- perform: (SEL) aSelector with: anObject with: secondObject
{
  return (id)_msg(self, aSelector, anObject, secondObject);
}

/* -----------------------------------------------------------------------
 * Comparing (defaults: identity and subclass responsibility for order)
 * ----------------------------------------------------------------------- */

- (unsigned) hash
{
  return (unsigned)PTR2INT(self);
}

- (BOOL) isEqual: anObject
{
  return (BOOL)(self == anObject);
}

- (int) compare: anObject
{
  if ([self isEqual: anObject])
    return 0;
  return (self > anObject) ? 1 : -1;
}

- (BOOL) isSame: anObject
{
  return (BOOL)(self == anObject);
}

- (BOOL) notEqual: anObject
{
  return ![self isEqual: anObject];
}

- (BOOL) notSame: anObject
{
  return (BOOL)(self != anObject);
}

- (BOOL) isCopyOf: anObject
{
  return [self isEqual: anObject];
}

- (BOOL) isEqualSTR: (STR) aCStr
{
  STR s;

  s = [self str];
  if (s == (STR)0 || aCStr == (STR)0)
    return (BOOL)(s == aCStr);
  return (BOOL)(strcmp(s, aCStr) == 0);
}

/* -----------------------------------------------------------------------
 * Error and subclass responsibility
 * ----------------------------------------------------------------------- */

- subClassResponsibility: (SEL) aSelector
{
  return [self error: "Subclass should override this message." vaDcl: 0];
}

- shouldNotImplement: (SEL) aSelector
{
  return [self error: "Message is not appropriate for this class." vaDcl: 0];
}

- notImplemented: (SEL) aSelector
{
  return [self error: "Does not implement this message." vaDcl: 0];
}

- doesNotRecognize: (SEL) aSelector
{
  return [self error: "%s does not recognize selector." vaDcl: [self name]];
}

- error: (STR) aFormatString vaDcl: arg1
{
  return (*oc_error)(self, aFormatString, arg1);
}

/* -----------------------------------------------------------------------
 * Activation / passivation (store, read, show)
 * ----------------------------------------------------------------------- */

- (BOOL) storeOn: (STR) aFileName
{
  return (BOOL)((*_storeOn)(aFileName, self) != nil);
}

- show
{
  return (*oc_show)(self, 0);
}

/* -----------------------------------------------------------------------
 * Archiving (minimal: subclasses override)
 * ----------------------------------------------------------------------- */

- awake
{
  return self;
}

- write: (IOD) stream
{
  (void)stream;
  return self;
}

- read: (IOD) stream
{
  (void)stream;
  return self;
}

- finishUnarchiving
{
  return self;
}

/* -----------------------------------------------------------------------
 * Extras (OCT Object.h)
 * ----------------------------------------------------------------------- */

- freeContents
{
  return self;
}

- next
{
  return nil;
}

- asGraph: (BOOL) unique
{
  (void)unique;
  return self;
}

@end

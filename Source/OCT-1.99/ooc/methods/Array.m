/*
 * Array.m -- Abstract array superclass for OCT (Objective-C Translator).
 *
 * Indexed storage follows the object layout: IV(obj) points to the first
 * indexed variable (see objc.h). Subclasses override ndxVarSize and
 * ndxVarType and provide type-specific access (e.g. at: / at:put: for ids).
 */

#include <objc/Array.h>
#include <objc/vectors.h>
#include <string.h>
#include <objc/objc-runtime.h>

@implementation Array : Object

/* -----------------------------------------------------------------------
 * Factory: creation
 * ----------------------------------------------------------------------- */

+ new: (int) nElements
{
  id newObject;

  newObject = (*oc_alloc)(self, [self ndxVarSize] * nElements);
  newObject->capacity = (short)nElements;
  return newObject;
}

/* Subclass responsibility: argument types vary (id, int, etc.). */
+ with: (int) nArgs vaDcl: arglist
{
  id newObject;

  newObject = [self new: nArgs];
  memcpy(IV(newObject), (char *)&arglist, (unsigned)(nArgs * [self ndxVarSize]));
  return newObject;
}

+ (STR) ndxVarType
{
  [self subClassResponsibility: _cmd];
  return "";
}

+ (int) ndxVarSize
{
  [self subClassResponsibility: _cmd];
  return 0;
}

/* -----------------------------------------------------------------------
 * Instance: size and capacity (activation/passivation rely on capacity)
 * ----------------------------------------------------------------------- */

- (int) size
{
  return capacity;
}

- (int) capacity
{
  return capacity;
}

/* May relocate; new id may differ. Uses substrate oc_realloc. */
- capacity: (int) nSlots
{
  return (*oc_realloc)(self,
      [self ndxVarSize] * nSlots + (int)self->isa->clsSizInstance);
}

/* -----------------------------------------------------------------------
 * Compare, hash, equality
 * ----------------------------------------------------------------------- */

- (unsigned) hash
{
  return (unsigned)((~(unsigned)capacity | PTR2INT(self->isa)) & OC_HASH_MASK);
}

- (BOOL) isEqual: anObject
{
  return (BOOL)(self == anObject);
}

/* Same contents (e.g. copies). Compare header + indexed bytes. */
- (BOOL) isCopyOf: anObject
{
  char *s, *t;
  int n;

  if (self == anObject)
    return YES;
  if (!anObject || anObject->isa != isa)
    return NO;
  n = [self->isa ndxVarSize] * self->capacity + (int)self->isa->clsSizInstance;
  s = (char *)self;
  t = (char *)anObject;
  while (--n >= 0 && *s++ == *t++)
    ;
  return (BOOL)(n < 0);
}

/* -----------------------------------------------------------------------
 * Error handling ("bounds violation" / "zero capacity array")
 * ----------------------------------------------------------------------- */

- boundsViolation: (int) anOffset
{
  int maxIdx;

  maxIdx = capacity > 0 ? capacity - 1 : -1;
  return [self error: "bounds violation: %d outside range [0..%d]",
      anOffset, maxIdx];
}

/* -----------------------------------------------------------------------
 * Describe (indexed var type string); sort is subclass responsibility
 * ----------------------------------------------------------------------- */

- (STR) describe
{
  [self subClassResponsibility: _cmd];
  return "";
}

- (int) count
{
  return capacity;
}

- sort
{
  [self subClassResponsibility: _cmd];
  return self;
}

@end

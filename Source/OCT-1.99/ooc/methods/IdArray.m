/*
 * IdArray.m -- Array of object identifiers (id) for OCT.
 */

#include <objc/IdArray.h>
#include <objc/vectors.h>
#include <objc/objc-runtime.h>
#include <collect/Sequence.h>
#include <string.h>

#define IDS(self)  ((id *)IV(self))
/* Avoid IDS(self)[i] so OCT does not parse [ as message start; use pointer add. */
#define IDAT(self, idx)  (*(IDS(self) + (idx)))

@implementation IdArray : Object

/* -----------------------------------------------------------------------
 * Indexed variable type (activation/passivation)
 * ----------------------------------------------------------------------- */

+ (int) ndxVarSize
{
  return (int)sizeof(id);
}

+ (STR) ndxVarType
{
  return "@";
}

- (STR) describe
{
  return "@";
}

/* -----------------------------------------------------------------------
 * Factory
 * ----------------------------------------------------------------------- */

+ new: (int) nElements
{
  id newObject;

  newObject = (*oc_alloc)(self, [self ndxVarSize] * nElements);
  newObject->capacity = (short)nElements;
  return newObject;
}

+ with: (int) nArgs vaDcl: arglist
{
  id newObject;
  id *p = &arglist;

  newObject = [self new: nArgs];
  while (nArgs--)
    [newObject add: (*p++)];
  return newObject;
}

/* -----------------------------------------------------------------------
 * Accessing: at:, at:put:
 * ----------------------------------------------------------------------- */

- at: (int) anOffset
{
  if (anOffset < 0 || anOffset >= capacity)
    return [self boundsViolation: anOffset];
  return IDAT(self, anOffset);
}

- at: (int) anOffset put: anObject
{
  id old;

  if (anOffset < 0 || anOffset >= capacity)
    return [self boundsViolation: anOffset];
  old = IDAT(self, anOffset);
  IDAT(self, anOffset) = anObject;
  return old;
}

/* -----------------------------------------------------------------------
 * Adding: add: uses first nil slot; addContentsTo/Of
 * ----------------------------------------------------------------------- */

- add: anObject
{
  int i;

  for (i = 0; i < capacity; i++)
    if (IDAT(self, i) == nil)
      {
	IDAT(self, i) = anObject;
	return self;
      }
  return [self boundsViolation: capacity];
}

- addContentsTo: aList
{
  int i;
  id *p;

  for (i = 0, p = IDS(self); i < capacity; i++, p++)
    if (*p != nil)
      [aList add: *p];
  return aList;
}

- addContentsOf: aCollection
{
  id seq;
  id member;

  seq = [aCollection eachElement];
  while ((member = [seq next]) != nil)
    [self add: member];
  [seq free];
  return self;
}

/* -----------------------------------------------------------------------
 * Removing: removeAt:, remove:, removeContentsFrom:, packContents
 * ----------------------------------------------------------------------- */

- removeAt: (register int) anOffset
{
  id old;
  int i;

  if (anOffset < 0 || anOffset >= capacity)
    return [self boundsViolation: anOffset];
  old = IDAT(self, anOffset);
  for (i = anOffset; i < capacity - 1; i++)
    IDAT(self, i) = IDAT(self, i + 1);
  IDAT(self, capacity - 1) = nil;
  return old;
}

- remove: anObject
{
  int i;

  for (i = 0; i < capacity; i++)
    if (IDAT(self, i) == anObject)
      {
	IDAT(self, i) = nil;
	return anObject;
      }
  return nil;
}

- removeContentsFrom: aList
{
  int i;
  id *p;

  for (i = 0, p = IDS(self); i < capacity; i++, p++)
    if (*p != nil)
      [aList remove: *p];
  return self;
}

- packContents
{
  int i, j;
  id *p;

  for (i = 0, j = 0, p = IDS(self); i < capacity; i++, p++)
    if (*p != nil)
      {
	if (i != j)
	  IDAT(self, j) = *p;
	j++;
      }
  for ( ; j < capacity; j++)
    IDAT(self, j) = nil;
  return self;
}

/* -----------------------------------------------------------------------
 * Testing: count (non-nil), capacity
 * ----------------------------------------------------------------------- */

- (int) count
{
  int n, i;

  for (n = 0, i = 0; i < capacity; i++)
    if (IDAT(self, i) != nil)
      n++;
  return n;
}

- (int) capacity
{
  return capacity;
}

- (BOOL) contains: anObject
{
  return [self find: anObject] != nil;
}

/* -----------------------------------------------------------------------
 * Comparing, hash (for Set membership)
 * ----------------------------------------------------------------------- */

- (unsigned) hash
{
  unsigned n, code;
  id *p;

  code = (unsigned)PTR2INT(isa) ^ (unsigned)capacity;
  for (n = capacity, p = IDS(self); n--; p++)
    if (*p != nil)
      code = (code * 31) ^ (unsigned)[*p hash];
  return code;
}

- (BOOL) isEqual: anObject
{
  int i;
  id *p, *q;

  if (self == anObject)
    return YES;
  if (anObject == nil || anObject->isa != isa ||
      [anObject capacity] != capacity)
    return NO;
  for (i = 0, p = IDS(self), q = (id *)IV(anObject); i < capacity; i++, p++, q++)
    if (*p != *q && (*p == nil || *q == nil || [*p isEqual: *q] == NO))
      return NO;
  return YES;
}

- (BOOL) isCopyOf: anObject
{
  return [super isCopyOf: anObject];
}

/* -----------------------------------------------------------------------
 * copy: include variable-sized slot storage so oc_copy copies full object.
 * ----------------------------------------------------------------------- */

- copy
{
  int extra;

  extra = capacity * (int)[[self class] ndxVarSize];
  return (*oc_copy)(self, extra);
}

/* -----------------------------------------------------------------------
 * capacity: (realloc)
 * ----------------------------------------------------------------------- */

- capacity: (int) nSlots
{
  return (*oc_realloc)(self,
      [self ndxVarSize] * nSlots + (int)self->isa->clsSizInstance);
}

- boundsViolation: (int) anOffset
{
  int maxIdx;

  maxIdx = capacity > 0 ? capacity - 1 : -1;
  return [self error: "bounds violation: %d outside range [0..%d]",
      anOffset, maxIdx];
}

/* -----------------------------------------------------------------------
 * Searching: find:, findMatching:, findSTR:
 * ----------------------------------------------------------------------- */

- find: anObject
{
  int i;

  for (i = 0; i < capacity; i++)
    if (IDAT(self, i) == anObject)
      return anObject;
  return nil;
}

- findMatching: anObject
{
  int i;
  id elem;

  for (i = 0; i < capacity; i++)
    {
      elem = IDAT(self, i);
      if (elem != nil && [elem isEqual: anObject])
	return elem;
    }
  return nil;
}

- findSTR: (STR) aStr
{
  int i;
  id elem;

  for (i = 0; i < capacity; i++)
    {
      elem = IDAT(self, i);
      if (elem != nil && [elem isEqualSTR: aStr])
	return elem;
    }
  return nil;
}

/* -----------------------------------------------------------------------
 * Offset determination
 * ----------------------------------------------------------------------- */

- (int) offsetOf: anObject
{
  int i;

  for (i = 0; i < capacity; i++)
    if (IDAT(self, i) == anObject)
      return i;
  return -1;
}

- (int) offsetMatching: anObject
{
  int i;

  for (i = 0; i < capacity; i++)
    if (IDAT(self, i) != nil && [IDAT(self, i) isEqual: anObject])
      return i;
  return -1;
}

- (int) offsetSTR: (STR) aStr
{
  int i;

  for (i = 0; i < capacity; i++)
    if (IDAT(self, i) != nil && [IDAT(self, i) isEqualSTR: aStr])
      return i;
  return -1;
}

/* -----------------------------------------------------------------------
 * Deallocating: freeContents frees each member, not the IdArray
 * ----------------------------------------------------------------------- */

- freeContents
{
  int i;
  id *p;

  for (i = 0, p = IDS(self); i < capacity; i++, p++)
    if (*p != nil)
      {
	[*p free];
	*p = nil;
      }
  return self;
}

/* -----------------------------------------------------------------------
 * Enumerating: eachElement, eachElementPerform:
 * ----------------------------------------------------------------------- */

- eachElement
{
  return [Sequence array: self];
}

- eachElementPerform: (SEL) aSelector
{
  int i;
  id *p;

  for (i = 0, p = IDS(self); i < capacity; i++, p++)
    if (*p != nil)
      [*p perform: aSelector];
  return self;
}

- eachElementPerform: (SEL) aSelector with: arg1
{
  int i;
  id *p;

  for (i = 0, p = IDS(self); i < capacity; i++, p++)
    if (*p != nil)
      [*p perform: aSelector with: arg1];
  return self;
}

- eachElementPerform: (SEL) aSelector with: arg1 with: arg2
{
  int i;
  id *p;

  for (i = 0, p = IDS(self); i < capacity; i++, p++)
    if (*p != nil)
      [*p perform: aSelector with: arg1 with: arg2];
  return self;
}

@end

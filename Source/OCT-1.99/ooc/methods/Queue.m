/*
 * Queue.m -- Double-ended queue; order of addition preserved.
 */

#include <collect/Queue.h>
#include <objc/objc-runtime.h>

#define FIRST_EXPAND  9
#define NEXT_EXPAND(c)  (((CAPACITY(c)) * 4) / 3)

@implementation Queue : Collection

/* -----------------------------------------------------------------------
 * expand: create or grow contents; set firstEmptySlot from packed count
 * ----------------------------------------------------------------------- */

- expand
{
  id prev = self->contents;
  int n;

  if (prev == nil)
    {
      self->contents = [IdArray new: FIRST_EXPAND];
      self->firstEmptySlot = 0;
    }
  else
    {
      self->contents = [IdArray new: NEXT_EXPAND(self->contents)];
      [prev addContentsTo: self];
      n = [self->contents count];
      self->firstEmptySlot = (short)n;
      [prev free];
    }
  return self;
}

/* -----------------------------------------------------------------------
 * add: append at tail; reject nil; expand when full
 * ----------------------------------------------------------------------- */

- add: newObject
{
  if (newObject == nil)
    return self;
  if (self->contents == nil || self->firstEmptySlot >= CAPACITY(self->contents))
    [self expand];
  ARRAY(self->contents)[(self->firstEmptySlot)++] = newObject;
  return self;
}

/* -----------------------------------------------------------------------
 * addAtFront: insert at index 0; shift existing elements up; reject nil
 * ----------------------------------------------------------------------- */

- addAtFront: newObject
{
  int i;

  if (newObject == nil)
    return self;
  if (self->contents == nil || self->firstEmptySlot >= CAPACITY(self->contents))
    [self expand];
  for (i = self->firstEmptySlot - 1; i >= 0; i--)
    ARRAY(self->contents)[i + 1] = ARRAY(self->contents)[i];
  ARRAY(self->contents)[0] = newObject;
  self->firstEmptySlot++;
  return self;
}

/* -----------------------------------------------------------------------
 * at: / at:put: with bounds check; delegate to contents
 * ----------------------------------------------------------------------- */

- at: (int) offset
{
  if (offset < 0 || offset >= self->firstEmptySlot)
    return [self boundsViolation: offset];
  return [self->contents at: offset];
}

- at: (int) offset put: obj
{
  if (offset < 0 || offset >= self->firstEmptySlot)
    return [self boundsViolation: offset];
  return [self->contents at: offset put: obj];
}

- firstElement
{
  return [self at: 0];
}

- lastElement
{
  if (self->firstEmptySlot == 0)
    return nil;
  return [self at: self->firstEmptySlot - 1];
}

/* -----------------------------------------------------------------------
 * addContentsOf: add all from aCollection; set firstEmptySlot from count
 * ----------------------------------------------------------------------- */

- addContentsOf: aCollection
{
  int n;

  [super addContentsOf: aCollection];
  n = [self->contents count];
  self->firstEmptySlot = (short)n;
  return self;
}

/* -----------------------------------------------------------------------
 * count: logical size (firstEmptySlot)
 * ----------------------------------------------------------------------- */

- (int) count
{
  return self->firstEmptySlot;
}

/* -----------------------------------------------------------------------
 * packContents: remove nils from contents; set firstEmptySlot from
 * packed count (fix: do not use [self count], which is firstEmptySlot)
 * ----------------------------------------------------------------------- */

- packContents
{
  int n;

  if (self->contents != nil)
    {
      [self->contents packContents];
      n = [self->contents count];
      self->firstEmptySlot = (short)n;
    }
  return self;
}

/* -----------------------------------------------------------------------
 * remove: one matching object; clear slot then pack; update firstEmptySlot
 * ----------------------------------------------------------------------- */

- remove: anObject
{
  int off;
  id cut = nil;

  off = [self offsetMatching: anObject];
  if (off >= 0)
    {
      cut = [self at: off];
      [self at: off put: nil];
      [self packContents];
    }
  return cut;
}

/* -----------------------------------------------------------------------
 * boundsViolation: report and error (vaDcl for OCT)
 * ----------------------------------------------------------------------- */

- boundsViolation: (int) anOffset
{
  return [self error: "cannot access element %d (firstEmpty=%d, cap=%d)",
      anOffset, self->firstEmptySlot, [self capacity]];
}

@end

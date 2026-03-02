/*
 * OrderedCollection.m -- Ordered collection (order of addition preserved).
 */

#include <collect/OrderedCollection.h>

#define FIRST_EXPAND  9
#define NEXT_EXPAND(c)  (((CAPACITY(c)) * 4) / 3)

@implementation OrderedCollection : Collection

/* -----------------------------------------------------------------------
 * Expand: grow contents IdArray or create initial one
 * ----------------------------------------------------------------------- */

- expand
{
  int n;
  id prev = contents;

  if (prev == nil)
    {
      contents = [IdArray new: FIRST_EXPAND];
      firstEmptySlot = 0;
    }
  else
    {
      contents = [IdArray new: NEXT_EXPAND(contents)];
      [prev addContentsTo: self];
      n = [contents count];
      firstEmptySlot = (short)n;
      [prev free];
    }
  return self;
}

/* -----------------------------------------------------------------------
 * Adding: append at firstEmptySlot; expand if needed; reject nil
 * ----------------------------------------------------------------------- */

- add: newObject
{
  if (newObject == nil)
    return self;
  if (contents == nil || firstEmptySlot >= CAPACITY(contents))
    [self expand];
  ARRAY(contents)[firstEmptySlot++] = newObject;
  return self;
}

/* -----------------------------------------------------------------------
 * Accessing: at:, firstElement, lastElement
 * ----------------------------------------------------------------------- */

- at: (int) offset
{
  if (offset < 0 || offset >= firstEmptySlot)
    return [self boundsViolation: offset];
  return [contents at: offset];
}

- at: (int) offset put: obj
{
  if (offset < 0 || offset >= firstEmptySlot)
    return [self boundsViolation: offset];
  return [contents at: offset put: obj];
}

- firstElement
{
  return [self at: 0];
}

- lastElement
{
  if (firstEmptySlot == 0)
    return nil;
  return [self at: firstEmptySlot - 1];
}

/* -----------------------------------------------------------------------
 * addContentsOf: add all from aCollection then set firstEmptySlot
 * ----------------------------------------------------------------------- */

- addContentsOf: aCollection
{
  int n;

  [super addContentsOf: aCollection];
  n = [contents count];
  firstEmptySlot = (short)n;
  return self;
}

/* -----------------------------------------------------------------------
 * Testing: count is number of members (firstEmptySlot)
 * ----------------------------------------------------------------------- */

- (int) count
{
  return firstEmptySlot;
}

/* -----------------------------------------------------------------------
 * packContents: remove nils from contents, update firstEmptySlot
 * ----------------------------------------------------------------------- */

- packContents
{
  int n;
  if (contents != nil)
    {
      [contents packContents];
      n = [contents count];
      firstEmptySlot = (short)n;
    }
  return self;
}

/* -----------------------------------------------------------------------
 * remove: first matching id; delegate to contents then pack and sync count
 * ----------------------------------------------------------------------- */

- remove: anObject
{
  id r;

  if (contents == nil)
    return nil;
  r = [contents remove: anObject];
  if (r != nil)
    {
      [contents packContents];
      firstEmptySlot = (short)[contents count];
    }
  return r;
}

/* -----------------------------------------------------------------------
 * Bounds error (boundsError:)
 * ----------------------------------------------------------------------- */

- boundsViolation: (int) anOffset
{
  return [self error: "cannot access element %d (firstEmpty=%d, cap=%d)"
      vaDcl: anOffset, firstEmptySlot, [self capacity]];
}

@end

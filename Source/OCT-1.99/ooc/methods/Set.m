/*
 * Set.m -- Collection with no duplicates (by isEqual:) for OCT.
 */

#include <collect/Set.h>
#include <objc/objc-runtime.h>

#define FIRST_EXPAND  9
#define NEXT_EXPAND(a)  ((CAPACITY(a) * 4) / 3)

/* Expand when no contents or tally >= 75% of capacity */
#define Q_EXPAND(s)  ((s)->contents == nil || \
  (unsigned)(s)->tally * 4 >= (unsigned)CAPACITY((s)->contents) * 3)

/* Find an empty or matching slot (macro form to keep it in method bodies,
 * where OCT will translate message expressions correctly).
 */
#define SET_FIND_SLOT(pSelf, element, begin, end, now, slot, cap, rem, start) \
  do { \
    if ((pSelf)->contents == nil) \
      [pSelf expand]; \
    cap = CAPACITY((pSelf)->contents); \
    begin = ARRAY((pSelf)->contents); \
    end = begin + cap; \
    if (start < 0) start = -start; \
    now = begin + (start % cap); \
    slot = (id *)0; \
    for (rem = cap; rem-- >= 0; now++) { \
      if (now >= end) now = begin; \
      if (*now == nil || [*now isEqual: element]) { slot = now; break; } \
    } \
    if (slot == (id *)0) { [pSelf error: "(findElementOrNil) failure" vaDcl: 0]; } \
  } while (0)

@implementation Set : Collection

/* -----------------------------------------------------------------------
 * expand: create contents or grow, re-add all members
 * ----------------------------------------------------------------------- */

- expand
{
  id prev = self->contents;

  if (prev == nil)
    {
      self->contents = [IdArray new: FIRST_EXPAND];
      self->tally = 0;
    }
  else
    {
      self->contents = [IdArray new: NEXT_EXPAND(prev)];
      self->tally = 0;
      [prev addContentsTo: self];
      [prev free];
    }
  return self;
}

/* -----------------------------------------------------------------------
 * add: add if no equal member; return nil if added, newObject if duplicate
 * ----------------------------------------------------------------------- */

- add: newObject
{
  id *begin, *end, *now, *p;
  int cap, rem, start;
  unsigned h;

  if (newObject == nil)
    return nil;
  if (Q_EXPAND(self))
    [self expand];
  h = [newObject hash];
  start = (int)h;
  SET_FIND_SLOT(self, newObject, begin, end, now, p, cap, rem, start);
  if (*p == nil)
    {
      *p = newObject;
      self->tally++;
      return nil;
    }
  return newObject;
}

/* -----------------------------------------------------------------------
 * filter: add or replace; if duplicate, free newObject and return old
 * ----------------------------------------------------------------------- */

- filter: newObject
{
  id *begin, *end, *now, *p;
  int cap, rem, start;
  unsigned h;

  if (newObject == nil)
    return nil;
  if (Q_EXPAND(self))
    [self expand];
  h = [newObject hash];
  start = (int)h;
  SET_FIND_SLOT(self, newObject, begin, end, now, p, cap, rem, start);
  if (*p == nil)
    {
      self->tally++;
      *p = newObject;
      return newObject;
    }
  [newObject free];
  return *p;
}

/* -----------------------------------------------------------------------
 * addContentsTo: add each non-nil member to aCollection
 * ----------------------------------------------------------------------- */

- addContentsTo: aCollection
{
  id *member;
  int n;

  if (self->contents == nil)
    return aCollection;
  n = [self capacity];
  member = ARRAY(self->contents);
  while (n-- > 0)
    {
      if (*member != nil)
	[aCollection add: *member];
      member++;
    }
  return aCollection;
}

/* -----------------------------------------------------------------------
 * union / intersection / difference
 * ----------------------------------------------------------------------- */

- union: aCollection
{
  id all;

  all = [[self class] new: 1];
  return [[all addContentsOf: self] addContentsOf: aCollection];
}

- difference: aCollection
{
  id all;

  all = [[self class] new: 1];
  return [[all addContentsOf: self] removeContentsOf: aCollection];
}

- intersection: aCollection
{
  id sum, tmp;

  sum = [self union: aCollection];
  tmp = [self difference: aCollection];
  [sum removeContentsOf: tmp];
  [tmp free];
  tmp = [aCollection difference: self];
  [sum removeContentsOf: tmp];
  [tmp free];
  return sum;
}

/* -----------------------------------------------------------------------
 * count, find, contains
 * ----------------------------------------------------------------------- */

- (int) count
{
  return self->tally;
}

- find: anObject
{
  id *begin, *end, *now, *p;
  int cap, rem, start;
  unsigned h;

  if (anObject == nil)
    return nil;
  h = [anObject hash];
  start = (int)h;
  SET_FIND_SLOT(self, anObject, begin, end, now, p, cap, rem, start);
  return p != (id *)0 ? *p : nil;
}

- (BOOL) contains: anObject
{
  return (BOOL)([self find: anObject] != nil);
}

/* -----------------------------------------------------------------------
 * remove: clear slot, decrement tally, reply old member
 * ----------------------------------------------------------------------- */

- remove: anObject
{
  id *begin, *end, *now, *p;
  int cap, rem, start;
  unsigned h;
  id old;

  if (anObject == nil)
    return nil;
  h = [anObject hash];
  start = (int)h;
  SET_FIND_SLOT(self, anObject, begin, end, now, p, cap, rem, start);
  old = *p;
  if (old != nil)
    self->tally--;
  *p = nil;
  return old;
}

@end

/*
 * Sequence.m -- Enumeration over Collections and IdArrays for OCT.
 */

#include <collect/Sequence.h>

#define IDS_OF(c)  ((id *)IV((c)))

@implementation Sequence : Collection

/* -----------------------------------------------------------------------
 * Factory: create sequence over anIdArray (or Collection → its contents)
 * ----------------------------------------------------------------------- */

+ array: anIdArray
{
  id obj;

  if (anIdArray == nil)
    return nil;
  if ([anIdArray isKindOf: Collection])
    {
      anIdArray = ((id)anIdArray)->contents;
      if (anIdArray == nil)
	return nil;
    }
  else if ([anIdArray isMemberOf: IdArray] == NO)
    return [self error: "IdArray or Collection needed" vaDcl: 0];

  obj = [self new];
  obj->contents = anIdArray;   /* reference only; do not modify collection while sequencing */
  obj->offset = 0;
  return obj;
}

/* -----------------------------------------------------------------------
 * Positioning
 * ----------------------------------------------------------------------- */

- toFirst
{
  offset = 0;
  return self;
}

- toLast
{
  offset = (short)(contents != nil ? [contents capacity] : 0);
  return self;
}

- rewind
{
  return [self toFirst];
}

/* -----------------------------------------------------------------------
 * next: reply next non-nil member, advance offset past it
 * ----------------------------------------------------------------------- */

- next
{
  int cap, i;
  id *p;

  if (contents == nil)
    return nil;
  cap = [contents capacity];
  for (i = offset; i < cap; i++)
    {
      p = IDS_OF(contents) + i;
      if (*p != nil)
	{
	  offset = (short)(i + 1);
	  return *p;
	}
    }
  return nil;
}

/* -----------------------------------------------------------------------
 * previous: reply prior non-nil member, set offset to its index
 * ----------------------------------------------------------------------- */

- previous
{
  int i;
  id *p;

  if (contents == nil || offset <= 0)
    return nil;
  for (i = offset - 1; i >= 0; i--)
    {
      p = IDS_OF(contents) + i;
      if (*p != nil)
	{
	  offset = (short)i;
	  return *p;
	}
    }
  return nil;
}

/* -----------------------------------------------------------------------
 * first / last
 * ----------------------------------------------------------------------- */

- first
{
  [self toFirst];
  return [self next];
}

- last
{
  [self toLast];
  return [self previous];
}

/* -----------------------------------------------------------------------
 * count: number of elements remaining (non-nil from offset to end)
 * ----------------------------------------------------------------------- */

- (int) count
{
  int cap, n, i;
  id *p;

  if (contents == nil)
    return 0;
  cap = [contents capacity];
  for (n = 0, i = offset, p = IDS_OF(contents) + offset; i < cap; i++, p++)
    if (*p != nil)
      n++;
  return n;
}

/* -----------------------------------------------------------------------
 * free: release sequence (caller responsibility)
 * ----------------------------------------------------------------------- */

- free
{
  contents = nil;
  offset = 0;
  return [super free];
}

@end

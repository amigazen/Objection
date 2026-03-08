/*
 * NSIndexSet.m -- OpenStep immutable index set implementation.
 * Amiga-only: NSAllocator (no libc malloc).
 */

#include <OpenStep/NSIndexSet.h>
#include <OpenStep/NSAllocator.h>
#include <objc/objc-runtime.h>
#include <objc/vectors.h>

#define MIN_CAPACITY 4
#define GROW(c) (((c) * 3) / 2 + 1)

@implementation NSIndexSet : NSObject

+ (id) indexSet
{
  return [[self alloc] init];
}

+ (id) indexSetWithIndex: (unsigned int)value
{
  return [[self alloc] initWithIndex: value];
}

+ (id) indexSetWithIndexesInRange: (NSRange)range
{
  return [[self alloc] initWithIndexesInRange: range];
}

- (id) init
{
  [super init];
  _indexes = (unsigned int *)0;
  _count = 0;
  _capacity = 0;
  return self;
}

- (id) initWithIndex: (unsigned int)value
{
  [super init];
  _capacity = MIN_CAPACITY;
  _indexes = (unsigned int *)OPENSTEP_ALLOC((size_t)_capacity * sizeof(unsigned int));
  if (_indexes == (unsigned int *)0)
    return (id)0;
  _indexes[0] = value;
  _count = 1;
  return self;
}

- (id) initWithIndexesInRange: (NSRange)range
{
  unsigned int i;
  unsigned int n;

  [super init];
  n = range.length;
  if (n == 0)
    {
      _indexes = (unsigned int *)0;
      _count = 0;
      _capacity = 0;
      return self;
    }
  _capacity = n < MIN_CAPACITY ? MIN_CAPACITY : n;
  _indexes = (unsigned int *)OPENSTEP_ALLOC((size_t)_capacity * sizeof(unsigned int));
  if (_indexes == (unsigned int *)0)
    return (id)0;
  for (i = 0; i < n; i++)
    _indexes[i] = range.location + i;
  _count = n;
  return self;
}

- (id) initWithIndexSet: (id)indexSet
{
  [super init];
  _indexes = (unsigned int *)0;
  _count = 0;
  _capacity = 0;
  if (indexSet == (id)0)
    return self;
  _count = [indexSet count];
  if (_count == 0)
    return self;
  _capacity = _count < MIN_CAPACITY ? MIN_CAPACITY : _count;
  _indexes = (unsigned int *)OPENSTEP_ALLOC((size_t)_capacity * sizeof(unsigned int));
  if (_indexes == (unsigned int *)0)
    return (id)0;
  {
    unsigned int i;
    unsigned int idx;
    idx = [indexSet firstIndex];
    for (i = 0; i < _count; i++)
      {
        _indexes[i] = idx;
        idx = [indexSet indexGreaterThanIndex: idx];
      }
  }
  return self;
}

- (void) dealloc
{
  if (_indexes != (unsigned int *)0)
    {
      OPENSTEP_FREE_VOID(_indexes);
      _indexes = (unsigned int *)0;
    }
  _count = 0;
  _capacity = 0;
  [super dealloc];
}

- free
{
  if (_indexes != (unsigned int *)0)
    {
      OPENSTEP_FREE_VOID(_indexes);
      _indexes = (unsigned int *)0;
    }
  _count = 0;
  _capacity = 0;
  return [super free];
}

- (unsigned int) count
{
  return _count;
}

- (unsigned int) firstIndex
{
  if (_count == 0)
    return NSNotFound;
  return _indexes[0];
}

- (unsigned int) indexGreaterThanIndex: (unsigned int)value
{
  unsigned int lo;
  unsigned int hi;
  unsigned int mid;

  if (_count == 0)
    return NSNotFound;
  if (value >= _indexes[_count - 1])
    return NSNotFound;
  lo = 0;
  hi = _count;
  while (lo < hi)
    {
      mid = lo + (hi - lo) / 2;
      if (_indexes[mid] <= value)
        lo = mid + 1;
      else
        hi = mid;
    }
  return lo < _count ? _indexes[lo] : NSNotFound;
}

- (BOOL) containsIndex: (unsigned int)value
{
  unsigned int lo;
  unsigned int hi;
  unsigned int mid;

  if (_count == 0)
    return NO;
  lo = 0;
  hi = _count;
  while (lo < hi)
    {
      mid = lo + (hi - lo) / 2;
      if (_indexes[mid] < value)
        lo = mid + 1;
      else if (_indexes[mid] > value)
        hi = mid;
      else
        return YES;
    }
  return NO;
}

@end

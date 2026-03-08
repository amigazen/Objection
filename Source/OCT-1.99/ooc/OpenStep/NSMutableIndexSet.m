/*
 * NSMutableIndexSet.m -- OpenStep mutable index set implementation.
 * Amiga-only: NSAllocator (no libc malloc).
 */

#include <OpenStep/NSMutableIndexSet.h>
#include <OpenStep/NSAllocator.h>
#include <objc/objc-runtime.h>
#include <objc/vectors.h>

@includes

#define MIN_CAPACITY 4
#define GROW(c) (((c) * 3) / 2 + 1)

/* Layout for instance ivars; avoid type NSIndexSet (class pointer name in generated C). */
struct _NSIndexSetIvars { void *isa; void *next; unsigned int *_indexes; unsigned int _count; unsigned int _capacity; };

/* Return index of first slot >= value, or _count if all less. */
static unsigned int
find_insert_position(id self, unsigned int value)
{
  struct _NSIndexSetIvars *s;
  unsigned int lo;
  unsigned int hi;
  unsigned int mid;
  unsigned int n;

  s = (struct _NSIndexSetIvars *)self;
  n = s->_count;
  if (n == 0)
    return 0;
  lo = 0;
  hi = n;
  while (lo < hi)
    {
      mid = lo + (hi - lo) / 2;
      if (s->_indexes[mid] < value)
        lo = mid + 1;
      else
        hi = mid;
    }
  return lo;
}

@implementation NSMutableIndexSet : NSIndexSet

+ (id) indexSetWithCapacity: (unsigned int)capacity
{
  return [[self alloc] initWithCapacity: capacity];
}

- (id) initWithCapacity: (unsigned int)capacity
{
  [super init];
  _count = 0;
  _capacity = capacity < MIN_CAPACITY ? MIN_CAPACITY : capacity;
  _indexes = (unsigned int *)OPENSTEP_ALLOC((size_t)_capacity * sizeof(unsigned int));
  if (_indexes == (unsigned int *)0)
    return (id)0;
  return self;
}

- (void) addIndex: (unsigned int)value
{
  unsigned int pos;
  unsigned int i;
  unsigned int *newp;
  unsigned int newcap;

  pos = find_insert_position(self, value);
  if (pos < _count && _indexes[pos] == value)
    return;
  if (_count >= _capacity)
    {
      newcap = GROW(_capacity);
      newp = (unsigned int *)OPENSTEP_REALLOC_KNOWN(_indexes,
        (size_t)_capacity * sizeof(unsigned int),
        (size_t)newcap * sizeof(unsigned int));
      if (newp == (unsigned int *)0)
        return;
      _indexes = newp;
      _capacity = newcap;
    }
  for (i = _count; i > pos; i--)
    _indexes[i] = _indexes[i - 1];
  _indexes[pos] = value;
  _count++;
}

- (void) addIndexesInRange: (NSRange)range
{
  unsigned int i;
  unsigned int end;

  end = range.location + range.length;
  for (i = range.location; i < end; i++)
    [self addIndex: i];
}

- (void) removeIndex: (unsigned int)value
{
  unsigned int pos;
  unsigned int i;

  if (_count == 0)
    return;
  pos = find_insert_position(self, value);
  if (pos >= _count || _indexes[pos] != value)
    return;
  for (i = pos; i + 1 < _count; i++)
    _indexes[i] = _indexes[i + 1];
  _count--;
}

- (void) removeIndexesInRange: (NSRange)range
{
  unsigned int i;
  unsigned int end;

  end = range.location + range.length;
  for (i = range.location; i < end; i++)
    [self removeIndex: i];
}

- (void) removeAllIndexes
{
  _count = 0;
}

@end

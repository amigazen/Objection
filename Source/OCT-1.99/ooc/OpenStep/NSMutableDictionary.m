/*
 * NSMutableDictionary.m -- OpenStep mutable dictionary implementation.
 */

#include <OpenStep/NSMutableDictionary.h>
#include <OpenStep/NSMutableArray.h>
#include <OpenStep/NSEnumerator.h>
#include <OpenStep/NSAllocator.h>
#include <objc/objc-runtime.h>
#include <objc/vectors.h>
#include <string.h>

@includes

#define MIN_CAPACITY 4
#define GROW(c) (((c) * 3) / 2 + 1)

/* Layout matching translator instance struct; avoid type NSMutableDictionary (class pointer name in generated C). */
struct _NSMutDictIvars { void *isa; void *next; id *_keys; id *_values; unsigned int _count; unsigned int _capacity; };

static unsigned int
_find_key_index(id self, id aKey)
{
  struct _NSMutDictIvars *d;
  unsigned int i;

  d = (struct _NSMutDictIvars *)self;
  for (i = 0; i < d->_count; i++)
    {
      if (d->_keys[i] == aKey)
        return i;
      if (d->_keys[i] != (id)0 && [d->_keys[i] isEqual: aKey])
        return i;
    }
  return d->_count;
}

@implementation NSMutableDictionary : NSDictionary

+ (id) dictionaryWithCapacity: (unsigned int)capacity
{
  return [[self alloc] initWithCapacity: capacity];
}

- (id) initWithCapacity: (unsigned int)capacity
{
  [super init];
  _count = 0;
  _capacity = capacity < MIN_CAPACITY ? MIN_CAPACITY : capacity;
  _keys = (id *)OPENSTEP_ALLOC((size_t)_capacity * sizeof(id));
  if (_keys == (id *)0)
    return (id)0;
  _values = (id *)OPENSTEP_ALLOC((size_t)_capacity * sizeof(id));
  if (_values == (id *)0)
    {
      OPENSTEP_FREE_VOID(_keys);
      _keys = (id *)0;
      return (id)0;
    }
  return self;
}

- (void) setObject: (id)object forKey: (id)aKey
{
  unsigned int idx;
  id *newKeys;
  id *newValues;
  unsigned int newcap;

  if (aKey == (id)0)
    return;
  idx = _find_key_index(self, aKey);
  if (idx < _count)
    {
      _values[idx] = object;
      return;
    }
  if (_count >= _capacity)
    {
      newcap = GROW(_capacity);
      newKeys = (id *)OPENSTEP_REALLOC_KNOWN(_keys, (size_t)_capacity * sizeof(id), (size_t)newcap * sizeof(id));
      if (newKeys == (id *)0)
        return;
      newValues = (id *)OPENSTEP_REALLOC_KNOWN(_values, (size_t)_capacity * sizeof(id), (size_t)newcap * sizeof(id));
      if (newValues == (id *)0)
        return;
      _keys = newKeys;
      _values = newValues;
      _capacity = newcap;
    }
  _keys[_count] = aKey;
  _values[_count] = object;
  _count++;
}

- (void) removeObjectForKey: (id)aKey
{
  unsigned int i;
  unsigned int idx;

  idx = _find_key_index(self, aKey);
  if (idx >= _count)
    return;
  for (i = idx; i + 1 < _count; i++)
    {
      _keys[i] = _keys[i + 1];
      _values[i] = _values[i + 1];
    }
  _count--;
}

- (void) removeAllObjects
{
  _count = 0;
}

@end

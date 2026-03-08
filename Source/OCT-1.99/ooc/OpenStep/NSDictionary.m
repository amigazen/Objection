/*
 * NSDictionary.m -- OpenStep immutable dictionary implementation.
 */

#include <OpenStep/NSDictionary.h>
#include <OpenStep/NSMutableArray.h>
#include <OpenStep/NSEnumerator.h>
#include <OpenStep/NSAllocator.h>
#include <objc/objc-runtime.h>
#include <objc/vectors.h>

@includes

#define MIN_CAPACITY 4

/* Layout matching translator-generated instance struct so static helpers need not use type NSDictionary (name clashes with class pointer in generated C). */
struct _NSDictIvars { void *isa; void *next; id *_keys; id *_values; unsigned int _count; unsigned int _capacity; };

static unsigned int
_hash_for_key(id key)
{
  if (key == (id)0)
    return 0;
  return (unsigned int)[key hash];
}

static int
_find_key(id self, id aKey, unsigned int *outIndex)
{
  struct _NSDictIvars *d;
  unsigned int i;
  unsigned int h;
  unsigned int n;

  d = (struct _NSDictIvars *)self;
  if (aKey == (id)0 || d->_count == 0)
    return 0;
  h = _hash_for_key(aKey);
  n = d->_capacity;
  for (i = 0; i < d->_count; i++)
    {
      if (d->_keys[i] == aKey)
        {
          *outIndex = i;
          return 1;
        }
      if ([d->_keys[i] isEqual: aKey])
        {
          *outIndex = i;
          return 1;
        }
    }
  return 0;
}

@implementation NSDictionary : NSObject

+ (id) dictionary
{
  id obj;
  obj = [self alloc];
  if (obj == (id)0)
    return (id)0;
  return [obj initWithObjects: (id *)0 forKeys: (id *)0 count: 0];
}

+ (id) dictionaryWithObject: (id)object forKey: (id)key
{
  id obj;
  id keys[1];
  id values[1];

  keys[0] = key;
  values[0] = object;
  obj = [self alloc];
  if (obj == (id)0)
    return (id)0;
  return [obj initWithObjects: values forKeys: keys count: 1];
}

+ (id) dictionaryWithObjects: (id *)objects forKeys: (id *)keys count: (unsigned int)count
{
  id obj;
  obj = [self alloc];
  if (obj == (id)0)
    return (id)0;
  return [obj initWithObjects: objects forKeys: keys count: count];
}

- (id) initWithObjects: (id *)objects forKeys: (id *)keys count: (unsigned int)count
{
  unsigned int i;
  unsigned int cap;

  _keys = (id *)0;
  _values = (id *)0;
  _count = 0;
  _capacity = 0;
  if (count == 0)
    return self;
  cap = count < MIN_CAPACITY ? MIN_CAPACITY : count;
  _keys = (id *)OPENSTEP_ALLOC((size_t)cap * sizeof(id));
  if (_keys == (id *)0)
    return (id)0;
  _values = (id *)OPENSTEP_ALLOC((size_t)cap * sizeof(id));
  if (_values == (id *)0)
    {
      OPENSTEP_FREE_VOID(_keys);
      _keys = (id *)0;
      return (id)0;
    }
  _capacity = cap;
  for (i = 0; i < count; i++)
    {
      _keys[i] = keys[i];
      _values[i] = objects[i];
    }
  _count = count;
  return self;
}

- (void) dealloc
{
  if (_keys != (id *)0)
    {
      OPENSTEP_FREE_VOID(_keys);
      _keys = (id *)0;
    }
  if (_values != (id *)0)
    {
      OPENSTEP_FREE_VOID(_values);
      _values = (id *)0;
    }
  _count = 0;
  _capacity = 0;
  [super dealloc];
}

- free
{
  if (_keys != (id *)0)
    {
      OPENSTEP_FREE_VOID(_keys);
      _keys = (id *)0;
    }
  if (_values != (id *)0)
    {
      OPENSTEP_FREE_VOID(_values);
      _values = (id *)0;
    }
  _count = 0;
  _capacity = 0;
  return [super free];
}

- (unsigned int) count
{
  return _count;
}

- (id) objectForKey: (id)aKey
{
  unsigned int idx;

  if (_find_key(self, aKey, &idx))
    return _values[idx];
  return (id)0;
}

- (id) allKeys
{
  id arr;
  unsigned int i;

  arr = [NSMutableArray arrayWithCapacity: _count];
  if (arr == (id)0)
    return (id)0;
  for (i = 0; i < _count; i++)
    [arr addObject: _keys[i]];
  return arr;
}

- (id) keyEnumerator
{
  id keysArray;

  keysArray = [self allKeys];
  if (keysArray == (id)0)
    return (id)0;
  return [[NSEnumerator alloc] initWithArray: keysArray];
}

- (id) description
{
  return (id)"<NSDictionary>";
}

@end

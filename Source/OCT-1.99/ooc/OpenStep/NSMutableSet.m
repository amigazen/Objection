/*
 * NSMutableSet.m -- OpenStep mutable set implementation.
 */

#include <OpenStep/NSMutableSet.h>
#include <OpenStep/NSAllocator.h>
#include <objc/objc-runtime.h>
#include <objc/vectors.h>

@includes

#define MIN_CAPACITY 4
#define GROW(c) (((c) * 3) / 2 + 1)

/* Layout matching translator instance struct; avoid type NSMutableSet (class pointer name in generated C). */
struct _NSMutSetIvars { void *isa; void *next; id *_objects; unsigned int _count; unsigned int _capacity; };

static int
_contains(id self, id anObject)
{
  struct _NSMutSetIvars *s;
  unsigned int i;

  if (anObject == (id)0)
    return 0;
  s = (struct _NSMutSetIvars *)self;
  for (i = 0; i < s->_count; i++)
    if (s->_objects[i] == anObject || [s->_objects[i] isEqual: anObject])
      return 1;
  return 0;
}

static unsigned int
_find_index(id self, id anObject)
{
  struct _NSMutSetIvars *s;
  unsigned int i;

  s = (struct _NSMutSetIvars *)self;
  for (i = 0; i < s->_count; i++)
    if (s->_objects[i] == anObject || [s->_objects[i] isEqual: anObject])
      return i;
  return s->_count;
}

@implementation NSMutableSet : NSSet

+ (id) setWithCapacity: (unsigned int)capacity
{
  return [[self alloc] initWithCapacity: capacity];
}

- (id) initWithCapacity: (unsigned int)capacity
{
  [super init];
  _count = 0;
  _capacity = capacity < MIN_CAPACITY ? MIN_CAPACITY : capacity;
  _objects = (id *)OPENSTEP_ALLOC((size_t)_capacity * sizeof(id));
  if (_objects == (id *)0)
    return (id)0;
  return self;
}

- (void) addObject: (id)anObject
{
  id *newp;
  unsigned int newcap;

  if (anObject == (id)0)
    return;
  if (_contains(self, anObject))
    return;
  if (_count >= _capacity)
    {
      newcap = GROW(_capacity);
      newp = (id *)OPENSTEP_REALLOC_KNOWN(_objects, (size_t)_capacity * sizeof(id), (size_t)newcap * sizeof(id));
      if (newp == (id *)0)
        return;
      _objects = newp;
      _capacity = newcap;
    }
  _objects[_count++] = anObject;
}

- (void) removeObject: (id)anObject
{
  unsigned int i;
  unsigned int idx;

  idx = _find_index(self, anObject);
  if (idx >= _count)
    return;
  for (i = idx; i + 1 < _count; i++)
    _objects[i] = _objects[i + 1];
  _count--;
}

- (void) removeAllObjects
{
  _count = 0;
}

@end

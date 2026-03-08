/*
 * NSMutableArray.m -- OpenStep mutable array implementation.
 */

#include <OpenStep/NSMutableArray.h>
#include <OpenStep/NSAllocator.h>
#include <objc/objc-runtime.h>
#include <objc/vectors.h>

#define MIN_CAPACITY 4
#define GROW(c) (((c) * 3) / 2 + 1)

@implementation NSMutableArray : NSArray

+ (id) arrayWithCapacity: (unsigned int)capacity
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

- (void) insertObject: (id)anObject atIndex: (unsigned int)index
{
  unsigned int i;
  id *newp;
  unsigned int newcap;

  if (anObject == (id)0)
    return;
  if (index > _count)
    index = _count;
  if (_count >= _capacity)
    {
      newcap = GROW(_capacity);
      newp = (id *)OPENSTEP_REALLOC_KNOWN(_objects, (size_t)_capacity * sizeof(id), (size_t)newcap * sizeof(id));
      if (newp == (id *)0)
        return;
      _objects = newp;
      _capacity = newcap;
    }
  for (i = _count; i > index; i--)
    _objects[i] = _objects[i - 1];
  _objects[index] = anObject;
  _count++;
}

- (void) removeObjectAtIndex: (unsigned int)index
{
  unsigned int i;

  if (index >= _count)
    return;
  for (i = index; i + 1 < _count; i++)
    _objects[i] = _objects[i + 1];
  _count--;
}

- (void) removeLastObject
{
  if (_count > 0)
    _count--;
}

- (void) removeAllObjects
{
  _count = 0;
}

- (void) replaceObjectAtIndex: (unsigned int)index withObject: (id)anObject
{
  if (index < _count)
    _objects[index] = anObject;
}

@end

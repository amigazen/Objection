/*
 * NSArray.m -- OpenStep immutable array implementation.
 */

#include <OpenStep/NSArray.h>
#include <OpenStep/NSAllocator.h>
#include <objc/objc-runtime.h>
#include <objc/vectors.h>
#include <stdio.h>
#include <string.h>

#ifndef NO_PROTOTYPE
# ifdef __STDC__
#  include <stdarg.h>
# else
#  include <varargs.h>
# endif
#endif

#define MIN_CAPACITY 4

@implementation NSArray : NSObject

+ (id) array
{
  return [[self alloc] initWithObjects: (id)0];
}

+ (id) arrayWithObject: (id)anObject
{
  return [[self alloc] initWithObjects: anObject, (id)0];
}

+ (id) arrayWithObject: (id)first withObject: (id)second
{
  id obj;
  id *store;

  obj = [self alloc];
  if (obj == (id)0)
    return (id)0;
  store = (id *)OPENSTEP_ALLOC(2 * sizeof(id));
  if (store == (id *)0)
    return (id)0;
  store[0] = first;
  store[1] = second;
  obj->_objects = store;
  obj->_count = 2;
  obj->_capacity = 2;
  return obj;
}

+ (id) arrayWithObjects: (id)firstObj, ...
{
  id obj;
  va_list ap;
  id item;
  unsigned int n;
  id *tmp;
  unsigned int cap;

  obj = [self alloc];
  if (obj == (id)0)
    return (id)0;
  obj->_objects = (id *)0;
  obj->_count = 0;
  obj->_capacity = 0;
  va_start(ap, firstObj);
  n = 0;
  item = firstObj;
  while (item != (id)0)
    {
      n++;
      item = va_arg(ap, id);
    }
  va_end(ap);
  if (n == 0)
    return obj;
  cap = n < MIN_CAPACITY ? MIN_CAPACITY : n;
  obj->_objects = (id *)OPENSTEP_ALLOC((size_t)cap * sizeof(id));
  if (obj->_objects == (id *)0)
    return (id)0;
  obj->_capacity = cap;
  va_start(ap, firstObj);
  item = firstObj;
  n = 0;
  while (item != (id)0)
    {
      obj->_objects[n++] = item;
      item = va_arg(ap, id);
    }
  va_end(ap);
  obj->_count = n;
  return obj;
}

- (id) initWithObjects: (id)firstObj, ...
{
  va_list ap;
  id item;
  unsigned int n;
  unsigned int cap;

  _objects = (id *)0;
  _count = 0;
  _capacity = 0;
  if (firstObj == (id)0)
    return self;
  va_start(ap, firstObj);
  n = 0;
  item = firstObj;
  while (item != (id)0)
    {
      n++;
      item = va_arg(ap, id);
    }
  va_end(ap);
  if (n == 0)
    return self;
  cap = n < MIN_CAPACITY ? MIN_CAPACITY : n;
  _objects = (id *)OPENSTEP_ALLOC((size_t)cap * sizeof(id));
  if (_objects == (id *)0)
    return (id)0;
  _capacity = cap;
  va_start(ap, firstObj);
  item = firstObj;
  n = 0;
  while (item != (id)0)
    {
      _objects[n++] = item;
      item = va_arg(ap, id);
    }
  va_end(ap);
  _count = n;
  return self;
}

- (void) dealloc
{
  if (_objects != (id *)0)
    {
      OPENSTEP_FREE_VOID(_objects);
      _objects = (id *)0;
    }
  _count = 0;
  _capacity = 0;
  [super dealloc];
}

- free
{
  if (_objects != (id *)0)
    {
      OPENSTEP_FREE_VOID(_objects);
      _objects = (id *)0;
    }
  _count = 0;
  _capacity = 0;
  return [super free];
}

- (unsigned int) count
{
  return _count;
}

- (id) objectAtIndex: (unsigned int)index
{
  if (index >= _count)
    return (id)0;
  return _objects[index];
}

- (id) firstObject
{
  if (_count == 0)
    return (id)0;
  return _objects[0];
}

- (id) lastObject
{
  if (_count == 0)
    return (id)0;
  return _objects[_count - 1];
}

- (BOOL) containsObject: (id)anObject
{
  unsigned int i;

  if (anObject == (id)0)
    return NO;
  for (i = 0; i < _count; i++)
    if (_objects[i] == anObject || (_objects[i] != (id)0 && [_objects[i] isEqual: anObject]))
      return YES;
  return NO;
}

- (id) description
{
  return (id)"<NSArray>";
}

@end

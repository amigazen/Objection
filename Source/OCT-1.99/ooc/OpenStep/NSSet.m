/*
 * NSSet.m -- OpenStep immutable set implementation.
 */

#include <OpenStep/NSSet.h>
#include <OpenStep/NSAllocator.h>
#include <objc/objc-runtime.h>
#include <objc/vectors.h>
#include <stdarg.h>

@includes

#define MIN_CAPACITY 4

/* Layout matching translator instance struct; avoid type NSSet (class pointer name in generated C). */
struct _NSSetIvars { void *isa; void *next; id *_objects; unsigned int _count; unsigned int _capacity; };

static int
_contains(id self, id anObject)
{
  struct _NSSetIvars *s;
  unsigned int i;

  if (anObject == (id)0)
    return 0;
  s = (struct _NSSetIvars *)self;
  for (i = 0; i < s->_count; i++)
    if (s->_objects[i] == anObject || [s->_objects[i] isEqual: anObject])
      return 1;
  return 0;
}

@implementation NSSet : NSObject

+ (id) set
{
  return [[self alloc] initWithObjects: (id)0];
}

+ (id) setWithObject: (id)anObject
{
  return [[self alloc] initWithObjects: anObject, (id)0];
}

+ (id) setWithObjects: (id)firstObj, ...
{
  id obj;
  va_list ap;
  id item;
  unsigned int n;
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
      if (_contains(obj, item) == 0)
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
      if (_contains(self, item) == 0)
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

- (id) member: (id)anObject
{
  unsigned int i;

  if (anObject == (id)0)
    return (id)0;
  for (i = 0; i < _count; i++)
    if (_objects[i] == anObject || [_objects[i] isEqual: anObject])
      return _objects[i];
  return (id)0;
}

- (BOOL) containsObject: (id)anObject
{
  return (BOOL)_contains(self, anObject);
}

- (id) description
{
  return (id)"<NSSet>";
}

@end

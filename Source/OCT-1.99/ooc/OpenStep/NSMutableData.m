/*
 * NSMutableData.m -- OpenStep mutable byte buffer implementation.
 */

#include <OpenStep/NSMutableData.h>
#include <OpenStep/NSAllocator.h>
#include <objc/objc-runtime.h>
#include <objc/vectors.h>
#include <string.h>

#define MIN_CAPACITY 16
#define GROW(s) (((s) * 3) / 2 + 1)

@implementation NSMutableData : NSData

+ (id) dataWithCapacity: (unsigned int)capacity
{
  return [[self alloc] initWithCapacity: capacity];
}

- (id) initWithCapacity: (unsigned int)capacity
{
  [super init];
  _length = 0;
  _owned = 1;
  _capacity = capacity < MIN_CAPACITY ? MIN_CAPACITY : capacity;
  _bytes = (const unsigned char *)OPENSTEP_ALLOC((size_t)_capacity);
  if (_bytes == (const unsigned char *)0)
    return (id)0;
  return self;
}

- (void *) mutableBytes
{
  return (void *)_bytes;
}

- (void) setLength: (unsigned int)length
{
  unsigned int cap;
  void *newp;

  if (length > _capacity)
    {
      cap = GROW(length);
      newp = OPENSTEP_REALLOC_KNOWN((void *)_bytes, _capacity, cap);
      if (newp == (void *)0)
        return;
      _bytes = (const unsigned char *)newp;
      _capacity = cap;
    }
  _length = length;
}

- (void) appendBytes: (const void *)bytes length: (unsigned int)length
{
  unsigned int newlen;
  unsigned int cap;
  void *newp;

  if (length == 0)
    return;
  newlen = _length + length;
  if (newlen > _capacity)
    {
      cap = GROW(newlen);
      newp = OPENSTEP_REALLOC_KNOWN((void *)_bytes, _capacity, cap);
      if (newp == (void *)0)
        return;
      _bytes = (const unsigned char *)newp;
      _capacity = cap;
    }
  OPENSTEP_COPY((void *)(_bytes + _length), bytes, (size_t)length);
  _length = newlen;
}

- (void) appendData: (id)other
{
  const void *p;
  unsigned int len;

  if (other == (id)0)
    return;
  if ([other isKindOf: [NSData class]] == NO)
    return;
  len = [other length];
  if (len == 0)
    return;
  p = [other bytes];
  if (p != (const void *)0)
    [self appendBytes: p length: len];
}

@end

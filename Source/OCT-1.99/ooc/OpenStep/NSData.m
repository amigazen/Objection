/*
 * NSData.m -- OpenStep immutable byte buffer implementation.
 */

#include <OpenStep/NSData.h>
#include <OpenStep/NSAllocator.h>
#include <objc/objc-runtime.h>
#include <objc/vectors.h>
#include <stdio.h>
#include <string.h>

@implementation NSData : NSObject

+ (id) data
{
  return [[self alloc] initWithBytes: (const void *)0 length: 0];
}

+ (id) dataWithBytes: (const void *)bytes length: (unsigned int)length
{
  return [[self alloc] initWithBytes: bytes length: length];
}

+ (id) dataWithBytesNoCopy: (void *)bytes length: (unsigned int)length
{
  return [[self alloc] initWithBytesNoCopy: bytes length: length];
}

- (id) initWithBytes: (const void *)bytes length: (unsigned int)length
{
  _owned = 1;
  _length = length;
  if (length == 0 || bytes == (const void *)0)
    {
      _bytes = (const unsigned char *)0;
      return self;
    }
  _bytes = (const unsigned char *)OPENSTEP_ALLOC((size_t)length);
  if (_bytes == (const unsigned char *)0)
    return (id)0;
  OPENSTEP_COPY((void *)_bytes, bytes, (size_t)length);
  return self;
}

- (id) initWithBytesNoCopy: (void *)bytes length: (unsigned int)length
{
  _owned = 0;
  _length = length;
  _bytes = (const unsigned char *)bytes;
  return self;
}

- (void) dealloc
{
  if (_owned && _bytes != (const unsigned char *)0)
    OPENSTEP_FREE_VOID((void *)_bytes);
  _bytes = (const unsigned char *)0;
  _length = 0;
  _owned = 0;
  [super dealloc];
}

- free
{
  if (_owned && _bytes != (const unsigned char *)0)
    OPENSTEP_FREE_VOID((void *)_bytes);
  _bytes = (const unsigned char *)0;
  _length = 0;
  _owned = 0;
  return [super free];
}

- (unsigned int) length
{
  return _length;
}

- (const void *) bytes
{
  return (const void *)_bytes;
}

- (id) description
{
  return (id)"<NSData>";
}

@end

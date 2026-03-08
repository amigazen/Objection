/*
 * NSValue.m -- OpenStep value (box for arbitrary bytes) implementation.
 */

#include <OpenStep/NSValue.h>
#include <OpenStep/NSAllocator.h>
#include <objc/objc-runtime.h>
#include <objc/vectors.h>
#include <stdio.h>
#include <string.h>

@implementation NSValue : NSObject

+ (id) valueWithBytes: (const void *)value objCType: (const char *)type
{
  return [[self alloc] initWithBytes: value objCType: type];
}

+ (id) value: (const void *)value withObjCType: (const char *)type
{
  return [self valueWithBytes: value objCType: type];
}

- (id) initWithBytes: (const void *)value objCType: (const char *)type
{
  unsigned int sz;

  _owned = 1;
  _objCType = type != (const char *)0 ? type : "";
  sz = 0;
  if (type != (const char *)0 && *type != '\0')
    {
      switch (*type)
        {
        case 'c': case 'C': sz = sizeof(char); break;
        case 's': case 'S': sz = sizeof(short); break;
        case 'i': case 'I': sz = sizeof(int); break;
        case 'l': case 'L': sz = sizeof(long); break;
        case 'f': sz = sizeof(float); break;
        case 'd': sz = sizeof(double); break;
        case '@': case '#': case ':': case '^': sz = sizeof(void *); break;
        default: sz = sizeof(void *); break;
        }
    }
  _size = sz;
  _data = (void *)0;
  if (sz > 0 && value != (const void *)0)
    {
      _data = OPENSTEP_ALLOC((size_t)sz);
      if (_data == (void *)0)
        return (id)0;
      OPENSTEP_COPY(_data, value, (size_t)sz);
    }
  return self;
}

- (void) dealloc
{
  if (_owned && _data != (void *)0)
    OPENSTEP_FREE_VOID(_data);
  _data = (void *)0;
  _size = 0;
  _objCType = (const char *)0;
  [super dealloc];
}

- free
{
  if (_owned && _data != (void *)0)
    OPENSTEP_FREE_VOID(_data);
  _data = (void *)0;
  _size = 0;
  _objCType = (const char *)0;
  return [super free];
}

- (void) getValue: (void *)value
{
  if (value != (void *)0 && _data != (void *)0 && _size > 0)
    OPENSTEP_COPY(value, _data, (size_t)_size);
}

- (const char *) objCType
{
  return _objCType != (const char *)0 ? _objCType : "";
}

- (id) description
{
  return (id)"<NSValue>";
}

@end

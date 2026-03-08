/*
 * NSString.m -- OpenStep immutable string implementation.
 */

#include <OpenStep/NSString.h>
#include <OpenStep/NSAllocator.h>
#include <OpenStep/NSRuntime.h>
#include <objc/objc-runtime.h>
#include <objc/vectors.h>

#ifndef NO_PROTOTYPE
# ifdef __STDC__
#  include <stdarg.h>
# else
#  include <varargs.h>
# endif
#endif

#define MIN_CAPACITY 16
#define GROW(s) (((s) * 3) / 2 + 1)

@implementation NSString : NSObject

+ (id) stringWithCString: (const char *)bytes
{
  return [[self alloc] initWithCString: bytes];
}

+ (id) stringWithCString: (const char *)bytes length: (unsigned int)length
{
  return [[self alloc] initWithCString: bytes length: length];
}

+ (id) stringWithFormat: (const char *)format, ...
{
  id obj;
  char buf[4096];
  int n;
  unsigned int len, cap;
  va_list ap;

  if (format == (const char *)0)
    format = "";
  va_start(ap, format);
  OpenStep_VSprintf(buf, format, (void *)ap);
  va_end(ap);
  n = (int)OpenStep_Strlen(buf);
  if (n < 0 || (unsigned int)n >= sizeof(buf))
    n = (int)sizeof(buf) - 1;
  buf[n] = '\0';
  len = (unsigned int)n;
  cap = len + 1;
  if (cap < MIN_CAPACITY)
    cap = MIN_CAPACITY;
  obj = [self alloc];
  if (obj == (id)0)
    return (id)0;
  obj->_bytes = (char *)OPENSTEP_ALLOC(cap);
  if (obj->_bytes == (char *)0)
    return (id)0;
  OPENSTEP_COPY(obj->_bytes, buf, (size_t)(len + 1));
  obj->_length = len;
  obj->_capacity = cap;
  return obj;
}

- (id) initWithCString: (const char *)bytes
{
  unsigned int len;
  unsigned int cap;

  if (bytes == (const char *)0)
    bytes = "";
  len = OpenStep_Strlen(bytes);
  cap = len + 1;
  if (cap < MIN_CAPACITY)
    cap = MIN_CAPACITY;
  _bytes = (char *)OPENSTEP_ALLOC(cap);
  if (_bytes == (char *)0)
    return (id)0;
  OPENSTEP_COPY(_bytes, bytes, (size_t)(len + 1));
  _length = len;
  _capacity = cap;
  return self;
}

- (id) initWithCString: (const char *)bytes length: (unsigned int)length
{
  unsigned int cap;

  if (bytes == (const char *)0)
    {
      _bytes = (char *)OPENSTEP_ALLOC(MIN_CAPACITY);
      if (_bytes == (char *)0)
        return (id)0;
      _bytes[0] = '\0';
      _length = 0;
      _capacity = MIN_CAPACITY;
      return self;
    }
  cap = length + 1;
  if (cap < MIN_CAPACITY)
    cap = MIN_CAPACITY;
  _bytes = (char *)OPENSTEP_ALLOC(cap);
  if (_bytes == (char *)0)
    return (id)0;
  if (length > 0)
    OPENSTEP_COPY(_bytes, bytes, (size_t)length);
  _bytes[length] = '\0';
  _length = length;
  _capacity = cap;
  return self;
}

- (void) dealloc
{
  if (_bytes != (char *)0)
    {
      OPENSTEP_FREE_VOID(_bytes);
      _bytes = (char *)0;
    }
  _length = 0;
  _capacity = 0;
  [super dealloc];
}

- free
{
  if (_bytes != (char *)0)
    {
      OPENSTEP_FREE_VOID(_bytes);
      _bytes = (char *)0;
    }
  _length = 0;
  _capacity = 0;
  return [super free];
}

- (unsigned int) length
{
  return _length;
}

- (const char *) cString
{
  return _bytes != (char *)0 ? (const char *)_bytes : "";
}

- (const char *) UTF8String
{
  return [self cString];
}

- (unsigned short) characterAtIndex: (unsigned int)index
{
  if (index >= _length || _bytes == (char *)0)
    return (unsigned short)0;
  return (unsigned short)(unsigned char)_bytes[index];
}

- (BOOL) isEqual: (id)anObject
{
  const char *s1;
  const char *s2;

  if (self == anObject)
    return YES;
  if (anObject == (id)0)
    return NO;
  if ([anObject isKindOf: [self class]] == NO)
    return NO;
  s1 = [self cString];
  s2 = [anObject cString];
  if (s1 == (const char *)0 && s2 == (const char *)0)
    return YES;
  if (s1 == (const char *)0 || s2 == (const char *)0)
    return NO;
  return (BOOL)(OpenStep_Strcmp(s1, s2) == 0);
}

- (BOOL) isEqualToString: (id)aString
{
  return [self isEqual: aString];
}

- (int) compare: (id)aString
{
  const char *s1;
  const char *s2;
  int r;

  if (aString == (id)0)
    return 1;
  s1 = [self cString];
  s2 = [aString cString];
  if (s1 == (const char *)0)
    s1 = "";
  if (s2 == (const char *)0)
    s2 = "";
  r = OpenStep_Strcmp(s1, s2);
  return r;
}

- (id) description
{
  return self;
}

@end

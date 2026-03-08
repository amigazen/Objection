/*
 * NSMutableString.m -- OpenStep mutable string implementation.
 * Amiga-only: NSAllocator, NSRuntime (no libc).
 */

#include <OpenStep/NSMutableString.h>
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

@implementation NSMutableString : NSString

+ (id) stringWithCapacity: (unsigned int)capacity
{
  return [[self alloc] initWithCapacity: capacity];
}

- (id) initWithCapacity: (unsigned int)capacity
{
  unsigned int cap;

  [super init];
  cap = capacity < MIN_CAPACITY ? MIN_CAPACITY : capacity;
  _bytes = (char *)OPENSTEP_ALLOC(cap);
  if (_bytes == (char *)0)
    return (id)0;
  _bytes[0] = '\0';
  _length = 0;
  _capacity = cap;
  return self;
}

- (void) setString: (id)aString
{
  const char *s;
  unsigned int len;
  unsigned int cap;
  char *newp;

  if (aString == (id)0)
    s = "";
  else
    s = [aString cString];
  len = OpenStep_Strlen(s);
  cap = len + 1;
  if (cap < MIN_CAPACITY)
    cap = MIN_CAPACITY;
  if (cap > _capacity)
    {
      newp = (char *)OPENSTEP_REALLOC_KNOWN(_bytes, _capacity, cap);
      if (newp == (char *)0)
        return;
      _bytes = newp;
      _capacity = cap;
    }
  OPENSTEP_COPY(_bytes, s, len + 1);
  _length = len;
}

- (void) appendString: (id)aString
{
  const char *s;
  unsigned int addLen;
  unsigned int newLen;
  unsigned int cap;
  char *newp;

  if (aString == (id)0)
    return;
  s = [aString cString];
  if (s == (const char *)0)
    return;
  addLen = OpenStep_Strlen(s);
  if (addLen == 0)
    return;
  newLen = _length + addLen;
  cap = newLen + 1;
  if (cap > _capacity)
    {
      cap = GROW(cap);
      if (cap < MIN_CAPACITY)
        cap = MIN_CAPACITY;
      newp = (char *)OPENSTEP_REALLOC_KNOWN(_bytes, _capacity, cap);
      if (newp == (char *)0)
        return;
      _bytes = newp;
      _capacity = cap;
    }
  OPENSTEP_COPY(_bytes + _length, s, addLen + 1);
  _length = newLen;
}

- (void) appendFormatCString: (const char *)format
{
  id str;

  if (format == (const char *)0)
    return;
  str = [[NSString alloc] initWithCString: format];
  if (str != (id)0)
    {
      [self appendString: str];
      [str free];
    }
}

- (void) insertString: (id)aString atIndex: (unsigned int)index
{
  const char *s;
  unsigned int addLen;
  unsigned int newLen;
  unsigned int cap;
  char *newp;

  if (aString == (id)0)
    return;
  if (index > _length)
    index = _length;
  s = [aString cString];
  if (s == (const char *)0)
    return;
  addLen = OpenStep_Strlen(s);
  if (addLen == 0)
    return;
  newLen = _length + addLen;
  cap = newLen + 1;
  if (cap > _capacity)
    {
      cap = GROW(cap);
      if (cap < MIN_CAPACITY)
        cap = MIN_CAPACITY;
      newp = (char *)OPENSTEP_REALLOC_KNOWN(_bytes, _capacity, cap);
      if (newp == (char *)0)
        return;
      _bytes = newp;
      _capacity = cap;
    }
  if (index < _length)
    OPENSTEP_COPY(_bytes + index + addLen, _bytes + index, _length - index);
  OPENSTEP_COPY(_bytes + index, s, addLen);
  _bytes[newLen] = '\0';
  _length = newLen;
}

- (void) deleteCharactersInRange: (NSRange)range
{
  unsigned int end;
  unsigned int tailLen;

  if (range.length == 0)
    return;
  if (range.location >= _length)
    return;
  end = range.location + range.length;
  if (end > _length)
    end = _length;
  tailLen = _length - end;
  if (tailLen > 0)
    OPENSTEP_COPY(_bytes + range.location, _bytes + end, tailLen);
  _length = range.location + tailLen;
  _bytes[_length] = '\0';
}

- (void) replaceCharactersInRange: (NSRange)range withString: (id)aString
{
  const char *s;
  unsigned int addLen;
  unsigned int end;
  unsigned int tailLen;
  unsigned int newLen;
  unsigned int cap;
  char *newp;
  unsigned int dest;
  unsigned int src;
  unsigned int i;

  end = range.location + range.length;
  if (end > _length)
    end = _length;
  if (aString == (id)0)
    s = "";
  else
    s = [aString cString];
  addLen = s != (const char *)0 ? OpenStep_Strlen(s) : 0;
  tailLen = _length - end;
  newLen = range.location + addLen + tailLen;
  cap = newLen + 1;
  if (cap > _capacity)
    {
      cap = GROW(cap);
      if (cap < MIN_CAPACITY)
        cap = MIN_CAPACITY;
      newp = (char *)OPENSTEP_REALLOC_KNOWN(_bytes, _capacity, cap);
      if (newp == (char *)0)
        return;
      _bytes = newp;
      _capacity = cap;
    }
  if (tailLen > 0)
    {
      dest = range.location + addLen;
      src = end;
      if (dest >= src)
        OPENSTEP_COPY(_bytes + dest, _bytes + src, tailLen);
      else
        for (i = tailLen; i > 0; i--)
          _bytes[dest + i - 1] = _bytes[src + i - 1];
    }
  if (addLen > 0 && s != (const char *)0)
    OPENSTEP_COPY(_bytes + range.location, s, addLen);
  _bytes[newLen] = '\0';
  _length = newLen;
}

- (void) setLength: (unsigned int)length
{
  unsigned int cap;
  char *newp;

  if (length > _capacity)
    {
      cap = GROW(length + 1);
      if (cap < MIN_CAPACITY)
        cap = MIN_CAPACITY;
      newp = (char *)OPENSTEP_REALLOC_KNOWN(_bytes, _capacity, cap);
      if (newp == (char *)0)
        return;
      _bytes = newp;
      _capacity = cap;
    }
  if (length > _length && _bytes != (char *)0)
    OPENSTEP_ZERO(_bytes + _length, length - _length);
  _length = length;
  if (_bytes != (char *)0)
    _bytes[_length] = '\0';
}

@end

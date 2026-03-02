/*
 * String.m -- String of characters for OCT.
 */

#include <objc/String.h>
#include <objc/objc-runtime.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef NO_PROTOTYPE
# ifdef __STDC__
#  include <stdarg.h>
# else
#  include <varargs.h>
# endif
#endif

#define MIN_SIZE  16
#define GROW(s)   (((s) * 3) / 2 + 1)

@includes()

/* Hash over string bytes (Set requirement). */
static unsigned
strhash(const char *s)
{
  unsigned h;

  if (s == (const char *)0)
    return 0;
  for (h = 0; *s; s++)
    h = (h * 31) + (unsigned char)*s;
  return h;
}

@implementation String : Object

/* -----------------------------------------------------------------------
 * Factory: str:, sprintf:
 * ----------------------------------------------------------------------- */

+ str: (STR) aStr
{
  id obj;
  unsigned len, sz;

  if (aStr == (STR)0)
    aStr = "";
  len = (unsigned int)strlen(aStr);
  sz = len + 1;
  if (sz < MIN_SIZE)
    sz = MIN_SIZE;
  obj = [self new];
  obj->string = (char *)malloc((size_t)sz);
  if (obj->string == (char *)0)
    return [self error: "String: malloc failed" vaDcl: 0];
  obj->length = len;
  obj->size = sz;
  strcpy(obj->string, aStr);
  return obj;
}

+ sprintf: (STR) aFormatStr vaDcl: firstArg
{
  id obj;
  char buf[4096];
  int n;
  unsigned len, sz;
  va_list ap;

  if (aFormatStr == (STR)0)
    aFormatStr = "";
  (void)firstArg;  /* used only to anchor va_list for variadic args */
  va_start(ap, firstArg);
  n = vsprintf(buf, aFormatStr, ap);
  va_end(ap);
  if (n < 0 || (unsigned)n >= sizeof buf)
    n = (int)sizeof buf - 1;
  buf[n] = '\0';
  if (n < 0 || (unsigned)n >= sizeof buf)
    n = (int)(sizeof buf - 1);
  buf[n] = '\0';
  len = (unsigned)n;
  sz = len + 1;
  if (sz < MIN_SIZE)
    sz = MIN_SIZE;
  obj = [self new];
  obj->string = (char *)malloc((size_t)sz);
  if (obj->string == (char *)0)
    return [self error: "String: sprintf malloc failed" vaDcl: 0];
  obj->length = len;
  obj->size = sz;
  strcpy(obj->string, buf);
  return obj;
}

/* -----------------------------------------------------------------------
 * free: release string buffer then super
 * ----------------------------------------------------------------------- */

- free
{
  if (string != (char *)0)
    {
      free((void *)string);
      string = (char *)0;
    }
  length = 0;
  size = 0;
  return [super free];
}

/* -----------------------------------------------------------------------
 * str, describe, str:
 * ----------------------------------------------------------------------- */

- (STR) str
{
  return string != (char *)0 ? string : (STR)"";
}

- (STR) describe
{
  return "@";
}

- str: (STR) aStr
{
  unsigned len, sz;

  if (aStr == (STR)0)
    aStr = "";
  len = (unsigned int)strlen(aStr);
  sz = len + 1;
  if (string != (char *)0 && size >= sz)
    {
      strcpy(string, aStr);
      length = len;
      return self;
    }
  if (string != (char *)0)
    free((void *)string);
  if (sz < MIN_SIZE)
    sz = MIN_SIZE;
  string = (char *)malloc((size_t)sz);
  if (string == (char *)0)
    return [self error: "String str: malloc failed" vaDcl: 0];
  size = sz;
  length = len;
  strcpy(string, aStr);
  return self;
}

/* -----------------------------------------------------------------------
 * charAt:, charAt:put: (bounds check)
 * ----------------------------------------------------------------------- */

- (char) charAt: (int) anOffset
{
  if (string == (char *)0 || anOffset < 0 || (unsigned)anOffset >= length)
    return (char)([self boundsViolation: anOffset], 0);
  return string[anOffset];
}

- (char) charAt: (int) anOffset put: (char) aChar
{
  char old;

  if (string == (char *)0 || anOffset < 0 || (unsigned)anOffset >= length)
    return (char)([self boundsViolation: anOffset], 0);
  old = string[anOffset];
  string[anOffset] = aChar;
  return old;
}

/* -----------------------------------------------------------------------
 * strcat: append receiver's C string to aBuf (caller ensures aBuf is large enough)
 * ----------------------------------------------------------------------- */

- (STR) strcat: (STR) aBuf
{
  if (string != (char *)0 && aBuf != (STR)0)
    strcat(aBuf, string);
  return aBuf;
}

/* -----------------------------------------------------------------------
 * asInt, asLong, asFloat
 * ----------------------------------------------------------------------- */

- (int) asInt
{
  return string != (char *)0 ? atoi(string) : 0;
}

- (long) asLong
{
  return string != (char *)0 ? atol(string) : 0L;
}

- (double) asFloat
{
  return string != (char *)0 ? atof(string) : 0.0;
}

/* -----------------------------------------------------------------------
 * compare, compareSTR, isEqual, isEqualSTR, hash (Set requirement)
 * ----------------------------------------------------------------------- */

- (int) compare: anObject
{
  return anObject != nil ? strcmp(string != (char *)0 ? string : "", [anObject str]) : 1;
}

- (int) compareSTR: (STR) aCString
{
  return strcmp(string != (char *)0 ? string : "", aCString != (STR)0 ? aCString : "");
}

- (BOOL) isEqual: anObject
{
  return (BOOL)(anObject != nil && strcmp(string != (char *)0 ? string : "", [anObject str]) == 0);
}

- (BOOL) isEqualSTR: (STR) aStr
{
  return (BOOL)(strcmp(string != (char *)0 ? string : "", aStr != (STR)0 ? aStr : "") == 0);
}

- (unsigned) hash
{
  return strhash(string);
}

- (BOOL) isCopyOf: anObject
{
  return (BOOL)(anObject != nil && [anObject isMemberOf: [self class]] && [self isEqual: anObject]);
}

/* -----------------------------------------------------------------------
 * concat:, concatSTR:
 * ----------------------------------------------------------------------- */

- concat: anObject
{
  STR other;

  if (anObject == nil)
    return self;
  other = [anObject str];
  return other != (STR)0 ? [self concatSTR: other] : self;
}

- concatSTR: (STR) aCString
{
  unsigned addlen, newlen, newsz;
  char *p;

  if (aCString == (STR)0)
    return self;
  addlen = (unsigned int)strlen(aCString);
  if (addlen == 0)
    return self;
  newlen = length + addlen;
  newsz = newlen + 1;
  if (string == (char *)0)
    {
      string = (char *)malloc((size_t)(newsz < MIN_SIZE ? MIN_SIZE : newsz));
      if (string == (char *)0)
	return [self error: "String concatSTR: malloc failed" vaDcl: 0];
      size = (unsigned)(newsz < MIN_SIZE ? MIN_SIZE : newsz);
      length = 0;
      string[0] = '\0';
    }
  else if (size < newsz)
    {
      newsz = GROW(newsz);
      p = (char *)realloc((void *)string, (size_t)newsz);
      if (p == (char *)0)
	return [self error: "String concatSTR: realloc failed" vaDcl: 0];
      string = p;
      size = newsz;
    }
  strcat(string, aCString);
  length = newlen;
  return self;
}

/* -----------------------------------------------------------------------
 * boundsViolation, sort
 * ----------------------------------------------------------------------- */

- boundsViolation: (int) anOffset
{
  return [self error: "String bounds violation: offset %d (length=%u)" vaDcl: anOffset, length];
}

- sort
{
  return self;
}

@end

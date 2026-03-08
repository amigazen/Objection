/*
 * NSException.m -- OpenStep exception implementation.
 * Amiga-only: raise uses OpenStep_Printf and exit.
 */

#include <OpenStep/NSException.h>
#include <OpenStep/NSString.h>
#include <OpenStep/NSRuntime.h>
#include <objc/objc-runtime.h>
#include <objc/vectors.h>

/* Avoid stdlib.h: SAS/C stdlib.h uses __attribute__((__stkparm__)) which triggers Error 90. */
extern void exit(int);

@implementation NSException : NSObject

+ (id) exceptionWithName: (id)name reason: (id)reason userInfo: (id)userInfo
{
  return [[self alloc] initWithName: name reason: reason userInfo: userInfo];
}

- (id) initWithName: (id)name reason: (id)reason userInfo: (id)userInfo
{
  [super init];
  _name = name;
  _reason = reason;
  _userInfo = userInfo;
  return self;
}

- (void) dealloc
{
  _name = (id)0;
  _reason = (id)0;
  _userInfo = (id)0;
  [super dealloc];
}

- free
{
  _name = (id)0;
  _reason = (id)0;
  _userInfo = (id)0;
  return [super free];
}

- (id) name
{
  return _name;
}

- (id) reason
{
  return _reason;
}

- (id) userInfo
{
  return _userInfo;
}

- (void) raise
{
  const char *n;
  const char *r;

  n = _name != (id)0 && [_name respondsTo: @selector(cString)] ? [_name cString] : "NSException";
  r = _reason != (id)0 && [_reason respondsTo: @selector(cString)] ? [_reason cString] : "";
  OpenStep_Printf("NSException: %s - %s\n", n, r);
  exit(1);
}

@end

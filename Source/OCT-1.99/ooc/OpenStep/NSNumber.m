/*
 * NSNumber.m -- OpenStep number (box for scalar) implementation.
 */

#include <OpenStep/NSNumber.h>
#include <OpenStep/NSRuntime.h>
#include <objc/objc-runtime.h>
#include <objc/vectors.h>

@implementation NSNumber : NSValue

+ (id) numberWithChar: (char)value
{
  return [[self alloc] initWithBytes: (const void *)&value objCType: "c"];
}

+ (id) numberWithShort: (short)value
{
  return [[self alloc] initWithBytes: (const void *)&value objCType: "s"];
}

+ (id) numberWithInt: (int)value
{
  return [[self alloc] initWithInt: value];
}

+ (id) numberWithLong: (long)value
{
  return [[self alloc] initWithLong: value];
}

+ (id) numberWithFloat: (float)value
{
  return [[self alloc] initWithBytes: (const void *)&value objCType: "f"];
}

+ (id) numberWithDouble: (double)value
{
  return [[self alloc] initWithDouble: value];
}

- (id) initWithInt: (int)value
{
  return [self initWithBytes: (const void *)&value objCType: "i"];
}

- (id) initWithLong: (long)value
{
  return [self initWithBytes: (const void *)&value objCType: "l"];
}

- (id) initWithDouble: (double)value
{
  return [self initWithBytes: (const void *)&value objCType: "d"];
}

- (char) charValue
{
  char v;
  [self getValue: (void *)&v];
  return v;
}

- (short) shortValue
{
  short v;
  [self getValue: (void *)&v];
  return v;
}

- (int) intValue
{
  int v;
  [self getValue: (void *)&v];
  return v;
}

- (long) longValue
{
  long v;
  [self getValue: (void *)&v];
  return v;
}

- (float) floatValue
{
  float v;
  [self getValue: (void *)&v];
  return v;
}

- (double) doubleValue
{
  double v;
  [self getValue: (void *)&v];
  return v;
}

- (BOOL) isEqual: (id)anObject
{
  double d1;
  double d2;

  if (self == anObject)
    return YES;
  if (anObject == (id)0)
    return NO;
  if ([anObject isKindOf: [self class]] == NO)
    return NO;
  d1 = [self doubleValue];
  d2 = [anObject doubleValue];
  return (BOOL)(d1 == d2);
}

/* OCT translator fails on [NSString ...] and [[NSString alloc] initWithCString:] in this file; return self until fixed. */
- (id) description
{
  return self;
}

@end

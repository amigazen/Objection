/*
 * NSDate.m -- OpenStep date (time interval since reference) implementation.
 */

#include <OpenStep/NSDate.h>
#include <OpenStep/NSString.h>
#include <OpenStep/NSRuntime.h>
#include <objc/objc-runtime.h>
#include <objc/vectors.h>

@implementation NSDate : NSObject

+ (id) date
{
  return [[self alloc] initWithTimeIntervalSinceReferenceDate: 0.0];
}

+ (id) dateWithTimeIntervalSinceReferenceDate: (double)seconds
{
  return [[self alloc] initWithTimeIntervalSinceReferenceDate: seconds];
}

- (id) initWithTimeIntervalSinceReferenceDate: (double)seconds
{
  _timeIntervalSinceReferenceDate = seconds;
  return self;
}

- (double) timeIntervalSinceReferenceDate
{
  return _timeIntervalSinceReferenceDate;
}

/* OCT translator fails on [NSString ...] here; return self until fixed. */
- (id) description
{
  return self;
}

@end

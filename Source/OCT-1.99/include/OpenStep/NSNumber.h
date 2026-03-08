/*
 * NSNumber.h -- OpenStep number (box for scalar) interface.
 */

#ifndef _OPENSTEP_NSNUMBER_H
# define _OPENSTEP_NSNUMBER_H 1

# include <OpenStep/NSValue.h>

@interface NSNumber : NSValue

+ (id) numberWithChar: (char)value;
+ (id) numberWithShort: (short)value;
+ (id) numberWithInt: (int)value;
+ (id) numberWithLong: (long)value;
+ (id) numberWithFloat: (float)value;
+ (id) numberWithDouble: (double)value;

- (id) initWithInt: (int)value;
- (id) initWithLong: (long)value;
- (id) initWithDouble: (double)value;

- (char) charValue;
- (short) shortValue;
- (int) intValue;
- (long) longValue;
- (float) floatValue;
- (double) doubleValue;

- (BOOL) isEqual: (id)anObject;
- (id) description;

@end

#endif

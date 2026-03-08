/*
 * NSDate.h -- OpenStep date (time interval) interface.
 * Time interval since reference date (1 Jan 2001 00:00:00 GMT).
 */

#ifndef _OPENSTEP_NSDATE_H
# define _OPENSTEP_NSDATE_H 1

# include <OpenStep/NSObject.h>

@interface NSDate : NSObject
{
  double _timeIntervalSinceReferenceDate;
}

+ (id) date;
+ (id) dateWithTimeIntervalSinceReferenceDate: (double)seconds;

- (id) initWithTimeIntervalSinceReferenceDate: (double)seconds;
- (double) timeIntervalSinceReferenceDate;

- (id) description;

@end

#endif

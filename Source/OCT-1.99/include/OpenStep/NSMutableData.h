/*
 * NSMutableData.h -- OpenStep mutable byte buffer interface.
 */

#ifndef _OPENSTEP_NSMUTABLEDATA_H
# define _OPENSTEP_NSMUTABLEDATA_H 1

# include <OpenStep/NSData.h>

@interface NSMutableData : NSData
{
  unsigned int _capacity;
}

+ (id) dataWithCapacity: (unsigned int)capacity;

- (id) initWithCapacity: (unsigned int)capacity;

- (void *) mutableBytes;
- (void) setLength: (unsigned int)length;
- (void) appendBytes: (const void *)bytes length: (unsigned int)length;
- (void) appendData: (id)other;

@end

#endif

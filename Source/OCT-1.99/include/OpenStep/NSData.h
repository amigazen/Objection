/*
 * NSData.h -- OpenStep immutable byte buffer interface.
 */

#ifndef _OPENSTEP_NSDATA_H
# define _OPENSTEP_NSDATA_H 1

# include <OpenStep/NSObject.h>

@interface NSData : NSObject
{
  const unsigned char *_bytes;
  unsigned int         _length;
  unsigned int         _owned;
}

+ (id) data;
+ (id) dataWithBytes: (const void *)bytes length: (unsigned int)length;
+ (id) dataWithBytesNoCopy: (void *)bytes length: (unsigned int)length;

- (id) initWithBytes: (const void *)bytes length: (unsigned int)length;
- (id) initWithBytesNoCopy: (void *)bytes length: (unsigned int)length;
- (void) dealloc;

- (unsigned int) length;
- (const void *) bytes;

- (id) description;

@end

#endif

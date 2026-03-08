/*
 * NSString.h -- OpenStep immutable string interface.
 */

#ifndef _OPENSTEP_NSSTRING_H
# define _OPENSTEP_NSSTRING_H 1

# include <OpenStep/NSObject.h>

@interface NSString : NSObject
{
  char           *_bytes;
  unsigned int    _length;
  unsigned int    _capacity;
}

+ (id) stringWithCString: (const char *)bytes;
+ (id) stringWithCString: (const char *)bytes length: (unsigned int)length;
+ (id) stringWithFormat: (const char *)format, ...;

- (id) initWithCString: (const char *)bytes;
- (id) initWithCString: (const char *)bytes length: (unsigned int)length;
- (void) dealloc;

- (unsigned int) length;
- (const char *) cString;
- (const char *) UTF8String;
- (unsigned short) characterAtIndex: (unsigned int)index;

- (BOOL) isEqual: (id)anObject;
- (BOOL) isEqualToString: (id)aString;
- (int) compare: (id)aString;
- (id) description;

@end

#endif

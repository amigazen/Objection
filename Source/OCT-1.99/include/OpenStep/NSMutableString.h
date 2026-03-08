/*
 * NSMutableString.h -- OpenStep mutable string interface.
 */

#ifndef _OPENSTEP_NSMUTABLESTRING_H
# define _OPENSTEP_NSMUTABLESTRING_H 1

# include <OpenStep/NSString.h>
# include <OpenStep/NSRange.h>

@interface NSMutableString : NSString

+ (id) stringWithCapacity: (unsigned int)capacity;

- (id) initWithCapacity: (unsigned int)capacity;

- (void) setString: (id)aString;
- (void) appendString: (id)aString;
/* appendFormat: with ... not supported by OCT translator; use appendFormatCString: for single string. */
- (void) appendFormatCString: (const char *)format;
- (void) insertString: (id)aString atIndex: (unsigned int)index;
- (void) deleteCharactersInRange: (NSRange)range;
- (void) replaceCharactersInRange: (NSRange)range withString: (id)aString;
- (void) setLength: (unsigned int)length;

@end

#endif

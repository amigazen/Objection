/*
 * NSException.h -- OpenStep exception interface.
 */

#ifndef _OPENSTEP_NSEXCEPTION_H
# define _OPENSTEP_NSEXCEPTION_H 1

# include <OpenStep/NSObject.h>

@interface NSException : NSObject
{
  id _name;
  id _reason;
  id _userInfo;
}

+ (id) exceptionWithName: (id)name reason: (id)reason userInfo: (id)userInfo;

- (id) initWithName: (id)name reason: (id)reason userInfo: (id)userInfo;
- (void) dealloc;

- (id) name;
- (id) reason;
- (id) userInfo;
- (void) raise;

@end

#endif

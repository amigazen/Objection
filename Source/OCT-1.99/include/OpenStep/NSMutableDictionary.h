/*
 * NSMutableDictionary.h -- OpenStep mutable dictionary interface.
 */

#ifndef _OPENSTEP_NSMUTABLEDICTIONARY_H
# define _OPENSTEP_NSMUTABLEDICTIONARY_H 1

# include <OpenStep/NSDictionary.h>

@interface NSMutableDictionary : NSDictionary

+ (id) dictionaryWithCapacity: (unsigned int)capacity;

- (id) initWithCapacity: (unsigned int)capacity;

- (void) setObject: (id)object forKey: (id)aKey;
- (void) removeObjectForKey: (id)aKey;
- (void) removeAllObjects;

@end

#endif

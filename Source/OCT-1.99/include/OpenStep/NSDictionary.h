/*
 * NSDictionary.h -- OpenStep immutable dictionary (key-value) interface.
 * Keys and values are objects; keys must support copy and hash.
 */

#ifndef _OPENSTEP_NSDICTIONARY_H
# define _OPENSTEP_NSDICTIONARY_H 1

# include <OpenStep/NSObject.h>

@interface NSDictionary : NSObject
{
  id    *_keys;
  id    *_values;
  unsigned int _count;
  unsigned int _capacity;
}

+ (id) dictionary;
+ (id) dictionaryWithObject: (id)object forKey: (id)key;
+ (id) dictionaryWithObjects: (id *)objects forKeys: (id *)keys count: (unsigned int)count;

- (id) initWithObjects: (id *)objects forKeys: (id *)keys count: (unsigned int)count;
- (void) dealloc;

- (unsigned int) count;
- (id) objectForKey: (id)aKey;
- (id) allKeys;
- (id) keyEnumerator;

- (id) description;

@end

#endif

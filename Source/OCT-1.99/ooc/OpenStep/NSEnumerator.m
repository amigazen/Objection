/*
 * NSEnumerator.m -- OpenStep enumerator implementation.
 */

#include <OpenStep/NSEnumerator.h>
#include <OpenStep/NSArray.h>
#include <objc/objc-runtime.h>
#include <objc/vectors.h>

@implementation NSEnumerator : NSObject

- (id) initWithArray: (id)array
{
  [super init];
  _collection = array;
  _index = 0;
  return self;
}

- (id) nextObject
{
  id obj;

  if (_collection == (id)0)
    return (id)0;
  if ([_collection isKindOf: [NSArray class]] == NO)
    return (id)0;
  if (_index >= [_collection count])
    return (id)0;
  obj = [_collection objectAtIndex: _index];
  _index++;
  return obj;
}

- (void) dealloc
{
  if (_collection != (id)0)
    [_collection free];
  _collection = (id)0;
  [super dealloc];
}

- free
{
  if (_collection != (id)0)
    {
      [_collection free];
      _collection = (id)0;
    }
  return [super free];
}

@end

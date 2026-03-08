/*
 * NSValue.h -- OpenStep value (box) interface.
 */

#ifndef _OPENSTEP_NSVALUE_H
# define _OPENSTEP_NSVALUE_H 1

# include <OpenStep/NSObject.h>

@interface NSValue : NSObject
{
  void         *_data;
  unsigned int  _size;
  const char   *_objCType;
  unsigned int  _owned;
}

+ (id) valueWithBytes: (const void *)value objCType: (const char *)type;
+ (id) value: (const void *)value withObjCType: (const char *)type;

- (id) initWithBytes: (const void *)value objCType: (const char *)type;
- (void) dealloc;

- (void) getValue: (void *)value;
- (const char *) objCType;

- (id) description;

@end

#endif

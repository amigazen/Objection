/*
 * Collection.m -- Abstract collection (container) class for OCT.
 *
 * Enumeration: eachElement replies a Sequence; caller frees it. Conversion
 * methods create a new collection of the target type and add the receiver's
 * members.
 */

#include <collect/Collection.h>
#include <collect/Sequence.h>

extern id Set;
extern id OrderedCollection;

@implementation Collection : Object

/* -----------------------------------------------------------------------
 * Factory: instance creation
 * ----------------------------------------------------------------------- */

+ new: (int) size
{
  id obj;

  obj = [self new];
  obj->contents = [IdArray new: size];
  return obj;
}

/* New instance whose contents are initialized from nArgs following arguments. */
+ with: (int) nArgs vaDcl: args
{
  id newObj;
  id *p = &args;

  newObj = [self new: nArgs];
  while (nArgs--)
    [newObj add: (*p++)];
  return newObj;
}

+ initialize
{
  if (self != &instCollection)
    return [instCollection.super_class initialize];
  if (!CLS_GETINFO(self, CLS_INITIALIZED))
    {
      CLS_SETINFO(self, CLS_INITIALIZED);
      [self->super_class initialize];
      [IdArray initialize];
    }
  return (id)self;
}

/* -----------------------------------------------------------------------
 * Memory management
 * ----------------------------------------------------------------------- */

- free
{
  if (contents != nil && contents->isa != Nil)
    [contents free];
  return [super free];
}

- freeContents
{
  if (contents != nil && contents->isa != Nil)
    [contents freeContents];
  return self;
}

/* Subclass overrides when storage is not IdArray-based. */
- expand
{
  return [self error: "Can't add to nil contents" vaDcl: 0];
}

/* -----------------------------------------------------------------------
 * Adding (add: is subclass responsibility)
 * ----------------------------------------------------------------------- */

- add: newObject
{
  return [self subClassResponsibility: _cmd];
}

- addContentsTo: aCollection
{
  if (contents == nil)
    [self expand];
  [contents addContentsTo: aCollection];
  return aCollection;
}

- addContentsOf: aCollection
{
  [aCollection addContentsTo: self];
  return self;
}

/* -----------------------------------------------------------------------
 * Removing (remove: is subclass responsibility)
 * ----------------------------------------------------------------------- */

- remove: anObject
{
  return [self subClassResponsibility: _cmd];
}

- removeContentsOf: aCollection
{
  if (contents != nil)
    [contents removeContentsOf: aCollection];
  return self;
}

/* -----------------------------------------------------------------------
 * Enumeration: Sequence over receiver's members; caller frees sequence
 * ----------------------------------------------------------------------- */

- eachElement
{
  if (contents != nil)
    return [Sequence array: contents];
  return [Sequence new: 1];
}

- eachElementPerform: (SEL) aSelector
{
  if (contents != nil)
    [contents eachElementPerform: aSelector];
  return self;
}

- eachElementPerform: (SEL) aSelector with: arg1
{
  if (contents != nil)
    [contents eachElementPerform: aSelector with: arg1 with: nil];
  return self;
}

- eachElementPerform: (SEL) aSelector with: arg1 with: arg2
{
  if (contents != nil)
    [contents eachElementPerform: aSelector with: arg1 with: arg2];
  return self;
}

/* -----------------------------------------------------------------------
 * Converting between collection types (asSet, asOrdCltn)
 * ----------------------------------------------------------------------- */

- asSet
{
  int size = ([self count] + 4) / 3;
  if (size < 1)
    size = 1;
  return [self addContentsTo: [Set new: size]];
}

- asOrderedCollection
{
  return [self addContentsTo: [OrderedCollection new: [self count]]];
}

/* -----------------------------------------------------------------------
 * Testing
 * ----------------------------------------------------------------------- */

- (int) count
{
  return contents != nil ? [contents count] : 0;
}

- (int) capacity
{
  return contents != nil ? [contents capacity] : 0;
}

- (BOOL) isEmpty
{
  return (BOOL)([self count] == 0);
}

- (BOOL) contains: anObject
{
  return (BOOL)([self find: anObject] != nil);
}

- (BOOL) isCopyOf: anObject
{
  if (anObject == nil || ![anObject isMemberOf: [self class]])
    return NO;
  return (BOOL)(contents != nil && anObject->contents != nil &&
      [anObject->contents isCopyOf: contents]);
}

- (unsigned) hash
{
  int cap, cnt, spice;

  cap = [self capacity];
  cnt = [self count];
  spice = (int)(PTR2INT(self) >> 3);
  return (unsigned)((~(unsigned)cnt) ^ ((unsigned)cap * (unsigned)cnt) ^ (unsigned)spice) >> 1;
}

/* -----------------------------------------------------------------------
 * Copy (receiver's structure; contents is copied)
 * ----------------------------------------------------------------------- */

- copy
{
  id guy = [super copy];

  guy->contents = contents != nil ? [contents copy] : nil;
  return guy;
}

/* -----------------------------------------------------------------------
 * Searching and offsets (delegate to contents IdArray)
 * ----------------------------------------------------------------------- */

- find: anObject
{
  int there;

  if (contents != nil && (there = [self offsetOf: anObject]) >= 0)
    return [contents at: there];
  return nil;
}

- findMatching: anObject
{
  int there;

  if (contents != nil && (there = [self offsetMatching: anObject]) >= 0)
    return [contents at: there];
  return nil;
}

- findSTR: (STR) aString
{
  int there;

  if (contents != nil && (there = [self offsetSTR: aString]) >= 0)
    return [contents at: there];
  return nil;
}

- (int) offsetOf: anObject
{
  return contents != nil ? [contents offsetOf: anObject] : -1;
}

- (int) offsetMatching: anObject
{
  return contents != nil ? [contents offsetMatching: anObject] : -1;
}

- (int) offsetSTR: (STR) aString
{
  return contents != nil ? [contents offsetSTR: aString] : -1;
}

@end

/*
 * List.m -- Array-based list of objects for OCT.
 */

#include <objc/List.h>
#include <objc/objc-runtime.h>
#include <stdlib.h>
#include <string.h>

#define MIN_CAPACITY  16
#define GROW(n)       (((n) * 3) / 2 + 1)

@implementation List : Object

/* -----------------------------------------------------------------------
 * Factory
 * ----------------------------------------------------------------------- */

+ new
{
  return [self newCount: MIN_CAPACITY];
}

+ newCount: (unsigned) numSlots
{
  id self_obj;
  unsigned n;

  if (numSlots < 1)
    numSlots = 1;
  self_obj = [super new];
  self_obj->dataPtr = (id *)malloc((size_t)numSlots * sizeof(id));
  if (self_obj->dataPtr == (id *)0)
    return (id)0;
  n = numSlots;
  self_obj->numElements = 0;
  self_obj->maxElements = n;
  memset(self_obj->dataPtr, 0, (size_t)n * sizeof(id));
  return self_obj;
}

/* -----------------------------------------------------------------------
 * free, freeObjects
 * ----------------------------------------------------------------------- */

- free
{
  if (dataPtr != (id *)0)
    {
      free((void *)dataPtr);
      dataPtr = (id *)0;
    }
  numElements = 0;
  maxElements = 0;
  return [super free];
}

- freeObjects
{
  unsigned i;

  if (dataPtr == (id *)0)
    return self;
  for (i = 0; i < numElements; i++)
    if (dataPtr[i] != nil)
      [dataPtr[i] free];
  return self;
}

/* -----------------------------------------------------------------------
 * copy, isEqual
 * ----------------------------------------------------------------------- */

- copy
{
  id copy;
  unsigned n;

  copy = [super copy];
  if (copy == nil || maxElements == 0)
    return copy;
  copy->dataPtr = (id *)malloc((size_t)maxElements * sizeof(id));
  if (copy->dataPtr == (id *)0)
    return copy;
  n = numElements;
  copy->numElements = n;
  copy->maxElements = maxElements;
  memcpy(copy->dataPtr, dataPtr, (size_t)n * sizeof(id));
  return copy;
}

- (BOOL) isEqual: otherList
{
  unsigned i;

  if (self == otherList)
    return YES;
  if (otherList == nil || [otherList class] != [self class])
    return NO;
  if ((unsigned)[otherList count] != numElements)
    return NO;
  for (i = 0; i < numElements; i++)
    if (dataPtr[i] != [otherList objectAt: i])
      return NO;
  return YES;
}

/* -----------------------------------------------------------------------
 * capacity, setAvailableCapacity, count
 * ----------------------------------------------------------------------- */

- (int) capacity
{
  return (int)maxElements;
}

- setAvailableCapacity: (unsigned) numSlots
{
  id *p;

  if (numSlots <= maxElements)
    return self;
  p = (id *)realloc((void *)dataPtr, (size_t)numSlots * sizeof(id));
  if (p == (id *)0)
    return self;
  dataPtr = p;
  memset(dataPtr + maxElements, 0, (size_t)(numSlots - maxElements) * sizeof(id));
  maxElements = numSlots;
  return self;
}

- (int) count
{
  return (int)numElements;
}

/* -----------------------------------------------------------------------
 * ensure capacity for one more element
 * ----------------------------------------------------------------------- */

- _growIfNeeded
{
  if (numElements >= maxElements)
    [self setAvailableCapacity: GROW(maxElements)];
  return self;
}

/* -----------------------------------------------------------------------
 * objectAt:, indexOf:, lastObject
 * ----------------------------------------------------------------------- */

- objectAt: (unsigned) index
{
  if (index >= numElements)
    return nil;
  return dataPtr[index];
}

- (unsigned) indexOf: matchObject
{
  unsigned i;

  if (matchObject == nil || dataPtr == (id *)0)
    return NX_NOT_IN_LIST;
  for (i = 0; i < numElements; i++)
    if (dataPtr[i] == matchObject)
      return i;
  return NX_NOT_IN_LIST;
}

- lastObject
{
  if (numElements == 0)
    return nil;
  return dataPtr[numElements - 1];
}

- first
{
  return [self objectAt: 0];
}

/* -----------------------------------------------------------------------
 * addObject:, addObjectIfAbsent:, insertObject: at:
 * ----------------------------------------------------------------------- */

- addObject: newElement
{
  if (newElement == nil)
    return self;
  [self _growIfNeeded];
  dataPtr[numElements++] = newElement;
  return self;
}

- addObjectIfAbsent: testObject
{
  if (testObject == nil)
    return self;
  if ([self indexOf: testObject] != NX_NOT_IN_LIST)
    return self;
  return [self addObject: testObject];
}

- insertObject: newElement at: (unsigned) index
{
  unsigned i;

  if (newElement == nil)
    return self;
  if (index > numElements)
    index = numElements;
  [self _growIfNeeded];
  for (i = numElements; i > index; i--)
    dataPtr[i] = dataPtr[i - 1];
  dataPtr[index] = newElement;
  numElements++;
  return self;
}

/* -----------------------------------------------------------------------
 * removeObject:, removeObjectAt:, removeLastObject
 * ----------------------------------------------------------------------- */

- removeObject: targetObj
{
  unsigned i;

  if (targetObj == nil || dataPtr == (id *)0)
    return nil;
  for (i = 0; i < numElements; i++)
    if (dataPtr[i] == targetObj)
      {
        for (; i + 1 < numElements; i++)
          dataPtr[i] = dataPtr[i + 1];
        numElements--;
        return targetObj;
      }
  return nil;
}

- removeObjectAt: (unsigned) index
{
  id old;
  unsigned i;

  if (index >= numElements)
    return nil;
  old = dataPtr[index];
  for (i = index; i + 1 < numElements; i++)
    dataPtr[i] = dataPtr[i + 1];
  numElements--;
  return old;
}

- removeLastObject
{
  if (numElements == 0)
    return nil;
  return [self removeObjectAt: numElements - 1];
}

/* -----------------------------------------------------------------------
 * empty, replaceObject:, replaceObjectAt: with:
 * ----------------------------------------------------------------------- */

- empty
{
  numElements = 0;
  return self;
}

- replaceObject: anElement with: newElement
{
  unsigned i;

  if (anElement == nil || dataPtr == (id *)0)
    return self;
  for (i = 0; i < numElements; i++)
    if (dataPtr[i] == anElement)
      {
        dataPtr[i] = newElement;
        break;
      }
  return self;
}

- replaceObjectAt: (unsigned) index with: newElement
{
  if (index >= numElements)
    return self;
  dataPtr[index] = newElement;
  return self;
}

/* -----------------------------------------------------------------------
 * makeObjectsPerform:
 * ----------------------------------------------------------------------- */

- makeObjectsPerform: (SEL) aSel
{
  unsigned i;

  if (dataPtr == (id *)0)
    return self;
  for (i = 0; i < numElements; i++)
    if (dataPtr[i] != nil && [dataPtr[i] respondsTo: aSel])
      [dataPtr[i] perform: aSel];
  return self;
}

- makeObjectsPerform: (SEL) aSel with: anObj
{
  unsigned i;

  if (dataPtr == (id *)0)
    return self;
  for (i = 0; i < numElements; i++)
    if (dataPtr[i] != nil && [dataPtr[i] respondsTo: aSel])
      [dataPtr[i] perform: aSel with: anObj];
  return self;
}

@end

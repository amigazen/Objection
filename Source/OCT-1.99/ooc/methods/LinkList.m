/*
 * LinkList.m -- Linked list of objects for OCT.
 */

#include <objc/LinkList.h>
#include <objc/objc-runtime.h>
#include <string.h>

@implementation LinkList : Object

/* -----------------------------------------------------------------------
 * free, freeContents, freeObjects
 * ----------------------------------------------------------------------- */

- free
{
  first = nil;
  current = nil;
  last = nil;
  return [super free];
}

- freeContents
{
  first = nil;
  current = nil;
  last = nil;
  return self;
}

- freeObjects
{
  id node;

  for (node = first; node != nil; )
    {
      id next = [node next];
      [node free];
      node = next;
    }
  first = nil;
  current = nil;
  last = nil;
  return self;
}

/* -----------------------------------------------------------------------
 * add: (at end), addFirst:, addLast:, addTail:
 * ----------------------------------------------------------------------- */

- add: anItem
{
  return [self addLast: anItem];
}

- addFirst: anItem
{
  if (anItem == nil)
    return self;
  [anItem setNext: first];
  first = anItem;
  if (last == nil)
    last = anItem;
  return self;
}

- addLast: anItem
{
  if (anItem == nil)
    return self;
  if (last == nil)
    first = last = anItem;
  else
    {
      [last setNext: anItem];
      last = anItem;
    }
  [last setNext: nil];
  return self;
}

- addTail: anItem
{
  return [self addLast: anItem];
}

/* -----------------------------------------------------------------------
 * remove: (by object), removeFirst, removeLast, remove (current)
 * ----------------------------------------------------------------------- */

- remove: anObject
{
  id prev, node;

  if (anObject == nil || first == nil)
    return nil;
  if (first == anObject)
    {
      first = [first next];
      if (first == nil)
        last = nil;
      if (current == anObject)
        current = first;
      return anObject;
    }
  prev = first;
  for (node = [first next]; node != nil; prev = node, node = [node next])
    if (node == anObject)
      {
        [prev setNext: [node next]];
        if (last == node)
          last = prev;
        if (current == node)
          current = [node next];
        return anObject;
      }
  return nil;
}

- removeFirst
{
  id old;

  if (first == nil)
    return nil;
  old = first;
  first = [first next];
  if (first == nil)
    last = nil;
  if (current == old)
    current = first;
  return old;
}

- removeLast
{
  id node, prev;

  if (last == nil)
    return nil;
  if (first == last)
    {
      node = first;
      first = last = nil;
      current = nil;
      return node;
    }
  prev = nil;
  for (node = first; node != last; node = [node next])
    prev = node;
  [prev setNext: nil];
  last = prev;
  if (current == node)
    current = nil;
  return node;
}

- remove
{
  id old;

  if (current == nil)
    return nil;
  old = current;
  current = [current next];
  return [self remove: old];
}

/* -----------------------------------------------------------------------
 * toFirst, first, next, toLast
 * ----------------------------------------------------------------------- */

- toFirst
{
  current = first;
  return self;
}

- first
{
  return first;
}

- next
{
  id result;

  if (current == nil)
    return nil;
  result = [current next];
  current = result;
  return result;
}

- toLast
{
  current = last;
  return self;
}

/* -----------------------------------------------------------------------
 * eachElementPerform: with:
 * ----------------------------------------------------------------------- */

- eachElementPerform: (SEL) sel with: arg
{
  id node;

  for (node = first; node != nil; node = [node next])
    if ([node respondsTo: sel])
      [node perform: sel with: arg];
  return self;
}

/* -----------------------------------------------------------------------
 * find:, findMatching:, findSTR:
 * ----------------------------------------------------------------------- */

- find: anObject
{
  id node;

  for (node = first; node != nil; node = [node next])
    if (node == anObject)
      return node;
  return nil;
}

- findMatching: anObject
{
  id node;

  if (anObject == nil)
    return nil;
  for (node = first; node != nil; node = [node next])
    if ([node isEqual: anObject])
      return node;
  return nil;
}

- findSTR: (STR) aStr
{
  id node;
  STR s;

  if (aStr == (STR)0)
    return nil;
  for (node = first; node != nil; node = [node next])
    if ([node respondsTo: @selector(str)])
      {
        s = [node str];
        if (s != (STR)0 && strcmp(s, aStr) == 0)
          return node;
      }
  return nil;
}

/* -----------------------------------------------------------------------
 * isEmpty, contains:, count
 * ----------------------------------------------------------------------- */

- (BOOL) isEmpty
{
  return (BOOL)(first == nil);
}

- (BOOL) contains: anObject
{
  return (BOOL)([self find: anObject] != nil);
}

- (int) count
{
  id node;
  int n;

  n = 0;
  for (node = first; node != nil; node = [node next])
    n++;
  return n;
}

@end

/*
 * submem.c -- OCT substrate: object allocation and deallocation.
 * Implements oc_alloc, oc_dealloc, oc_realloc (vectors.h).
 * subs.c assigns these to the vector pointers at startup.
 * Object layout: [isa][fixed ivars...] then indexed ivars.
 */

#include <objc/objc.h>
#include <objc/objc-runtime.h>
#include <objc/vectors.h>
#include <stdlib.h>
#include <string.h>

id
oc_alloc_impl(Class aClass, int nBytes)
{
  char *ptr;
  unsigned int total;

  if (aClass == (Class)0)
    return (id)0;
  total = (unsigned int)(aClass->clsSizInstance + nBytes);
  ptr = (char *)malloc((size_t)total);
  if (ptr == (char *)0)
    return (id)0;
  memset(ptr, 0, (size_t)total);
  *(Class *)ptr = aClass;
  return (id)ptr;
}

id
oc_dealloc_impl(id obj)
{
  if (obj != (id)0)
    free((void *)obj);
  return (id)0;
}

id
oc_realloc_impl(id obj, int newTotalSize)
{
  char *ptr;

  if (obj == (id)0 || newTotalSize <= 0)
    return (id)0;
  ptr = (char *)realloc((void *)obj, (size_t)newTotalSize);
  if (ptr == (char *)0)
    return (id)0;
  return (id)ptr;
}

/*
 * Shallow copy: allocate same size, copy bytes, return new id.
 */
id
oc_copy_impl(id obj, int extra)
{
  id newObj;
  unsigned int total;

  if (obj == (id)0)
    return (id)0;
  total = (unsigned int)(obj->isa->clsSizInstance + extra);
  newObj = oc_alloc_impl(obj->isa, extra);
  if (newObj == (id)0)
    return (id)0;
  memcpy((void *)newObj, (const void *)obj, (size_t)total);
  return newObj;
}

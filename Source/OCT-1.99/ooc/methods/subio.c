/*
 * subio.c -- OCT substrate: oc_show, oc_error, _storeOn, _readFrom.
 * Print/error and simple object archiving hooks (vectors.h / Object.m).
 */

#include <objc/objc.h>
#include <objc/objc-runtime.h>
#include <objc/vectors.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/*
 * oc_show: produce printable form of object (e.g. [obj show] or describe).
 */
id
oc_show_impl(id obj, void *unused)
{
  (void)unused;
  if (obj == (id)0)
    {
      printf("(nil)");
      return (id)0;
    }
  if (obj->isa != (Class)0 && obj->isa->name != (char *)0)
    printf("%s", obj->isa->name);
  else
    printf("%p", (void *)obj);
  return obj;
}

/*
 * oc_error: report error and abort.
 */
id
oc_error_impl(id self, const char *fmt, ...)
{
  va_list ap;

  if (fmt != (const char *)0)
    {
      va_start(ap, fmt);
      if (self != (id)0 && self->isa != (Class)0 && self->isa->name != (char *)0)
        fprintf(stderr, "%s: ", self->isa->name);
      vfprintf(stderr, fmt, ap);
      va_end(ap);
      fputc('\n', stderr);
    }
  abort();
  return (id)0;
}

/*
 * _storeOn: write object to file (archiving). Stub: not implemented.
 */
id
_storeOn_impl(STR aFileName, id self)
{
  (void)aFileName;
  (void)self;
  return (id)0;
}

/*
 * _readFrom: read object from file. Stub: not implemented.
 */
id
_readFrom_impl(STR aFileName)
{
  (void)aFileName;
  return (id)0;
}

/* Object.m references these; assign to impls so they are non-nil. */
id (*_storeOn)(STR aFileName, id self) = (id (*)(STR, id))_storeOn_impl;
id (*_readFrom)(STR aFileName) = (id (*)(STR))_readFrom_impl;

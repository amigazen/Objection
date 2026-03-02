/*
 * submsg.c -- OCT substrate: message dispatch (_msg, _msgSuper, _msgCheck).
 * Look up IMP from class method lists and invoke. Varargs forwarding uses
 * a fixed maximum number of long-sized args (OCT generated code passes args
 * on stack; IMP is called with same layout).
 *
 * Recursion guard: if dispatch depth exceeds MAX_MSG_DEPTH we return nil
 * to avoid stack overflow / invalid instruction from runaway recursion.
 */

#include <objc/objc.h>
#include <objc/objc-runtime.h>
#include <objc/objc-class.h>
#include <stdarg.h>

#ifndef PARMS
# define PARMS(x) ()
#endif

#define MAX_MSG_DEPTH 1024
static int _msg_depth;

/*
 * Find IMP for selector in class (and superclasses). Returns (IMP)0 if not found.
 */
IMP
_msgCheck(Class cls, SEL sel)
{
  struct objc_method_list *mlist;
  int i;

  while (cls != (Class)0)
    {
      for (mlist = cls->clsSelects; mlist != (struct objc_method_list *)0; mlist = mlist->da_next)
        {
          for (i = 0; i < (int)mlist->da_count; i++)
            {
              if (mlist->da_methods[i].md_Name == sel)
                return (IMP)mlist->da_methods[i].md_Proc;
            }
        }
      cls = cls->super_class;
    }
  return (IMP)0;
}

/*
 * Dispatch message to object. Forwards up to 4 long-sized args to IMP.
 * Methods with 0-4 extra args work; IMP ignores unused trailing args.
 */
void *
_msg(void *self, SEL sel, ...)
{
  va_list ap;
  long a1, a2, a3, a4;
  IMP imp;
  id (*fn)(id, SEL, long, long, long, long);
  void *res;

  if (self == (void *)0)
    return (void *)0;
  if (_msg_depth >= MAX_MSG_DEPTH)
    return (void *)0;
  _msg_depth++;
  imp = _msgCheck((Class)((id)self)->isa, sel);
  if (imp == (IMP)0)
    {
      _msg_depth--;
      return (void *)0;
    }
  va_start(ap, sel);
  a1 = va_arg(ap, long);
  a2 = va_arg(ap, long);
  a3 = va_arg(ap, long);
  a4 = va_arg(ap, long);
  va_end(ap);
  fn = (id (*)(id, SEL, long, long, long, long))imp;
  res = (void *)(*fn)((id)self, sel, a1, a2, a3, a4);
  _msg_depth--;
  return res;
}

/*
 * Dispatch to superclass implementation. Caller passes the super class
 * and self; we look up IMP in that class and invoke.
 */
void *
_msgSuper(void *superClass, void *self, SEL sel, ...)
{
  va_list ap;
  long a1, a2, a3, a4;
  IMP imp;
  id (*fn)(id, SEL, long, long, long, long);
  void *res;

  if (self == (void *)0 || superClass == (void *)0)
    return (void *)0;
  if (_msg_depth >= MAX_MSG_DEPTH)
    return (void *)0;
  _msg_depth++;
  imp = _msgCheck((Class)superClass, sel);
  if (imp == (IMP)0)
    {
      _msg_depth--;
      return (void *)0;
    }
  va_start(ap, sel);
  a1 = va_arg(ap, long);
  a2 = va_arg(ap, long);
  a3 = va_arg(ap, long);
  a4 = va_arg(ap, long);
  va_end(ap);
  fn = (id (*)(id, SEL, long, long, long, long))imp;
  res = (void *)(*fn)((id)self, sel, a1, a2, a3, a4);
  _msg_depth--;
  return res;
}

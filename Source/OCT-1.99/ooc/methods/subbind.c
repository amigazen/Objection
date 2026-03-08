/*
 * subbind.c -- OCT substrate: selector binding, class registry, poseAs.
 * _oc_bind() runs at startup: walks _oc_registry (from prelinker), each
 * element of which is an array of class_cb ptrs (app table + lib tables
 * from --with base,amiga).  Libraries ship their own _oc_allCCB_<name>.o.
 *
 * Compile with -DSUBSTRATE so objc.h uses struct objc_class for id
 * (avoids undefined struct _PRIVATE when building substrate alone).
 */

#include <objc/objc.h>
#include <objc/objc-class.h>
#include <objc/objc-runtime.h>
#include <objc/vectors.h>
#include <string.h>

#define MAX_SELS   2048
#define MAX_CLASSES 256

/* Prelinker-generated: registry of class tables (libs then app). */
extern char **_oc_registry[];

static STR sel_strings[MAX_SELS];
static SEL sel_values[MAX_SELS];
static int sel_count;

static STR class_names[MAX_CLASSES];
static Class class_ptrs[MAX_CLASSES];
static int class_count;

static int bound;

static void bind_selectors_in_list(struct objc_method_list *mlist);
static void fill_selector_arrays_in_list(struct objc_method_list *mlist, unsigned int max_ident);
static void register_class(Class cls);

/*
 * Return SEL for a selector string; create new if not yet mapped.
 */
SEL
sel_getUid(const char *colon_name)
{
  int i;
  const char *s;

  if (colon_name == (const char *)0)
    return (SEL)0;
  s = colon_name;
  for (i = 0; i < sel_count; i++)
    if (sel_strings[i] != (STR)0 && strcmp(sel_strings[i], s) == 0)
      return (SEL)(i + 1);
  if (sel_count >= MAX_SELS)
    return (SEL)0;
  sel_strings[sel_count] = (STR)colon_name;  /* store pointer; must not free */
  sel_values[sel_count] = (SEL)(sel_count + 1);
  i = sel_count++;
  return (SEL)(i + 1);
}

STR
sel_getName(SEL sel_id)
{
  unsigned int i;

  if (sel_id == (SEL)0)
    return (STR)"";
  i = (unsigned int)sel_id;
  if (i == 0 || i > (unsigned int)sel_count)
    return (STR)"";
  return sel_strings[i - 1];
}

BOOL
sel_isMapped(SEL sel_id)
{
  unsigned int i;

  if (sel_id == (SEL)0)
    return NO;
  i = (unsigned int)sel_id;
  return (BOOL)(i > 0 && i <= (unsigned int)sel_count);
}

STR
object_getClassName(id someObj)
{
  if (someObj == (id)0)
    return (STR)"nil";
  return someObj->isa->name;
}

/*
 * Bind all selectors in a method list: set md_Name from md_String.
 */
static void
bind_selectors_in_list(struct objc_method_list *mlist)
{
  int i;

  if (mlist == (struct objc_method_list *)0)
    return;
  for (i = 0; i < (int)mlist->da_count; i++)
    {
      if (mlist->da_methods[i].md_String != (STR)0)
        mlist->da_methods[i].md_Name = sel_getUid(mlist->da_methods[i].md_String);
    }
}

/*
 * Fill selector array (da_codes) from method list.
 *
 * Generated code uses _ocSAxxx[idx] as the SEL passed to _msg; those arrays
 * are never initialized without this step.
 *
 * md_Ident is an index into da_codes. We must bounds-check against the
 * class_cb's total_selectors (which is the maximum selector index, not the
 * count).
 */
static void
fill_selector_arrays_in_list(struct objc_method_list *mlist, unsigned int max_ident)
{
  int i;
  unsigned int idx;
  SEL *arr;

  if (mlist == (struct objc_method_list *)0 || mlist->da_codes == (SEL *)0)
    return;
  arr = (SEL *)mlist->da_codes;
  for (i = 0; i < (int)mlist->da_count; i++)
    {
      if (mlist->da_methods[i].md_String != (STR)0)
        {
          idx = (unsigned int)mlist->da_methods[i].md_Ident;
          if (idx <= max_ident)
            arr[idx] = mlist->da_methods[i].md_Name;
        }
    }
}

static void
register_class(Class cls)
{
  if (cls == (Class)0 || cls->name == (char *)0)
    return;
  if (class_count >= MAX_CLASSES)
    return;
  class_names[class_count] = cls->name;
  class_ptrs[class_count] = cls;
  class_count++;
}

/*
 * Walk _oc_registry (each entry an array of class_cb ptrs), bind selectors
 * and register classes.  Order: base lib, amiga lib, ..., app table.
 */
void
_oc_bind(void)
{
  int r;
  int i;
  char **arr;
  struct class_cb *cb;

  if (bound)
    return;
  bound = 1;
  sel_count = 0;
  class_count = 0;
  for (r = 0; _oc_registry[r] != (char **)0; r++)
    {
      arr = _oc_registry[r];
      if (arr == (char **)0)
        continue;
      for (i = 0; arr[i] != (char *)0; i++)
        {
          cb = (struct class_cb *)arr[i];
          if (cb == (struct class_cb *)0)
            continue;
          bind_selectors_in_list(cb->fact_sels);
          bind_selectors_in_list(cb->inst_sels);
          bind_selectors_in_list((struct objc_method_list *)cb->ref_methods);
          fill_selector_arrays_in_list(cb->fact_sels, (unsigned int)cb->total_selectors);
          fill_selector_arrays_in_list(cb->inst_sels, (unsigned int)cb->total_selectors);
          fill_selector_arrays_in_list((struct objc_method_list *)cb->ref_methods, (unsigned int)cb->total_selectors);
          if (cb->instance != (Class)0)
            {
              register_class(cb->instance);
              if (cb->instance->isa != (Class)0)
                register_class(cb->instance->isa);
            }
        }
    }
}

Class
objc_getClass(const char *className)
{
  int i;

  if (className == (const char *)0)
    return (Class)0;
  if (!bound)
    _oc_bind();
  for (i = 0; i < class_count; i++)
    if (class_names[i] != (const char *)0 && strcmp(class_names[i], className) == 0)
      return class_ptrs[i];
  return (Class)0;
}

/*
 * Meta class: factory. For simplicity return the same as class if no separate factory.
 */
Class
objc_getMetaClass(const char *className)
{
  Class cls;

  cls = objc_getClass(className);
  if (cls == (Class)0)
    return (Class)0;
  if (cls->isa != (Class)0)
    return cls->isa;
  return cls;
}

/*
 * Vector implementations: string -> Class, string -> SEL.
 */
id
oc_cvtToId_impl(const char *className)
{
  return (id)objc_getClass(className);
}

SEL
oc_cvtToSel_impl(const char *selName)
{
  return sel_getUid(selName);
}

/*
 * poseAs: make aClass pose as another. Stub: no-op (full impl would swap dispatch).
 */
void
poseAs(Class aClass, Class poseAsClass)
{
  (void)aClass;
  (void)poseAsClass;
}

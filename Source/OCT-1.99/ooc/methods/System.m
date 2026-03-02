/*
 * System.m -- System utilities for OCT (class methods only, no ivars).
 * abort:, initCache, dumpDictionary:, beep, getProfileString:, putProfileString:data:.
 */

#include <objc/System.h>
#include <objc/objc-runtime.h>
#include <objc/objc-class.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROFILE_ENTRIES  32
static struct { char *key; char *value; } profileStore[MAX_PROFILE_ENTRIES];
static int profileCount;

@implementation System : Object

/* -----------------------------------------------------------------------
 * abort:
 * ----------------------------------------------------------------------- */

+ abort: (int) rc
{
  exit(rc);
  return (id)0;
}

/* -----------------------------------------------------------------------
 * initCache (method caches; no-op or init cache structures)
 * ----------------------------------------------------------------------- */

+ initCache
{
  return (id)self;
}

/* -----------------------------------------------------------------------
 * dumpDictionary: (print methods for aClass)
 * ----------------------------------------------------------------------- */

+ dumpDictionary: (Class) aClass
{
  struct objc_method_list *mlist;
  int i;

  if (aClass == (Class)0)
    return (id)self;
  printf("Methods for class %s:\n", aClass->name ? aClass->name : "(null)");
  for (mlist = aClass->clsSelects; mlist != (struct objc_method_list *)0; mlist = mlist->da_next)
    for (i = 0; i < (int)mlist->da_count; i++)
      if (mlist->da_methods[i].md_String != (STR)0)
        printf("  %s\n", mlist->da_methods[i].md_String);
  return (id)self;
}

/* -----------------------------------------------------------------------
 * beep (stub; platform-specific)
 * ----------------------------------------------------------------------- */

+ beep
{
  return (id)self;
}

/* -----------------------------------------------------------------------
 * getProfileString:, putProfileString: data:
 * ----------------------------------------------------------------------- */

+ getProfileString: (STR) key
{
  int i;

  if (key == (STR)0)
    return (STR)0;
  for (i = 0; i < profileCount; i++)
    if (profileStore[i].key != (char *)0 && strcmp(profileStore[i].key, key) == 0)
      return (STR)profileStore[i].value;
  return (STR)0;
}

+ putProfileString: (STR) key data: (STR) value
{
  int i;

  if (key == (STR)0)
    return (id)self;
  for (i = 0; i < profileCount; i++)
    if (profileStore[i].key != (char *)0 && strcmp(profileStore[i].key, key) == 0)
      {
        profileStore[i].value = value != (STR)0 ? value : (STR)"";
        return (id)self;
      }
  if (profileCount < MAX_PROFILE_ENTRIES)
    {
      profileStore[profileCount].key = key;
      profileStore[profileCount].value = value != (STR)0 ? value : (STR)"";
      profileCount++;
    }
  return (id)self;
}

@end

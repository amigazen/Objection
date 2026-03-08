/*
 * oc_exit.c -- Call the real C library exit.
 *
 * objc.h #defines exit to app_exit when SUBSTRATE is set. Substrate code
 * (subs.c, stackovf.c) that includes objc.h must not call exit() directly
 * or it would recurse through app_exit. This file does NOT include objc.h,
 * so it can call the real exit() from stdlib.h.
 */

#include <stdlib.h>

void
_oc_real_exit(int status)
{
  exit(status);
}

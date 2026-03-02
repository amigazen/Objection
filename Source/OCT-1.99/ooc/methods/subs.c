/*
 * subs.c -- OCT substrate: vector initialization, _oc_errfmt, _oc_atExit, main.
 * Assigns vectors (oc_alloc, oc_dealloc, etc.) to implementations in submem,
 * subio, subbind. Defines main() to call _oc_bind() then app_main().
 */

#include <objc/objc.h>
#include <objc/objc-runtime.h>
#include <objc/vectors.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#ifndef PARMS
# define PARMS(x) ()
#endif

/* Implementations from submem.c */
extern id oc_alloc_impl(Class aClass, int nBytes);
extern id oc_dealloc_impl(id obj);
extern id oc_realloc_impl(id obj, int newTotalSize);
extern id oc_copy_impl(id obj, int extra);

/* Implementations from subio.c */
extern id oc_show_impl(id obj, void *unused);
extern id oc_error_impl(id self, const char *fmt, ...);

/* Implementations from subbind.c */
extern id oc_cvtToId_impl(const char *className);
extern SEL oc_cvtToSel_impl(const char *selName);

/* From subbind.c */
extern void _oc_bind(void);

/* Vector pointers (declared in vectors.h); assigned in init_vectors(). */
id (*oc_alloc)(Class, int) = (id (*)(Class, int))0;
id (*oc_dealloc)(id) = (id (*)(id))0;
id (*oc_realloc)(id, int) = (id (*)(id, int))0;
id (*oc_copy)(id, int) = (id (*)(id, int))0;
id (*oc_show)(id, void *) = (id (*)(id, void *))0;
id (*oc_error)(id, const char *, ...) = (id (*)(id, const char *, ...))0;
id (*oc_cvtToId)(const char *) = (id (*)(const char *))0;
SEL (*oc_cvtToSel)(const char *) = (SEL (*)(const char *))0;

char *_progName = (char *)0;

#define MAX_ATEXIT 32
static IMP atexit_callbacks[MAX_ATEXIT];
static int atexit_count;
static int atexit_running;

void
_oc_errfmt(const char *fmt, ...)
{
  va_list ap;

  if (fmt != (const char *)0)
    {
      va_start(ap, fmt);
      vfprintf(stderr, fmt, ap);
      va_end(ap);
      fputc('\n', stderr);
    }
}

void
_oc_atExit(IMP callback)
{
  if (callback != (IMP)0 && atexit_count < MAX_ATEXIT)
    atexit_callbacks[atexit_count++] = callback;
}

static void
run_atexit(void)
{
  int i;

  if (atexit_running)
    return;
  atexit_running = 1;
  for (i = atexit_count - 1; i >= 0; i--)
    if (atexit_callbacks[i] != (IMP)0)
      (*atexit_callbacks[i])((id)0, (SEL)0);
}

/*
 * Assign all vector pointers to substrate implementations.
 */
static void
init_vectors(void)
{
  oc_alloc = (id (*)(Class, int))oc_alloc_impl;
  oc_dealloc = (id (*)(id))oc_dealloc_impl;
  oc_realloc = (id (*)(id, int))oc_realloc_impl;
  oc_copy = (id (*)(id, int))oc_copy_impl;
  oc_show = (id (*)(id, void *))oc_show_impl;
  oc_error = (id (*)(id, const char *, ...))oc_error_impl;
  oc_cvtToId = (id (*)(const char *))oc_cvtToId_impl;
  oc_cvtToSel = (SEL (*)(const char *))oc_cvtToSel_impl;
}

/*
 * Application entry: init vectors, bind selectors/classes, run app_main.
 * app_exit: application code calls exit() which objc.h #defines to app_exit.
 * Must call real libc exit via _oc_real_exit to avoid infinite recursion.
 */
extern int app_main(int argc, char *argv[]);
extern void _oc_real_exit(int status);

void
app_exit(int status)
{
  _oc_real_exit(status);
}

int
main(int argc, char *argv[])
{
  if (argv != (char **)0 && argv[0] != (char *)0)
    _progName = argv[0];
  init_vectors();
  _oc_bind();
  atexit(run_atexit);
  return app_main(argc, argv);
}

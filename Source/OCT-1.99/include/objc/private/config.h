/*
 * config.h -- Site-specific defines for OCT (xlator and prelink).
 * Environment variable names and cpp program path. Create/customize
 * when building OCT for your system.
 */

#ifndef _OBJC_PRIVATE_CONFIG_H
#define _OBJC_PRIVATE_CONFIG_H  1

/* -----------------------------------------------------------------------
 * Environment variable names (used by oct and optionally by prelink).
 * Match the Makefile names (OOCINC, etc.) so one set of env vars works
 * for both make and the oct binary.
 * ----------------------------------------------------------------------- */

/* Where to put cpp temporary output. Amiga: T: or RAM: */
#ifndef EV_TEMPDIR
#define EV_TEMPDIR  "TEMP"
#endif

/* Extra -I dirs for cpp (same as Makefile OOCINC). Separator: ; or ! (Amiga), : (Unix). */
#ifndef EV_INCLUDE
#define EV_INCLUDE  "OOCINC"
#endif

/* Extra args passed through to cpp (e.g. -DNO_PROTOTYPE). ReadMe calls this OCTCPP. */
#ifndef EV_CPP
#define EV_CPP  "OCTCPP"
#endif

/* Extra oct options (as if on command line). ReadMe calls this OCTOPTS. */
#ifndef EV_OPTS
#define EV_OPTS  "OCTOPTS"
#endif

/* -----------------------------------------------------------------------
 * C preprocessor program. oct runs this via system().
 * Use full path if cpp is not on PATH.
 * ----------------------------------------------------------------------- */
#ifndef CPP_PNAME
#define CPP_PNAME  "cpp"
#endif

/* -----------------------------------------------------------------------
 * Prelink only: file listing extra modules to register (one per line).
 * Undef to disable.
 * ----------------------------------------------------------------------- */
/* #define MUST_FNAME  "mustinit.ooc" */

/* -----------------------------------------------------------------------
 * Default cpp arguments string (used by oct main.c to init preproc_opts).
 * Empty string if no default args.
 * ----------------------------------------------------------------------- */
#ifndef CPP_ARGS
#define CPP_ARGS  ""
#endif

/* -----------------------------------------------------------------------
 * Null device path when -n (no output) is used. Amiga: "NIL:"; Unix: "/dev/null".
 * ----------------------------------------------------------------------- */
#ifndef NULL_FNAME
#define NULL_FNAME  "NIL:"
#endif

#endif

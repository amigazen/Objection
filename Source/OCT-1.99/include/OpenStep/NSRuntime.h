/*
 * NSRuntime.h -- Amiga-native string and format (dos.library, exec.library).
 * This project is Amiga-only; no ifdefs. Use instead of libc string/stdio.
 */

#ifndef _OPENSTEP_NSRUNTIME_H
# define _OPENSTEP_NSRUNTIME_H 1

# include <exec/types.h>

/* String: no libc. Use exec CopyMem for copy; length and compare via these. */
unsigned int OpenStep_Strlen(const char *s);
int OpenStep_Strcmp(const char *a, const char *b);
void OpenStep_Strcpy(char *dst, const char *src);

/* Formatted output: exec RawDoFmt. Format uses %ld/%s/%lc etc. (RawDoFmt rules). */
void OpenStep_VSprintf(char *buf, const char *fmt, void *argList);
void OpenStep_Printf(const char *fmt, ...);

/* Double to string (RawDoFmt has no %f). Writes at most bufSize chars including NUL. */
void OpenStep_FormatDouble(char *buf, unsigned int bufSize, double d);

#endif

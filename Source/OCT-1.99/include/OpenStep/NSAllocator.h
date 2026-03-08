/*
 * NSAllocator.h -- Amiga-native allocation (exec.library) per SDK autodocs.
 * This project is Amiga-only. AllocVec, FreeVec, CopyMem; realloc via
 * AllocVec+CopyMem+FreeVec.
 */

#ifndef _OPENSTEP_NSALLOCATOR_H
# define _OPENSTEP_NSALLOCATOR_H 1

# include <exec/types.h>
# include <exec/memory.h>

# define OPENSTEP_ALLOC(s)         AllocVec((unsigned long)(s), MEMF_PUBLIC | MEMF_CLEAR)
# define OPENSTEP_FREE(p)         do { if (p) { FreeVec((p)); (p) = (void *)0; } } while (0)
# define OPENSTEP_FREE_VOID(p)     do { if (p) FreeVec((p)); } while (0)
# define OPENSTEP_REALLOC_KNOWN(p, oldSize, newSize) OpenStep_ReallocAmiga((p), (unsigned long)(oldSize), (unsigned long)(newSize))
# define OPENSTEP_COPY(dest, src, n) CopyMem((src), (dest), (unsigned long)(n))
# define OPENSTEP_ZERO(ptr, n)     OpenStep_ZeroMem((ptr), (unsigned long)(n))

extern void *OpenStep_ReallocAmiga(void *oldPtr, unsigned long oldSize, unsigned long newSize);
extern void OpenStep_ZeroMem(void *ptr, unsigned long n);

#endif

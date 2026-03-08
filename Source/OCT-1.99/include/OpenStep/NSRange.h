/*
 * NSRange.h -- OpenStep NSRange structure and inline helpers.
 */

#ifndef _OPENSTEP_NSRANGE_H
# define _OPENSTEP_NSRANGE_H 1

# include <objc/objc.h>

typedef struct NSRange {
  unsigned int location;
  unsigned int length;
} NSRange;

/* Sentinel for "not found" (e.g. indexGreaterThanIndex: when no next). */
# define NSNotFound ((unsigned int)-1)

/* NSRange constant and inline helpers (macros for C89). */
# define NSMakeRange(loc, len) ((NSRange){ (unsigned int)(loc), (unsigned int)(len) })
# define NSMaxRange(r)         ((r).location + (r).length)
# define NSLocationInRange(loc, r) ((unsigned int)(loc) >= (r).location && (unsigned int)(loc) < (r).location + (r).length)
# define NSEqualRanges(r1, r2) ((r1).location == (r2).location && (r1).length == (r2).length)

#endif

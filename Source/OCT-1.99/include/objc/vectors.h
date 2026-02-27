/*   VECTORS.H    Substrate indirection hooks               Sept 90, bjw */
/*	:ts=8	*/

/*
 *    This file contains the jump vectors used by the OCT substrate.
 *    These hooks should really be referenced by their front-end
 *    calling routines.
 *
 *    15-sept-90   bjw     Riped from substrate.h for NeXT-likeness.
 *    7-aug-91    bjw/db   Uses Std-Arg for oc_error().
 */

#include <objc/objc.h>

#ifndef _OCT_VECTORS
# define  _OCT_VECTORS  1

#include <octhead.h>


extern	id    (*oc_alloc)( );      /* Alloc memory to hold object */
extern	id    (*oc_dealloc)( );    /* Release memory holding self */
extern	id    (*oc_realloc)( );    /* Repositions object for new size */
extern	id    (*oc_copy)( );       /* Shallow copy self's instance vars */

extern	id    (*oc_cvtToId)( );    /* string to factory class ID */
extern  SEL   (*oc_cvtToSel)( );   /* convert string to selector */
extern  id    (*oc_show)( );       /* Produce printable form */

#if USE_STDARG
extern	id    (*oc_error)( id, const char *, ... );
#else
extern	id    (*oc_error)( /* var-arg */ );  /* Toss the cookies and die! */
#endif

#endif

/* end of vectors.h */

